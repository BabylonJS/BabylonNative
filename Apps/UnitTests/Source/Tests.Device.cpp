#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/FrameBuffer.h>

#ifdef HAS_TEST_UTILS
#include <Babylon/Plugins/TestUtils.h>
#endif

#ifdef HAS_NATIVE_CAPTURE
#include <Babylon/Plugins/NativeCapture.h>
#include <napi/pointer.h>
#endif

#include "Helpers.h"
#include "../../../Core/Graphics/Source/DeviceImpl.h"

#include <array>
#include <chrono>
#include <future>
#include <system_error>
#include <thread>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace Babylon::Graphics
{
    struct DeviceImplTestAccess
    {
        static void LoseDevice(DeviceImpl& device)
        {
            static_cast<bgfx::CallbackI&>(device.m_bgfxCallback).fatal(__FILE__, __LINE__, bgfx::Fatal::DeviceLost, "simulated device loss");
        }

        static void SubmitScreenshots(DeviceImpl& device)
        {
            device.RequestScreenShots();
        }

        static void FlushViews(DeviceImpl& device)
        {
            std::scoped_lock lock{device.m_frameSyncMutex};
            device.PerformMidFrameViewFlush();
        }

        static size_t PendingReadbacks(const DeviceImpl& device)
        {
            return device.m_readTextureRequests.size();
        }
    };
}

namespace
{
    void ExpectCanceled(std::exception_ptr error)
    {
        ASSERT_NE(error, nullptr);
        try
        {
            std::rethrow_exception(error);
        }
        catch (const std::system_error& canceled)
        {
            EXPECT_EQ(canceled.code(), std::errc::operation_canceled);
        }
    }
}

TEST(Device, DeviceLossCancelsReadbacksBeforeRecovery)
{
    using namespace Babylon::Graphics;
    for (bool midFrameFlush : {false, true})
    {
        SCOPED_TRACE(midFrameFlush ? "mid-frame flush" : "frame completion");
        DeviceImpl device{g_deviceConfig};
        device.EnableRendering();
        const std::array<uint8_t, 4> pixels{1, 2, 3, 255};
        const auto texture = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_READ_BACK, bgfx::copy(pixels.data(), static_cast<uint32_t>(pixels.size())));
        ASSERT_TRUE(bgfx::isValid(texture));

        device.StartRenderingCurrentFrame();
        std::array<uint8_t, 4> output{};
        size_t readsCanceled{};
        device.ReadTextureAsync(texture, output, 0)
            .then(arcana::inline_scheduler, arcana::cancellation::none(), [&](const arcana::expected<void, std::exception_ptr>& result) {
                ++readsCanceled;
                ASSERT_TRUE(result.has_error());
                ExpectCanceled(result.error());
            });

        size_t screenshotsCanceled{};
        const auto onScreenshot = [&](BgfxCallback::ScreenShotResult result) {
            ++screenshotsCanceled;
            ASSERT_TRUE(result.has_error());
            ExpectCanceled(result.error());
        };
        device.RequestScreenShot(onScreenshot);
        DeviceImplTestAccess::SubmitScreenshots(device);
        device.RequestScreenShot(onScreenshot);
        size_t reentrantCanceled{};
        device.RequestScreenShot([&](BgfxCallback::ScreenShotResult result) {
            ASSERT_TRUE(result.has_error());
            device.RequestScreenShot([&](BgfxCallback::ScreenShotResult nested) {
                ++reentrantCanceled;
                ASSERT_TRUE(nested.has_error());
                ExpectCanceled(nested.error());
            });
        });
        std::atomic<size_t> concurrentCanceled{};
        std::thread submitter{[&] {
            for (size_t request = 0; request < 32; ++request)
            {
                device.RequestScreenShot([&](BgfxCallback::ScreenShotResult result) {
                    ++concurrentCanceled;
                    ASSERT_TRUE(result.has_error());
                    ExpectCanceled(result.error());
                });
            }
        }};
        DeviceImplTestAccess::LoseDevice(device);

        // New requests must fail without issuing another GPU read or waiting for a frame.
        device.RequestScreenShot(onScreenshot);
        EXPECT_EQ(screenshotsCanceled, 1u);
        device.ReadTextureAsync(BGFX_INVALID_HANDLE, output, 0)
            .then(arcana::inline_scheduler, arcana::cancellation::none(), [&](const arcana::expected<void, std::exception_ptr>& result) {
                ++readsCanceled;
                ASSERT_TRUE(result.has_error());
                ExpectCanceled(result.error());
            });
        EXPECT_EQ(readsCanceled, 1u);
        EXPECT_EQ(output, (std::array<uint8_t, 4>{}));

        if (midFrameFlush)
        {
            DeviceImplTestAccess::FlushViews(device);
            EXPECT_EQ(readsCanceled, 2u);
            EXPECT_EQ(screenshotsCanceled, 3u);
        }
        device.FinishRenderingCurrentFrame();
        submitter.join();
        EXPECT_EQ(readsCanceled, 2u);
        EXPECT_EQ(screenshotsCanceled, 3u);
        EXPECT_EQ(reentrantCanceled, 1u);
        EXPECT_EQ(concurrentCanceled, 32u);

        bgfx::destroy(texture);
        device.DisableRendering();
        device.EnableRendering();
        EXPECT_FALSE(device.IsDeviceLost());
        size_t recoveredScreenshots{};
        device.RequestScreenShot([&](BgfxCallback::ScreenShotResult result) {
            ++recoveredScreenshots;
            ASSERT_FALSE(result.has_error());
            EXPECT_FALSE(result.value().empty());
        });
        for (size_t frame = 0; frame < 3 && recoveredScreenshots == 0; ++frame)
        {
            device.StartRenderingCurrentFrame();
            FrameBuffer backBuffer{device.GetContext(), BGFX_INVALID_HANDLE, 0, 0, true, true, true};
            backBuffer.Clear(*device.GetActiveEncoder(), BGFX_CLEAR_COLOR, 0x123456ff, 1.0f, 0);
            device.FinishRenderingCurrentFrame();
        }
        EXPECT_EQ(recoveredScreenshots, 1u);
        EXPECT_EQ(screenshotsCanceled, 3u);
        EXPECT_EQ(readsCanceled, 2u);
    }
}

#ifdef HAS_TEST_UTILS
TEST(Device, ScreenshotCancellationReachesJavaScript)
{
    using namespace Babylon::Graphics;
    DeviceImpl device{g_deviceConfig};
    device.EnableRendering();
    std::promise<void> queued;
    std::promise<void> rejected;
    std::promise<void> unhandled;
    std::atomic<size_t> successes{};
    std::atomic<size_t> failures{};
    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [&](const Napi::Error& error) {
        EXPECT_FALSE(error.Message().empty());
        unhandled.set_value();
    };
    Babylon::AppRuntime runtime{options};
    runtime.Dispatch([&](Napi::Env env) {
        device.AddToJavaScript(env);
        Babylon::Plugins::TestUtils::Initialize(env, g_deviceConfig.Window);
        auto testUtils = env.Global().Get("TestUtils").As<Napi::Object>();
        auto screenshot = testUtils.Get("getFrameBufferData").As<Napi::Function>();
        auto onSuccess = Napi::Function::New(env, [&](const Napi::CallbackInfo&) { ++successes; });
        auto onError = Napi::Function::New(env, [&](const Napi::CallbackInfo& info) {
            ++failures;
            EXPECT_EQ(info[0].As<Napi::Object>().Get("message").As<Napi::String>().Utf8Value(),
                std::system_error(std::make_error_code(std::errc::operation_canceled)).what());
            rejected.set_value();
        });
        screenshot.Call(testUtils, {onSuccess, onError});
        screenshot.Call(testUtils, {onSuccess});
        queued.set_value();
    });
    ASSERT_EQ(queued.get_future().wait_for(std::chrono::seconds{10}), std::future_status::ready);
    DeviceImplTestAccess::LoseDevice(device);
    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();
    EXPECT_EQ(rejected.get_future().wait_for(std::chrono::seconds{10}), std::future_status::ready);
    EXPECT_EQ(unhandled.get_future().wait_for(std::chrono::seconds{10}), std::future_status::ready);
    EXPECT_EQ(successes, 0u);
    EXPECT_EQ(failures, 1u);
    device.DisableRendering();
}
#endif

#ifdef HAS_NATIVE_CAPTURE
TEST(Device, OffscreenCaptureStopsAfterDeviceLoss)
{
    using namespace Babylon::Graphics;
    DeviceImpl device{g_deviceConfig};
    device.EnableRendering();
    const auto texture = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
    device.GetContext().AddTexture(texture, 1, 1, false, 1, bgfx::TextureFormat::RGBA8);
    const auto frameBufferHandle = bgfx::createFrameBuffer(1, &texture, true);
    FrameBuffer frameBuffer{device.GetContext(), frameBufferHandle, 1, 1, false, false, false};
    std::promise<void> initialized;
    std::promise<void> disposed;
    std::atomic<size_t> capturedFrames{};
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env env) {
        device.AddToJavaScript(env);
        Babylon::Plugins::NativeCapture::Initialize(env);
        auto capture = env.Global().Get("NativeCapture").As<Napi::Function>().New({
            Napi::Pointer<FrameBuffer>::Create(env, &frameBuffer)});
        capture.Get("addCallback").As<Napi::Function>().Call(capture, {
            Napi::Function::New(env, [&](const Napi::CallbackInfo&) { ++capturedFrames; })});
        env.Global().Set("capture", capture);
        initialized.set_value();
    });
    ASSERT_EQ(initialized.get_future().wait_for(std::chrono::seconds{10}), std::future_status::ready);
    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();
    EXPECT_GT(DeviceImplTestAccess::PendingReadbacks(device), 0u);

    DeviceImplTestAccess::LoseDevice(device);
    for (size_t frame = 0; frame < 3; ++frame)
    {
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();
        EXPECT_EQ(DeviceImplTestAccess::PendingReadbacks(device), 0u);
    }
    EXPECT_EQ(capturedFrames, 0u);
    device.GetContext().RemoveTexture(texture);
    frameBuffer.Dispose();
    device.DisableRendering();
    device.EnableRendering();

    // Reuse the old texture indices. Disposing the old capture must not destroy either.
    const std::array<uint8_t, 4> pixels{1, 2, 3, 255};
    std::array<bgfx::TextureHandle, 2> replacements;
    for (auto& replacement : replacements)
    {
        replacement = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_READ_BACK, bgfx::copy(pixels.data(), static_cast<uint32_t>(pixels.size())));
    }
    for (size_t frame = 0; frame < 3; ++frame)
    {
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();
        EXPECT_EQ(DeviceImplTestAccess::PendingReadbacks(device), 0u);
    }
    runtime.Dispatch([&](Napi::Env env) {
        auto capture = env.Global().Get("capture").As<Napi::Object>();
        capture.Get("dispose").As<Napi::Function>().Call(capture, {});
        disposed.set_value();
    });
    ASSERT_EQ(disposed.get_future().wait_for(std::chrono::seconds{10}), std::future_status::ready);
    for (const auto replacement : replacements)
    {
        std::array<uint8_t, 4> output{};
        bool completed{};
        device.ReadTextureAsync(replacement, output, 0)
            .then(arcana::inline_scheduler, arcana::cancellation::none(), [&](const arcana::expected<void, std::exception_ptr>& result) {
                completed = true;
                EXPECT_FALSE(result.has_error());
            });
        for (size_t frame = 0; frame < 3 && !completed; ++frame)
        {
            device.StartRenderingCurrentFrame();
            device.FinishRenderingCurrentFrame();
        }
        EXPECT_TRUE(completed);
        EXPECT_EQ(output, pixels);
        bgfx::destroy(replacement);
    }
    EXPECT_EQ(capturedFrames, 0u);
}
#endif

TEST(Device, HeadlessNoopDoesNotReportDeviceLoss)
{
    Babylon::Graphics::Configuration config{};
    Babylon::Graphics::Device device{config};
    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();
    EXPECT_FALSE(device.IsDeviceLost());
}

// Verifies UpdateDevice replaces the active graphics device after a DisableRendering / EnableRendering cycle.
TEST(Device, UpdateDevice)
{
    Babylon::Graphics::DeviceT deviceA = Helpers::CreateDevice();
    ASSERT_NE(deviceA, nullptr);

    Babylon::Graphics::DeviceT deviceB = nullptr;

    {
        // Inherit Window / Width / Height from the App layer's config so bgfx can manage its own
        // swap chain on the HWND. We deliberately do NOT set BackBufferColor here -- the
        // caller-provided-BackBuffer flow is a separate concern covered by TEST(Device, BackBuffer).
        Babylon::Graphics::Configuration config = g_deviceConfig;
        config.Device = deviceA;

        Babylon::Graphics::Device device{config};

        // Drive a frame to force EnableRendering -> bgfx::init with deviceA.
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();

        EXPECT_EQ(device.GetPlatformInfo().Device, deviceA);
        EXPECT_FALSE(device.IsDeviceLost());

        deviceB = Helpers::CreateDevice();
        ASSERT_NE(deviceB, nullptr);

        // Tear bgfx down before pointing the device at the new graphics device.
        device.DisableRendering();

        device.UpdateDevice(deviceB);

        // Drive another frame to force EnableRendering -> bgfx::init with deviceB.
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();

        EXPECT_EQ(device.GetPlatformInfo().Device, deviceB);
        EXPECT_FALSE(device.IsDeviceLost());
        // Note: no EXPECT_NE(deviceB, deviceA). On D3D12 with WARP, D3D12CreateDevice returns the
        // same singleton pointer on successive calls so distinctness is not assertable. On D3D11
        // distinctness holds but exercising it does not add value over the EXPECT_EQ above.
    }   // Babylon::Graphics::Device destructs here, calling DisableRendering on deviceB.

    Helpers::DestroyDevice(deviceB);
    Helpers::DestroyDevice(deviceA);
}
