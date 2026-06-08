#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>

#include "Helpers.h"

#include <d3d11.h>

#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <stdexcept>

extern Babylon::Graphics::Configuration g_deviceConfig;

// Renders a single slice of an NV12 (DXGI_FORMAT_NV12) Texture2DArray selected at bind time via the
// ExternalTexture layerIndex (bgfx setTexture view API). The Y (R8) and UV (R8G8) planes are wrapped
// as separate single-slice ExternalTextures and passed through unchanged to the output (R=Y, G=U,
// B=V). This is a regression test for the "green remote participants" bug, where the per-plane UV SRV
// of an NV12 array read the wrong array slice (or the Y plane) for slice >= 1 -- making the output
// differ from the target slice's chroma. Selecting the slice as a single-slice view fixes it.
TEST(ExternalTexture, RenderNV12Slice)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    constexpr uint32_t TEX_SIZE = 256;
    constexpr uint16_t TARGET_SLICE = 1;

    // Distinct raw YUV bytes per slice so the passthrough output reveals exactly which slice and
    // plane each sampler read. The target slice (1) must differ from slice 0 in every channel.
    const Helpers::YuvColor sliceYuv[2] = {
        {10, 20, 30},
        {100, 80, 160},
    };

    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nv12Texture = Helpers::CreateNV12TextureArrayWithData(
        device.GetPlatformInfo().Device, TEX_SIZE, TEX_SIZE, sliceYuv, 2);

    // Two per-plane views of the same NV12 array, each selecting only TARGET_SLICE.
    Babylon::Plugins::ExternalTexture yExternalTexture{nv12Texture, DXGI_FORMAT_R8_UNORM};
    Babylon::Plugins::ExternalTexture uvExternalTexture{nv12Texture, DXGI_FORMAT_R8G8_UNORM};

    auto outputTexture = Helpers::CreateTexture(
        device.GetPlatformInfo().Device, TEX_SIZE, TEX_SIZE, 1, true);
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
    loader.LoadScript("app:///Assets/tests.externalTexture.renderNV12.js");

    loader.Dispatch([&yExternalTexture, &uvExternalTexture, &outputExternalTexture, &startupDone](Napi::Env env) {
        auto jsY = yExternalTexture.CreateForJavaScript(env, TARGET_SLICE);
        auto jsUV = uvExternalTexture.CreateForJavaScript(env, TARGET_SLICE);
        auto jsOutput = outputExternalTexture.CreateForJavaScript(env);
        env.Global().Get("startup").As<Napi::Function>().Call({
            jsY,
            jsUV,
            jsOutput,
            Napi::Number::New(env, TEX_SIZE),
            Napi::Number::New(env, TEX_SIZE),
        });
        startupDone.set_value();
    });

    startupDone.get_future().wait();

    update.Finish();
    device.FinishRenderingCurrentFrame();

    device.StartRenderingCurrentFrame();
    update.Start();

    std::promise<void> renderDone;
    loader.Dispatch([&renderDone](Napi::Env env) {
        auto jsPromise = env.Global().Get("render").As<Napi::Function>().Call({}).As<Napi::Promise>();

        auto jsOnFulfilled = Napi::Function::New(env, [&renderDone](const Napi::CallbackInfo&) {
            renderDone.set_value();
        });
        auto jsOnRejected = Napi::Function::New(env, [&renderDone](const Napi::CallbackInfo& info) {
            renderDone.set_exception(std::make_exception_ptr(
                std::runtime_error{Napi::GetErrorString(info[0].As<Napi::Error>())}));
        });
        jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
    });

    auto renderFuture = renderDone.get_future();
    ASSERT_EQ(renderFuture.wait_for(std::chrono::seconds(30)), std::future_status::ready) << "render timed out";
    ASSERT_NO_THROW(renderFuture.get()) << "render rejected";

    update.Finish();
    device.FinishRenderingCurrentFrame();

    auto pixels = Helpers::ReadPixels(device.GetPlatformInfo(), outputTexture, TEX_SIZE, TEX_SIZE);

    const auto& expected = sliceYuv[TARGET_SLICE];
    constexpr int TOLERANCE = 2;
    uint32_t matchCount = 0;
    const uint32_t totalPixels = TEX_SIZE * TEX_SIZE;

    std::cout << "  pixel[0] = (" << static_cast<int>(pixels[0]) << ", "
              << static_cast<int>(pixels[1]) << ", " << static_cast<int>(pixels[2])
              << ") expected (" << static_cast<int>(expected.Y) << ", "
              << static_cast<int>(expected.U) << ", " << static_cast<int>(expected.V) << ")" << std::endl;

    for (uint32_t i = 0; i < totalPixels; ++i)
    {
        const int r = pixels[i * 4 + 0];
        const int g = pixels[i * 4 + 1];
        const int b = pixels[i * 4 + 2];
        if (std::abs(r - expected.Y) <= TOLERANCE &&
            std::abs(g - expected.U) <= TOLERANCE &&
            std::abs(b - expected.V) <= TOLERANCE)
        {
            ++matchCount;
        }
    }

    const double matchPercent = static_cast<double>(matchCount) / totalPixels * 100.0;
    std::cout << "RenderNV12Slice: " << matchCount << "/" << totalPixels
              << " pixels match (" << matchPercent << "%)" << std::endl;

    EXPECT_EQ(matchPercent, 100.0)
        << "Expected slice " << TARGET_SLICE << " YUV (" << static_cast<int>(expected.Y) << ", "
        << static_cast<int>(expected.U) << ", " << static_cast<int>(expected.V) << ") but only "
        << matchPercent << "% of pixels matched within +/-" << TOLERANCE
        << " -- the per-plane single-slice view read the wrong slice or plane.";

    Helpers::DestroyTexture(outputTexture);
    Helpers::DestroyTexture(nv12Texture);
#endif
}
