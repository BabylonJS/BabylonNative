#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include "../../../Core/Graphics/Source/DeviceImpl.h"

#include <chrono>
#include <future>
#include <thread>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace Babylon::Graphics
{
    struct DeviceFramePumpTestAccess
    {
        static bool WaitForFlushRequest(DeviceImpl& device)
        {
            std::unique_lock lock{device.m_frameSyncMutex};
            return device.m_frameSyncCV.wait_for(lock, std::chrono::seconds{10}, [&] {
                return device.m_flushRequested;
            });
        }

        static void WaitForPump(DeviceImpl& device)
        {
            std::unique_lock lock{device.m_frameSyncMutex};
            const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{10};
            while (!device.m_pumpingFrameRequests && std::chrono::steady_clock::now() < deadline)
            {
                device.m_frameSyncCV.wait_for(lock, std::chrono::milliseconds{1});
            }
            EXPECT_TRUE(device.m_pumpingFrameRequests);
        }
    };
}

namespace
{
    void ReachFlushThreshold(Babylon::Graphics::DeviceContext& context)
    {
        const auto threshold = bgfx::getCaps()->limits.maxViews - 16;
        while (context.PeekNextViewId() < threshold)
        {
            context.AcquireNewViewId();
        }
    }
}

TEST(DeviceFramePump, PlainHostWaitBlocksUntilTheHostServicesTheFlush)
{
    using namespace Babylon::Graphics;
    DeviceImpl device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
    auto& context = device.GetContext();
    std::promise<void> completed;
    auto completion = completed.get_future();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&, scope = context.AcquireFrameCompletionScope()](Napi::Env) {
        const auto generation = context.ViewIdGeneration();
        ReachFlushThreshold(context);
        context.FlushViewsIfNeeded();
        EXPECT_GT(context.ViewIdGeneration(), generation);
        EXPECT_NE(context.GetActiveEncoder(), nullptr);
        completed.set_value();
    });

    // The host has an open frame and JS holds a scope, but neither makes a plain
    // future wait capable of servicing the request. Rescue it instead of hanging.
    EXPECT_TRUE(DeviceFramePumpTestAccess::WaitForFlushRequest(device));
    EXPECT_EQ(completion.wait_for(std::chrono::milliseconds{100}), std::future_status::timeout);
    device.FinishRenderingCurrentFrame();
    EXPECT_EQ(completion.wait_for(std::chrono::seconds{10}), std::future_status::ready);
}

TEST(DeviceFramePump, SynchronousWaitServicesFlushesBeforeAndAfterPumpEntry)
{
    using namespace Babylon::Graphics;
    DeviceImpl device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
    auto& context = device.GetContext();
    Babylon::AppRuntime runtime{};
    size_t finishedFrames{};
    arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation::none(), [&] { ++finishedFrames; });

    for (const bool requestBeforePump : {true, false})
    {
        SCOPED_TRACE(requestBeforePump ? "request before pump" : "request after pump");
        const auto generation = context.ViewIdGeneration();
        device.DispatchAndWait([&](auto callback) {
            runtime.Dispatch(std::move(callback));
            if (requestBeforePump)
            {
                EXPECT_TRUE(DeviceFramePumpTestAccess::WaitForFlushRequest(device));
            }
        }, [&](Napi::Env) {
            if (!requestBeforePump)
            {
                DeviceFramePumpTestAccess::WaitForPump(device);
            }
            for (size_t flush = 0; flush < 3; ++flush)
            {
                ReachFlushThreshold(context);
                context.FlushViewsIfNeeded();
                EXPECT_EQ(context.ViewIdGeneration(), generation + flush + 1);
                EXPECT_EQ(context.PeekNextViewId(), 0u);
                EXPECT_NE(context.GetActiveEncoder(), nullptr);
            }
        });
        EXPECT_EQ(finishedFrames, 0u);
        EXPECT_NE(context.GetActiveEncoder(), nullptr);
    }
    device.FinishRenderingCurrentFrame();
    EXPECT_EQ(finishedFrames, 1u);
    EXPECT_EQ(context.GetActiveEncoder(), nullptr);
}

TEST(DeviceFramePump, CompletionBeforeWaitAndCallbackFailuresLeaveFrameUsable)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
    Babylon::AppRuntime runtime{};
    const auto dispatch = [&](auto callback) { runtime.Dispatch(std::move(callback)); };
    size_t callbacks{};
    device.DispatchAndWait([&](auto callback) {
        std::promise<void> completed;
        auto completion = completed.get_future();
        runtime.Dispatch([&, callback = std::move(callback)](Napi::Env env) {
            callback(env);
            completed.set_value();
        });
        ASSERT_EQ(completion.wait_for(std::chrono::seconds{10}), std::future_status::ready);
    }, [&](Napi::Env env) {
        device.AddToJavaScript(env);
        ++callbacks;
    });
    EXPECT_EQ(callbacks, 1u);

    EXPECT_THROW(device.DispatchAndWait(dispatch, [](Napi::Env) {
        throw std::logic_error{"native callback failure"};
    }), std::logic_error);
    try
    {
        device.DispatchAndWait(dispatch, [](Napi::Env env) {
            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            EXPECT_TRUE(context.ForceMidFrameFlush());
            throw Napi::Error::New(env, "JS callback failure");
        });
        FAIL() << "JS exception was not delivered to the host";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_NE(std::string{error.what()}.find("JS callback failure"), std::string::npos);
    }
    EXPECT_THROW(device.DispatchAndWait([](auto) {
        throw std::runtime_error{"dispatch failure"};
    }, [](Napi::Env) {}), std::runtime_error);
    EXPECT_THROW(device.DispatchAndWait(dispatch, [](Napi::Env env) {
        auto error = Napi::Error::New(env, "callback failure with throwing stack");
        auto object = env.Global().Get("Object").As<Napi::Object>();
        auto descriptor = Napi::Object::New(env);
        descriptor.Set("get", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
            throw Napi::Error::New(info.Env(), "stack getter failure");
        }));
        object.Get("defineProperty").As<Napi::Function>().Call(object, {
            error.Value(), Napi::String::New(env, "stack"), descriptor});
        throw error;
    }), std::runtime_error);

    device.DispatchAndWait(dispatch, [&](Napi::Env env) {
        ++callbacks;
        auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
        EXPECT_NE(context.GetActiveEncoder(), nullptr);
        EXPECT_TRUE(context.ForceMidFrameFlush());
    });
    EXPECT_EQ(callbacks, 2u);
    device.FinishRenderingCurrentFrame();
}

TEST(DeviceFramePump, InvalidCallsDoNotQueueWork)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    size_t queued{};
    const auto dispatch = [&](auto) { ++queued; };
    EXPECT_THROW(device.DispatchAndWait(dispatch, [](Napi::Env) {}), std::runtime_error);
    device.StartRenderingCurrentFrame();
    EXPECT_THROW(device.DispatchAndWait({}, [](Napi::Env) {}), std::invalid_argument);
    EXPECT_THROW(device.DispatchAndWait(dispatch, {}), std::invalid_argument);
    std::thread wrongThread{[&] {
        EXPECT_THROW(device.DispatchAndWait(dispatch, [](Napi::Env) {}), std::runtime_error);
    }};
    wrongThread.join();
    device.FinishRenderingCurrentFrame();
    EXPECT_THROW(device.DispatchAndWait(dispatch, [](Napi::Env) {}), std::runtime_error);
    EXPECT_EQ(queued, 0u);
}
