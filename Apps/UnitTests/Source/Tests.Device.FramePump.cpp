#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include "../../../Core/Graphics/Source/DeviceImpl.h"

#include <chrono>
#include <future>

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
        EXPECT_EQ(context.ViewIdGeneration(), generation + 1);
        EXPECT_EQ(context.PeekNextViewId(), 0u);
        EXPECT_NE(context.GetActiveEncoder(), nullptr);
        completed.set_value();
    });

    // Reproduce #1904 with a bounded wait, then let the existing frame-finish
    // pump rescue the request rather than hanging the test process.
    EXPECT_TRUE(DeviceFramePumpTestAccess::WaitForFlushRequest(device));
    EXPECT_EQ(completion.wait_for(std::chrono::milliseconds{100}), std::future_status::timeout);
    device.FinishRenderingCurrentFrame();
    EXPECT_EQ(completion.wait_for(std::chrono::seconds{10}), std::future_status::ready);
    EXPECT_EQ(context.GetActiveEncoder(), nullptr);
}

TEST(DeviceFramePump, FrameScheduledWorkFlushesWithoutAnExtraHostWait)
{
    using namespace Babylon::Graphics;
    DeviceImpl device{g_deviceConfig};
    auto& context = device.GetContext();
    Babylon::AppRuntime runtime{};
    size_t callbacks{};
    size_t finishedFrames{};

    for (size_t frame = 0; frame < 2; ++frame)
    {
        arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation::none(), [&] { ++finishedFrames; });
        arcana::make_task(context.FrameStartScheduler(), arcana::cancellation::none(), [&] {
            // Match NativeEngine's RAF path: reserve the scope before queuing JS.
            runtime.Dispatch([&, scope = context.AcquireFrameCompletionScope()](Napi::Env) {
                const auto generation = context.ViewIdGeneration();
                for (size_t flush = 0; flush < 3; ++flush)
                {
                    ReachFlushThreshold(context);
                    context.FlushViewsIfNeeded();
                    EXPECT_EQ(context.ViewIdGeneration(), generation + flush + 1);
                    EXPECT_EQ(context.PeekNextViewId(), 0u);
                    EXPECT_NE(context.GetActiveEncoder(), nullptr);
                    EXPECT_EQ(finishedFrames, frame);
                }
                ++callbacks;
            });
        });

        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();

        EXPECT_EQ(callbacks, frame + 1);
        EXPECT_EQ(finishedFrames, frame + 1);
        EXPECT_EQ(context.GetActiveEncoder(), nullptr);
    }
}
