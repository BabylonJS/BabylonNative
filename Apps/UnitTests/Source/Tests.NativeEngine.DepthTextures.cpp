#include <gtest/gtest.h>
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Polyfills/Console.h>
#include <napi/pointer.h>

#include <chrono>
#include <future>
#include <iostream>
#include <string>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(NativeEngineDepthTextures, ExposesReadableDepthAndPreservesFramebufferOwnership)
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
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
                std::cout << message << std::endl;
            });
            Babylon::Plugins::NativeEngine::Initialize(env);
            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            auto frameScope = context.AcquireFrameCompletionScope();
            auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
            const auto createFrameBuffer = [&](Napi::Value texture, bool stencil, bool depth) {
                return engine.Get("createFrameBuffer").As<Napi::Function>().Call(engine, {
                    texture, Napi::Number::New(env, 16), Napi::Number::New(env, 16),
                    Napi::Boolean::New(env, stencil), Napi::Boolean::New(env, depth)});
            };
            for (const bool stencil : {false, true})
            {
                for (const bool depth : {false, true})
                {
                    auto value = engine.Get("createTexture").As<Napi::Function>().Call(engine, {});
                    auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                    if (!stencil && !depth)
                    {
                        EXPECT_THROW(createFrameBuffer(value, stencil, depth), Napi::Error);
                        continue;
                    }
                    auto frameBufferValue = createFrameBuffer(value, stencil, depth);
                    auto* frameBuffer = frameBufferValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
                    EXPECT_TRUE(texture->IsValid());
                    EXPECT_EQ(texture->Width(), 16u);
                    EXPECT_EQ(texture->Height(), 16u);
                    EXPECT_EQ(texture->Flags() & BGFX_TEXTURE_RT_WRITE_ONLY, 0u);
                    EXPECT_EQ(texture->Handle().idx, bgfx::getTexture(frameBuffer->Handle(), 0).idx);
                    EXPECT_TRUE(frameBuffer->HasDepth());
                    EXPECT_EQ(frameBuffer->HasStencil(), stencil);
                    texture->Dispose();
                    // Disposing the non-owning alias must leave the framebuffer usable.
                    frameBuffer->Clear(*context.GetActiveEncoder(),
                        BGFX_CLEAR_DEPTH | (stencil ? BGFX_CLEAR_STENCIL : 0), 0, 0.25f, 0);
                    frameBuffer->Dispose();
                }
            }
            auto frameBufferValue = createFrameBuffer(env.Null(), false, true);
            auto* frameBuffer = frameBufferValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
            EXPECT_TRUE(frameBuffer->HasDepth());
            EXPECT_FALSE(frameBuffer->HasStencil());
            frameBuffer->Dispose();
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
