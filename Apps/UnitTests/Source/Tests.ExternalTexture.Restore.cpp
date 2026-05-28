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
//   1. Create an external GPU texture, wrap it, render red into it, readback → expect red.
//   2. Create a SECOND GPU texture (simulating device restore), call updateWrappedNativeTexture,
//      render blue into it, readback → expect blue.
TEST(ExternalTexture, RestoreAfterDeviceLoss)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};
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
        std::cerr.flush();
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
    loader.LoadScript("app:///Assets/tests.externalTexture.restore.js");

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

    // --- Phase 2: simulate device restore with a NEW texture, call restoreTexture, render blue, readback ---
    auto nativeTexture2 = Helpers::CreateTexture(
        device.GetPlatformInfo().Device, TEX_SIZE, TEX_SIZE, 1, true);
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
#endif
}
