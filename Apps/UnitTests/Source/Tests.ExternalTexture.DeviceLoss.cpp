#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>

#include "Helpers.h"

#include <cstdlib>
#include <future>
#include <iostream>
#include <stdexcept>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    constexpr uint32_t TEX_SIZE = 128;

    struct PixelStats
    {
        uint32_t pureRed = 0;
        uint32_t pureBlue = 0;
        uint32_t pureWhite = 0;
        uint32_t other = 0;
    };

    PixelStats ClassifyPixels(const std::vector<uint8_t>& pixels, uint32_t width, uint32_t height)
    {
        PixelStats stats{};
        constexpr int tolerance = 20;
        for (uint32_t i = 0; i < width * height; ++i)
        {
            const int r = pixels[i * 4 + 0];
            const int g = pixels[i * 4 + 1];
            const int b = pixels[i * 4 + 2];

            if (r >= 255 - tolerance && g <= tolerance && b <= tolerance)
            {
                ++stats.pureRed;
            }
            else if (r <= tolerance && g <= tolerance && b >= 255 - tolerance)
            {
                ++stats.pureBlue;
            }
            else if (r >= 255 - tolerance && g >= 255 - tolerance && b >= 255 - tolerance)
            {
                ++stats.pureWhite;
            }
            else
            {
                ++stats.other;
            }
        }
        return stats;
    }
}

// End-to-end test for the wrapped-native-texture device-loss restore flow:
//   1. Create explicit deviceA, wrap a texture on deviceA, render red, readback → expect red.
//   2. DisableRendering -> UpdateDevice(deviceB) tears bgfx down and re-points the Graphics::Device
//      at a fresh graphics device. The next StartRenderingCurrentFrame triggers EnableRendering
//      which re-inits bgfx and fires the JS render-reset callback (BJS _restoreEngineAfterContextLost).
//      BJS's rebuild walks skip InternalTextureSource.External, leaving the wrapped RT untouched.
//   3. Create a NEW external texture on deviceB, call updateWrappedNativeTexture to repoint the
//      wrapper at the new handle, render blue, readback → expect blue.
TEST(ExternalTexture, RestoreAfterDeviceLoss)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    Babylon::Graphics::DeviceT deviceA = Helpers::CreateDevice();
    ASSERT_NE(deviceA, nullptr);
    Babylon::Graphics::DeviceT deviceB = nullptr;

    Babylon::Graphics::Configuration config = g_deviceConfig;
    config.Device = deviceA;
    Babylon::Graphics::Device device{config};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture1 = Helpers::CreateTexture(
        device.GetPlatformInfo().Device, TEX_SIZE, TEX_SIZE, 1, true);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture1};

    std::promise<void> startupDone;

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
    };

    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&device](Napi::Env env) {
        env.Global().Set("globalThis", env.Global());
        device.AddToJavaScript(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
    });

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Assets/babylon.max.js");
    loader.LoadScript("app:///Assets/tests.externalTexture.deviceLoss.js");

    // Queue AddToContextAsync and call startup() with the wrapped texture.
    std::promise<void> addToContextCalled;
    loader.Dispatch([&externalTexture, &addToContextCalled, &startupDone](Napi::Env env) {
        auto jsPromise = externalTexture.AddToContextAsync(env);
        addToContextCalled.set_value();

        auto jsOnFulfilled = Napi::Function::New(env, [&startupDone](const Napi::CallbackInfo& info) {
            auto jsNativeTexture = info[0];
            info.Env().Global().Get("startup").As<Napi::Function>().Call({
                jsNativeTexture,
                Napi::Number::New(info.Env(), TEX_SIZE),
                Napi::Number::New(info.Env(), TEX_SIZE),
            });
            startupDone.set_value();
        });

        auto jsOnRejected = Napi::Function::New(env, [&startupDone](const Napi::CallbackInfo& info) {
            startupDone.set_exception(std::make_exception_ptr(
                std::runtime_error{Napi::GetErrorString(info[0].As<Napi::Error>())}));
        });

        jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
    });

    addToContextCalled.get_future().wait();
    update.Finish();
    device.FinishRenderingCurrentFrame();
    startupDone.get_future().get();

    // --- Phase 1: render red into texture 1, readback ---
    device.StartRenderingCurrentFrame();
    update.Start();

    std::promise<void> render1Done;
    loader.Dispatch([&render1Done](Napi::Env env) {
        auto jsPromise = env.Global().Get("renderFrame").As<Napi::Function>().Call({}).As<Napi::Promise>();
        auto jsOnFulfilled = Napi::Function::New(env, [&render1Done](const Napi::CallbackInfo&) {
            render1Done.set_value();
        });
        auto jsOnRejected = Napi::Function::New(env, [&render1Done](const Napi::CallbackInfo& info) {
            render1Done.set_exception(std::make_exception_ptr(
                std::runtime_error{Napi::GetErrorString(info[0].As<Napi::Error>())}));
        });
        jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
    });

    render1Done.get_future().get();
    update.Finish();
    device.FinishRenderingCurrentFrame();

    auto pixels1 = Helpers::ReadPixels(device.GetPlatformInfo(), nativeTexture1, TEX_SIZE, TEX_SIZE);
    auto stats1 = ClassifyPixels(pixels1, TEX_SIZE, TEX_SIZE);

    std::cout << "[before restore] pureRed=" << stats1.pureRed
              << " pureBlue=" << stats1.pureBlue
              << " pureWhite=" << stats1.pureWhite
              << " other=" << stats1.other << std::endl;

    EXPECT_GT(stats1.pureRed, 100u) << "Before restore: expected red plane pixels.";
    EXPECT_EQ(stats1.pureBlue, 0u) << "Before restore: no blue expected.";

    // --- Simulate device loss: tear bgfx down on deviceA and re-point Graphics::Device at deviceB ---
    device.DisableRendering();

    deviceB = Helpers::CreateDevice();
    ASSERT_NE(deviceB, nullptr);
    device.UpdateDevice(deviceB);

    // The next StartRenderingCurrentFrame (inside phase 2 below) triggers EnableRendering -> bgfx::init
    // on deviceB -> render-reset callback -> BJS _restoreEngineAfterContextLost (Native override) ->
    // _rebuildGraphicsResources -> onContextRestoredObservable. External-source RT wrappers and
    // InternalTextures are skipped by the rebuild walks, leaving our wrapped RT intact and waiting
    // for updateWrappedNativeTexture below.

    // --- Phase 2: simulate device restore with a NEW texture on deviceB, call restoreTexture, render blue, readback ---
    // bgfx is torn down between DisableRendering and EnableRendering, so device.GetPlatformInfo().Device
    // (which goes through bgfx::getInternalData) returns stale data in this window. Create the new texture
    // directly on deviceB instead.
    auto nativeTexture2 = Helpers::CreateTexture(deviceB, TEX_SIZE, TEX_SIZE, 1, true);
    Babylon::Plugins::ExternalTexture externalTexture2{nativeTexture2};

    device.StartRenderingCurrentFrame();
    update.Start();

    std::promise<void> restoreDone;
    std::promise<void> addToContext2Called;
    loader.Dispatch([&externalTexture2, &addToContext2Called, &restoreDone](Napi::Env env) {
        auto jsPromise = externalTexture2.AddToContextAsync(env);
        addToContext2Called.set_value();

        auto jsOnFulfilled = Napi::Function::New(env, [&restoreDone](const Napi::CallbackInfo& info) {
            auto jsNewNativeTexture = info[0];
            info.Env().Global().Get("restoreTexture").As<Napi::Function>().Call({jsNewNativeTexture});
            restoreDone.set_value();
        });

        auto jsOnRejected = Napi::Function::New(env, [&restoreDone](const Napi::CallbackInfo& info) {
            restoreDone.set_exception(std::make_exception_ptr(
                std::runtime_error{Napi::GetErrorString(info[0].As<Napi::Error>())}));
        });

        jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
    });

    addToContext2Called.get_future().wait();
    update.Finish();
    device.FinishRenderingCurrentFrame();
    restoreDone.get_future().get();

    // Render blue into restored RTT.
    device.StartRenderingCurrentFrame();
    update.Start();

    std::promise<void> render2Done;
    loader.Dispatch([&render2Done](Napi::Env env) {
        auto jsPromise = env.Global().Get("renderFrame").As<Napi::Function>().Call({}).As<Napi::Promise>();
        auto jsOnFulfilled = Napi::Function::New(env, [&render2Done](const Napi::CallbackInfo&) {
            render2Done.set_value();
        });
        auto jsOnRejected = Napi::Function::New(env, [&render2Done](const Napi::CallbackInfo& info) {
            render2Done.set_exception(std::make_exception_ptr(
                std::runtime_error{Napi::GetErrorString(info[0].As<Napi::Error>())}));
        });
        jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
    });

    render2Done.get_future().get();
    update.Finish();
    device.FinishRenderingCurrentFrame();

    auto pixels2 = Helpers::ReadPixels(device.GetPlatformInfo(), nativeTexture2, TEX_SIZE, TEX_SIZE);
    auto stats2 = ClassifyPixels(pixels2, TEX_SIZE, TEX_SIZE);

    std::cout << "[after restore] pureRed=" << stats2.pureRed
              << " pureBlue=" << stats2.pureBlue
              << " pureWhite=" << stats2.pureWhite
              << " other=" << stats2.other << std::endl;

    EXPECT_EQ(stats2.pureRed, 0u) << "After restore: red should be gone (color switched to blue).";
    EXPECT_GT(stats2.pureBlue, 100u) << "After restore: expected blue plane pixels.";

    Helpers::DestroyTexture(nativeTexture1);
    Helpers::DestroyTexture(nativeTexture2);

    // Graphics::Device destructs at scope exit; release the underlying graphics devices.
    Helpers::DestroyDevice(deviceB);
    Helpers::DestroyDevice(deviceA);
#endif
}
