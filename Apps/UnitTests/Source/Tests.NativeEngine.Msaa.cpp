#include <gtest/gtest.h>
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <napi/pointer.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

#if !defined(USE_NOOP_METAL_DEVICE) && !defined(SKIP_RENDER_TESTS)
namespace
{
    std::vector<uint8_t> ReadPixels(Babylon::Graphics::DeviceContext& context, bgfx::TextureHandle source,
        bgfx::TextureFormat::Enum format, uint16_t width, uint16_t height, uint16_t layer, uint8_t mip)
    {
        std::vector<uint8_t> pixels(static_cast<size_t>(width) * height * 4, 0xcd);
        Babylon::Graphics::Texture resolved{context};
        Babylon::Graphics::Texture staging{context};
        std::promise<void> completed;
        auto future = completed.get_future();
        {
            auto scope = context.AcquireFrameCompletionScope();
            resolved.Create2D(width, height, false, 1, format, BGFX_TEXTURE_NONE);
            staging.Create2D(width, height, false, 1, format, BGFX_TEXTURE_READ_BACK);
            bgfx::TextureRegion sourceRegion{};
            sourceRegion.init(source);
            sourceRegion.z = layer;
            sourceRegion.mip = mip;
            bgfx::TextureRegion resolvedRegion{};
            resolvedRegion.init(resolved.Handle());
            bgfx::TextureRegion stagingRegion{};
            stagingRegion.init(staging.Handle());
            for (const auto& regions : {std::pair{resolvedRegion, sourceRegion}, std::pair{stagingRegion, resolvedRegion}})
            {
                context.FlushViewsIfNeeded();
                const auto viewId = context.AcquireNewViewId();
                bgfx::resetView(viewId);
                context.GetActiveEncoder()->blit(viewId, regions.first, regions.second);
            }
            context.ReadTextureAsync(staging.Handle(), pixels)
                .then(arcana::inline_scheduler, arcana::cancellation::none(),
                    [&](const arcana::expected<void, std::exception_ptr>& result) {
                        if (result.has_error())
                        {
                            completed.set_exception(result.error());
                        }
                        else
                        {
                            completed.set_value();
                        }
                    });
        }
        future.get();
        return pixels;
    }
}
#endif

TEST(NativeEngineMsaa, InitializesRenderTargetMipsAndLayersToZero)
{
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP() << "Rendering and readback tests are disabled in this configuration";
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
    if (bgfx::getRendererType() == bgfx::RendererType::Noop)
    {
        device.FinishRenderingCurrentFrame();
        GTEST_SKIP() << "The no-op test device does not execute GPU clears or readbacks";
    }
    Babylon::AppRuntime runtime{};
    std::promise<std::string> completed;
    auto future = completed.get_future();
    runtime.Dispatch([&](Napi::Env env) {
        std::string error;
        try
        {
            device.AddToJavaScript(env);
            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            struct TestCase
            {
                bgfx::TextureFormat::Enum Format;
                uint64_t Flags;
                bool HasMips;
                uint16_t Layers;
            };
            const auto* caps = bgfx::getCaps();
            const auto largeLayerCount = static_cast<uint16_t>(std::min<uint32_t>(
                caps->limits.maxTextureLayers, caps->limits.maxFrameBuffers + 1u));
            const std::array<TestCase, 6> cases{{
                {bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT, true, 2},
                {bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT_MSAA_X2, true, 2},
                {bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT_MSAA_X4 | BGFX_TEXTURE_MSAA_SAMPLE, false, 1},
                {bgfx::TextureFormat::D32F, BGFX_TEXTURE_RT, true, 2},
                {bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT, false, 2},
                {bgfx::TextureFormat::D32F, BGFX_TEXTURE_RT, false, largeLayerCount},
            }};
            size_t readbacks = 0;
            for (const auto& test : cases)
            {
                SCOPED_TRACE(test.Format);
                SCOPED_TRACE(test.Flags);
                if (!bgfx::isTextureValid(0, false, test.Layers, test.Format, test.Flags | BGFX_TEXTURE_BLIT_DST) ||
                    !bgfx::isTextureValid(0, false, 1, test.Format, BGFX_TEXTURE_READ_BACK | BGFX_TEXTURE_BLIT_DST))
                {
                    std::cout << "Skipping unsupported initialization case: format=" << test.Format
                              << ", flags=" << test.Flags << std::endl;
                    continue;
                }
                Babylon::Graphics::Texture texture{context};
                texture.Create2D(16, 16, test.HasMips, test.Layers, test.Format, test.Flags);
                for (uint16_t layer = 0; layer < test.Layers; ++layer)
                {
                    if (test.Layers > 2 && layer != 0 && layer != test.Layers / 2 && layer + 1 != test.Layers)
                    {
                        continue;
                    }
                    SCOPED_TRACE(layer);
                    for (uint8_t mip = 0; mip < (test.HasMips ? 5 : 1); ++mip)
                    {
                        SCOPED_TRACE(mip);
                        const uint16_t size = static_cast<uint16_t>(16 >> mip);
                        const auto pixels = ReadPixels(context, texture.Handle(), test.Format, size, size, layer, mip);
                        EXPECT_TRUE(std::all_of(pixels.begin(), pixels.end(), [](uint8_t value) { return value == 0; }));
                        ++readbacks;
                    }
                }
            }
            EXPECT_GT(readbacks, 0u);
        }
        catch (const std::exception& ex)
        {
            error = ex.what();
        }
        completed.set_value(std::move(error));
    });
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{30};
    while (future.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready)
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            ADD_FAILURE() << "Timed out reading initialized render targets after 30 seconds";
            std::quick_exit(1);
        }
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }
    EXPECT_EQ(future.get(), "");
    device.FinishRenderingCurrentFrame();
#endif
}

TEST(NativeEngineMsaa, PreservesSampleCountsAndAllocatesSampledStorage)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
    if (!bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | BGFX_TEXTURE_BLIT_DST) ||
        !bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT_WRITE_ONLY))
    {
        device.FinishRenderingCurrentFrame();
        GTEST_SKIP() << "The backend does not support the required color/depth framebuffer formats";
    }
    Babylon::AppRuntime runtime{};
    std::promise<std::string> completed;
    auto future = completed.get_future();
    runtime.Dispatch([&](Napi::Env env) {
        std::string error;
        try
        {
            device.AddToJavaScript(env);
            Babylon::Plugins::NativeEngine::Initialize(env);
            auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
            const std::array<uint64_t, 4> flags{
                BGFX_TEXTURE_RT, BGFX_TEXTURE_RT_MSAA_X2,
                BGFX_TEXTURE_RT_MSAA_X4, BGFX_TEXTURE_RT_MSAA_X8};
            for (size_t index = 0; index < flags.size(); ++index)
            {
                const uint32_t samples = 1u << index;
                SCOPED_TRACE(samples);
                const uint64_t depthFlags = BGFX_TEXTURE_RT_WRITE_ONLY | flags[index];
                if (!bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA8, flags[index] | BGFX_TEXTURE_BLIT_DST) ||
                    !bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::D24S8, depthFlags))
                {
                    std::cout << "Skipping unsupported color/depth MSAA sample count: " << samples << std::endl;
                    continue;
                }
                auto value = engine.Get("createTexture").As<Napi::Function>().Call(engine, {});
                auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                engine.Get("initializeTexture").As<Napi::Function>().Call(engine, {
                    value, Napi::Number::New(env, 16), Napi::Number::New(env, 16),
                    Napi::Boolean::New(env, false), Napi::Number::New(env, bgfx::TextureFormat::RGBA8),
                    Napi::Boolean::New(env, true), Napi::Boolean::New(env, false), Napi::Number::New(env, samples)});
                EXPECT_EQ(texture->Flags() & BGFX_TEXTURE_RT_MSAA_MASK, flags[index]);
                // Do not create a mismatched framebuffer after an assertion: bgfx can abort on it.
                if ((texture->Flags() & BGFX_TEXTURE_RT_MSAA_MASK) == flags[index])
                {
                    auto frameBufferValue = engine.Get("createFrameBuffer").As<Napi::Function>().Call(engine, {
                        value, Napi::Number::New(env, 16), Napi::Number::New(env, 16),
                        Napi::Boolean::New(env, true), Napi::Boolean::New(env, true), Napi::Number::New(env, samples)});
                    auto* frameBuffer = frameBufferValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
                    EXPECT_TRUE(bgfx::isValid(frameBuffer->Handle()));
                    frameBuffer->Dispose();
                }
                texture->Dispose();
            }

            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            for (const auto rtFlag : {BGFX_TEXTURE_RT_MSAA_X2, BGFX_TEXTURE_RT_MSAA_X4})
            {
                const uint64_t sampledFlags = rtFlag | BGFX_TEXTURE_MSAA_SAMPLE;
                if (!bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA8, sampledFlags | BGFX_TEXTURE_BLIT_DST))
                {
                    std::cout << "Skipping unsupported sampled MSAA flags: " << sampledFlags << std::endl;
                    continue;
                }
                Babylon::Graphics::Texture sampled{context};
                sampled.Create2D(16, 16, false, 1, bgfx::TextureFormat::RGBA8, sampledFlags);
                auto handle = sampled.Handle();
                auto frameBuffer = bgfx::createFrameBuffer(1, &handle);
                EXPECT_TRUE(bgfx::isValid(frameBuffer));
                if (bgfx::isValid(frameBuffer))
                {
                    bgfx::destroy(frameBuffer);
                }
            }
            if (bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT_MSAA_X2 | BGFX_TEXTURE_BLIT_DST))
            {
                auto scope = context.AcquireFrameCompletionScope();
                Babylon::Graphics::Texture target{context};
                target.Create2D(16, 16, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
                auto handle = target.Handle();
                Babylon::Graphics::FrameBuffer frameBuffer{context, bgfx::createFrameBuffer(1, &handle), 16, 16, false, false, false, -1, false};
                frameBuffer.Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
                const auto beforeInitialization = context.PeekNextViewId();
                Babylon::Graphics::Texture inserted{context};
                inserted.Create2D(16, 16, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT_MSAA_X2);
                const auto afterInitialization = context.PeekNextViewId();
                EXPECT_GT(afterInitialization, beforeInitialization);
                frameBuffer.Submit(*context.GetActiveEncoder(), BGFX_INVALID_HANDLE, BGFX_DISCARD_ALL);
                EXPECT_EQ(context.PeekNextViewId(), afterInitialization + 1u)
                    << "Draws must not reuse a view ordered before the texture initialization clear";
            }
            engine.Get("dispose").As<Napi::Function>().Call(engine, {});
        }
        catch (const std::exception& ex)
        {
            error = ex.what();
        }
        completed.set_value(std::move(error));
    });
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{30};
    while (future.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready)
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            ADD_FAILURE() << "Timed out waiting for the MSAA allocation task after 30 seconds";
            // Do not unwind resources still referenced by a potentially stuck runtime task.
            std::quick_exit(1);
        }
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }
    EXPECT_EQ(future.get(), "");
    device.FinishRenderingCurrentFrame();
}
