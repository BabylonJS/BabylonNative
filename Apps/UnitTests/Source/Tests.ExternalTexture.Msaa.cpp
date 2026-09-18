#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>
#include <napi/pointer.h>

#include "Helpers.h"

#include <array>
#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    constexpr uint32_t TEX_SIZE = 128;

    // Renders the rotated red plane into an external GPU texture (created with the given sample
    // count), reads the result back, and returns the RGBA8 byte buffer.
    std::vector<uint8_t> RenderAndReadback(uint32_t samples)
    {
        Babylon::Graphics::Device device{g_deviceConfig};

        device.StartRenderingCurrentFrame();

        auto nativeTexture = Helpers::CreateTexture(
            device.GetPlatformInfo().Device, TEX_SIZE, TEX_SIZE, 1, true, samples);
        Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};

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
        loader.LoadScript("app:///Assets/tests.externalTexture.msaa.js");

        // CreateForJavaScript is synchronous so startup() can run in the same JS task as the
        // texture wrap. renderFrame() submits draw commands which are flushed when the frame ends.
        std::promise<void> renderDone;
        loader.Dispatch([&externalTexture, &renderDone, samples](Napi::Env env) {
            auto jsNativeTexture = externalTexture.CreateForJavaScript(env);

            env.Global().Get("startup").As<Napi::Function>().Call({
                jsNativeTexture,
                Napi::Number::New(env, TEX_SIZE),
                Napi::Number::New(env, TEX_SIZE),
                Napi::Number::New(env, samples),
            });

            auto jsPromise = env.Global().Get("renderFrame").As<Napi::Function>().Call({}).As<Napi::Promise>();

            auto jsOnFulfilled = Napi::Function::New(env, [&renderDone](const Napi::CallbackInfo&) {
                renderDone.set_value();
            });
            auto jsOnRejected = Napi::Function::New(env, [&renderDone](const Napi::CallbackInfo& info) {
                renderDone.set_exception(std::make_exception_ptr(
                    std::runtime_error{Napi::GetErrorString(info[0].As<Napi::Error>())}));
            });

            jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
        });

        renderDone.get_future().get();

        device.FinishRenderingCurrentFrame();

        auto pixels = Helpers::ReadPixels(
            device.GetPlatformInfo(), nativeTexture, TEX_SIZE, TEX_SIZE);

        Helpers::DestroyTexture(nativeTexture);
        return pixels;
    }

    struct PixelStats
    {
        uint32_t pureRed = 0;     // R=255, G=0, B=0 (within tolerance)
        uint32_t pureWhite = 0;   // R=255, G=255, B=255 (within tolerance)
        uint32_t blend = 0;       // R=255 but 0<G<255 and 0<B<255 -- partial-coverage AA pixel
        uint32_t other = 0;       // anything else (unexpected)
    };

    PixelStats ClassifyPixels(const std::vector<uint8_t>& pixels, uint32_t width, uint32_t height)
    {
        PixelStats stats{};
        constexpr int tolerance = 4;
        for (uint32_t i = 0; i < width * height; ++i)
        {
            const int r = pixels[i * 4 + 0];
            const int g = pixels[i * 4 + 1];
            const int b = pixels[i * 4 + 2];

            const bool isPureRed = (r >= 255 - tolerance) && (g <= tolerance) && (b <= tolerance);
            const bool isPureWhite = (r >= 255 - tolerance) && (g >= 255 - tolerance) && (b >= 255 - tolerance);
            const bool isRedWhiteBlend = (r >= 255 - tolerance) && (g > tolerance) && (g < 255 - tolerance) && (b > tolerance) && (b < 255 - tolerance);

            if (isPureRed)
            {
                ++stats.pureRed;
            }
            else if (isPureWhite)
            {
                ++stats.pureWhite;
            }
            else if (isRedWhiteBlend)
            {
                ++stats.blend;
            }
            else
            {
                ++stats.other;
            }
        }
        return stats;
    }
}

TEST(ExternalTexture, PreservesWrappedMsaaFlags)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS) || defined(SKIP_MULTISAMPLE_TESTS)
    GTEST_SKIP() << "Native multisample texture wrapping is unavailable in this test configuration";
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device](Napi::Env env) { device.AddToJavaScript(env); });

    const std::array<uint64_t, 3> rtFlags{
        BGFX_TEXTURE_RT, BGFX_TEXTURE_RT_MSAA_X2, BGFX_TEXTURE_RT_MSAA_X4};
    for (size_t index = 0; index < rtFlags.size(); ++index)
    {
        const uint32_t samples = 1u << index;
        const uint64_t expectedFlags = rtFlags[index] | (samples > 1 ? BGFX_TEXTURE_MSAA_SAMPLE : BGFX_TEXTURE_NONE);
        device.StartRenderingCurrentFrame();
        if (!bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA8, expectedFlags))
        {
            device.FinishRenderingCurrentFrame();
            std::cout << "Skipping unsupported external MSAA sample count: " << samples << std::endl;
            continue;
        }
        auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 16, 16, 1, true, samples);
        Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
        Helpers::DestroyTexture(nativeTexture);
        std::promise<std::string> completed;
        auto future = completed.get_future();
        runtime.Dispatch([&](Napi::Env env) {
            SCOPED_TRACE(samples);
            std::string error;
            try
            {
                auto value = externalTexture.CreateForJavaScript(env);
                auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                EXPECT_EQ(texture->Flags() & BGFX_TEXTURE_RT_MSAA_MASK, rtFlags[index]);
                EXPECT_EQ(texture->Flags() & BGFX_TEXTURE_MSAA_SAMPLE, expectedFlags & BGFX_TEXTURE_MSAA_SAMPLE);
                texture->Dispose();
            }
            catch (const std::exception& ex)
            {
                error = ex.what();
            }
            completed.set_value(std::move(error));
        });
        if (future.wait_for(std::chrono::seconds{30}) != std::future_status::ready)
        {
            ADD_FAILURE() << "Timed out inspecting external MSAA texture flags after 30 seconds";
            // The pending callback can still reference this iteration's resources.
            std::quick_exit(1);
        }
        EXPECT_EQ(future.get(), "");
        device.FinishRenderingCurrentFrame();
    }
#endif
}

// samples=1 discriminator: with no MSAA the rotated edges must be aliased -- pure red or pure
// white only, no blends. If this ever shows blend pixels, the test's notion of "blend" is faulty
// and the samples=4 case is no longer diagnostic.
TEST(ExternalTexture, RenderWithMsaaSamples1)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    auto pixels = RenderAndReadback(1);

    const auto stats = ClassifyPixels(pixels, TEX_SIZE, TEX_SIZE);
    std::cout << "[msaa=1] pureRed=" << stats.pureRed
              << " pureWhite=" << stats.pureWhite
              << " blend=" << stats.blend
              << " other=" << stats.other << std::endl;

    EXPECT_GT(stats.pureRed, 100u) << "Expected the rotated red plane to cover a meaningful interior region.";
    EXPECT_GT(stats.pureWhite, 100u) << "Expected the white clear color to cover a meaningful background region.";
    EXPECT_EQ(stats.blend, 0u) << "Expected samples=1 to produce aliased edges (no blend pixels). Blend pixels here would invalidate the samples=4 discriminator.";
    EXPECT_LT(stats.other, 100u) << "Unexpected pixel colors (neither red/white/blend) -- something other than the red plane was rendered.";
#endif
}

// samples=4: rotated red plane edges must be anti-aliased -- we expect red/white blend pixels
// after the MSAA resolve.
TEST(ExternalTexture, RenderWithMsaaSamples4)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS) || defined(SKIP_MULTISAMPLE_TESTS)
    GTEST_SKIP();
#else
    auto pixels = RenderAndReadback(4);

    const auto stats = ClassifyPixels(pixels, TEX_SIZE, TEX_SIZE);
    std::cout << "[msaa=4] pureRed=" << stats.pureRed
              << " pureWhite=" << stats.pureWhite
              << " blend=" << stats.blend
              << " other=" << stats.other << std::endl;

    EXPECT_GT(stats.pureRed, 100u) << "Expected the rotated red plane to cover a meaningful interior region.";
    EXPECT_GT(stats.pureWhite, 100u) << "Expected the white clear color to cover a meaningful background region.";
    EXPECT_GT(stats.blend, 8u) << "Expected MSAA samples=4 to produce red/white blend pixels along the rotated edges.";
    EXPECT_LT(stats.other, 100u) << "Unexpected pixel colors (neither red/white/blend) -- something other than the red plane was rendered.";
#endif
}
