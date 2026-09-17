#include <gtest/gtest.h>
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <napi/pointer.h>

#include <array>
#include <chrono>
#include <future>
#include <string>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(NativeEngineMsaa, PreservesSampleCountsAndAllocatesSampledStorage)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
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
                if (!bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA8, sampledFlags))
                {
                    continue;
                }
                Babylon::Graphics::Texture sampled{context};
                sampled.Create2D(16, 16, false, 1, bgfx::TextureFormat::RGBA8, sampledFlags);
                auto handle = sampled.Handle();
                auto frameBuffer = bgfx::createFrameBuffer(1, &handle);
                EXPECT_TRUE(bgfx::isValid(frameBuffer));
                bgfx::destroy(frameBuffer);
            }
            engine.Get("dispose").As<Napi::Function>().Call(engine, {});
        }
        catch (const std::exception& ex)
        {
            error = ex.what();
        }
        completed.set_value(std::move(error));
    });
    while (future.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready)
    {
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }
    EXPECT_EQ(future.get(), "");
    device.FinishRenderingCurrentFrame();
}
