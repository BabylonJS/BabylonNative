#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>

#include "Utils.h"
#ifdef HAS_RENDERDOC
#include "RenderDoc.h"
#endif

#include <cmath>
#include <future>
#include <iostream>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ExternalTexture, RenderTextureArray)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    constexpr uint32_t TEX_SIZE = 64;
    constexpr uint32_t SLICE_COUNT = 3;

    const Color sliceColors[SLICE_COUNT] = {
        {255, 0, 0, 255},
        {0, 255, 0, 255},
        {0, 0, 255, 255},
    };

#ifdef HAS_RENDERDOC
    RenderDoc::Init();
#endif

    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto inputTexture = CreateTestTextureArrayWithData(
        device.GetPlatformInfo().Device, TEX_SIZE, TEX_SIZE, sliceColors, SLICE_COUNT);
    Babylon::Plugins::ExternalTexture inputExternalTexture{inputTexture};

    auto outputTexture = CreateRenderTargetTexture(
        device.GetPlatformInfo().Device, TEX_SIZE, TEX_SIZE);
    Babylon::Plugins::ExternalTexture outputExternalTexture{outputTexture};

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
    loader.LoadScript("app:///Assets/tests.externalTexture.render.js");

    loader.Dispatch([&inputExternalTexture, &outputExternalTexture, &startupDone](Napi::Env env) {
        auto jsInput = inputExternalTexture.CreateForJavaScript(env);
        auto jsOutput = outputExternalTexture.CreateForJavaScript(env);
        env.Global().Get("startup").As<Napi::Function>().Call({
            jsInput,
            jsOutput,
            Napi::Number::New(env, TEX_SIZE),
            Napi::Number::New(env, TEX_SIZE),
        });
        startupDone.set_value();
    });

    update.Finish();
    device.FinishRenderingCurrentFrame();

    startupDone.get_future().wait();

    // Pump an extra frame so that bgfx::frame() processes the placeholder
    // texture creation and AfterRenderScheduler fires overrideInternal
    // to apply the native texture backing.
    device.StartRenderingCurrentFrame();
    update.Start();
    update.Finish();
    device.FinishRenderingCurrentFrame();

    for (uint32_t sliceIndex = 0; sliceIndex < SLICE_COUNT; ++sliceIndex)
    {
#ifdef HAS_RENDERDOC
        RenderDoc::StartFrameCapture(device.GetPlatformInfo().Device);
#endif

        device.StartRenderingCurrentFrame();
        update.Start();

        std::promise<void> renderDone;

        loader.Dispatch([sliceIndex, &renderDone](Napi::Env env) {
            auto jsPromise = env.Global().Get("renderSlice").As<Napi::Function>().Call({
                Napi::Number::New(env, sliceIndex),
            }).As<Napi::Promise>();

            auto jsOnFulfilled = Napi::Function::New(env, [&renderDone](const Napi::CallbackInfo&) {
                renderDone.set_value();
            });

            jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled});
        });

        renderDone.get_future().wait();

        update.Finish();
        device.FinishRenderingCurrentFrame();

#ifdef HAS_RENDERDOC
        RenderDoc::StopFrameCapture(device.GetPlatformInfo().Device);
#endif

        auto pixels = ReadBackRenderTarget(
            device.GetPlatformInfo().Device, outputTexture, TEX_SIZE, TEX_SIZE);

        const auto& expected = sliceColors[sliceIndex];
        uint32_t matchCount = 0;
        const uint32_t totalPixels = TEX_SIZE * TEX_SIZE;

        for (uint32_t i = 0; i < 5 && i < totalPixels; ++i)
        {
            std::cout << "  pixel[" << i << "] = ("
                      << static_cast<int>(pixels[i * 4 + 0]) << ", "
                      << static_cast<int>(pixels[i * 4 + 1]) << ", "
                      << static_cast<int>(pixels[i * 4 + 2]) << ", "
                      << static_cast<int>(pixels[i * 4 + 3]) << ")" << std::endl;
        }

        for (uint32_t i = 0; i < totalPixels; ++i)
        {
            const uint8_t r = pixels[i * 4 + 0];
            const uint8_t g = pixels[i * 4 + 1];
            const uint8_t b = pixels[i * 4 + 2];

            if (std::abs(static_cast<int>(r) - expected.R) < 25 &&
                std::abs(static_cast<int>(g) - expected.G) < 25 &&
                std::abs(static_cast<int>(b) - expected.B) < 25)
            {
                ++matchCount;
            }
        }

        const double matchPercent =
            static_cast<double>(matchCount) / totalPixels * 100.0;

        std::cout << "Slice " << sliceIndex << ": " << matchCount << "/"
                  << totalPixels << " pixels match (" << matchPercent << "%)"
                  << std::endl;

        EXPECT_GE(matchPercent, 90.0)
            << "Slice " << sliceIndex << ": expected ("
            << static_cast<int>(expected.R) << ", "
            << static_cast<int>(expected.G) << ", "
            << static_cast<int>(expected.B) << ") but only "
            << matchPercent << "% of pixels matched";
    }

    DestroyRenderTargetTexture(outputTexture);
    DestroyTestTexture(inputTexture);
#endif
}
