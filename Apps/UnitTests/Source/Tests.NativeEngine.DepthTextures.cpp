#include <gtest/gtest.h>
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Polyfills/Console.h>
#include <napi/pointer.h>

#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <vector>

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
            const auto createFrameBuffer = [&](Napi::Value texture, bool stencil, bool depth, uint32_t samples = 1) {
                return engine.Get("createFrameBuffer").As<Napi::Function>().Call(engine, {
                    texture, Napi::Number::New(env, 16), Napi::Number::New(env, 16),
                    Napi::Boolean::New(env, stencil), Napi::Boolean::New(env, depth), Napi::Number::New(env, samples)});
            };
            const std::array<uint64_t, 3> renderTargetFlags{
                BGFX_TEXTURE_RT, BGFX_TEXTURE_RT_MSAA_X2, BGFX_TEXTURE_RT_MSAA_X4};
            for (size_t index = 0; index < renderTargetFlags.size(); ++index)
            {
                const uint32_t samples = 1u << index;
                const uint64_t samplingFlag = index == 0 ? BGFX_TEXTURE_NONE : BGFX_TEXTURE_MSAA_SAMPLE;
                const uint64_t flags = renderTargetFlags[index] | samplingFlag;
                SCOPED_TRACE(samples);
                for (const bool stencil : {false, true})
                {
                    SCOPED_TRACE(stencil);
                    if (index != 0 &&
                        !bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::D24S8, flags) &&
                        (stencil || (!bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::D32F, flags) &&
                                     !bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::D24, flags))))
                    {
                        std::cout << "Skipping unsupported readable depth MSAA: samples=" << samples
                                  << ", stencil=" << stencil << std::endl;
                        continue;
                    }
                    for (const bool depth : {false, true})
                    {
                        auto value = engine.Get("createTexture").As<Napi::Function>().Call(engine, {});
                        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                        if (!stencil && !depth)
                        {
                            EXPECT_THROW(createFrameBuffer(value, stencil, depth, samples), Napi::Error);
                            continue;
                        }
                        auto frameBufferValue = createFrameBuffer(value, stencil, depth, samples);
                        auto* frameBuffer = frameBufferValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
                        EXPECT_TRUE(texture->IsValid());
                        EXPECT_EQ(texture->Width(), 16u);
                        EXPECT_EQ(texture->Height(), 16u);
                        EXPECT_EQ(texture->Flags() & BGFX_TEXTURE_RT_WRITE_ONLY, 0u);
                        EXPECT_EQ(texture->Flags() & BGFX_TEXTURE_MSAA_SAMPLE, samplingFlag);
                        EXPECT_EQ(texture->Flags() & BGFX_TEXTURE_RT_MSAA_MASK, renderTargetFlags[index]);
                        EXPECT_EQ(texture->Handle().idx, bgfx::getTexture(frameBuffer->Handle(), 0).idx);
                        EXPECT_TRUE(frameBuffer->HasDepth());
                        EXPECT_EQ(frameBuffer->HasStencil(), stencil);
                        texture->Dispose();
                        // Disposing the non-owning alias must leave the framebuffer usable.
                        frameBuffer->Clear(*context.GetActiveEncoder(),
                            BGFX_CLEAR_DEPTH | (stencil ? BGFX_CLEAR_STENCIL : 0), 0, 0, 0, 0, 0.25f, 0);
                        frameBuffer->Dispose();
                    }
                }
            }
            {
                auto value = engine.Get("createTexture").As<Napi::Function>().Call(engine, {});
                auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                const auto createSharedDepthFrameBuffer = [&] {
                    return engine.Get("createMultiFrameBuffer").As<Napi::Function>().Call(engine, {
                        Napi::Array::New(env), Napi::Number::New(env, 16), Napi::Number::New(env, 16),
                        Napi::Boolean::New(env, false), Napi::Boolean::New(env, true), Napi::Number::New(env, 1),
                        env.Undefined(), value});
                };
                auto firstValue = createSharedDepthFrameBuffer();
                auto secondValue = createSharedDepthFrameBuffer();
                auto* first = firstValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
                auto* second = secondValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
                EXPECT_EQ(texture->Handle().idx, bgfx::getTexture(first->Handle(), 0).idx);
                EXPECT_EQ(texture->Handle().idx, bgfx::getTexture(second->Handle(), 0).idx);
                first->Dispose();
                // Shared depth remains texture-owned when one of its framebuffers is disposed.
                second->Clear(*context.GetActiveEncoder(), BGFX_CLEAR_DEPTH, 0, 0, 0, 0, 0.25f, 0);
                second->Dispose();
                EXPECT_TRUE(texture->IsValid());
                texture->Dispose();
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

TEST(NativeEngineDepthTextures, SharesGeneratedDepthWithoutRebuildingTheFrameBuffer)
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
            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            auto frameScope = context.AcquireFrameCompletionScope();
            auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
            const auto call = [&](const char* name, const std::vector<Napi::Value>& args) {
                return engine.Get(name).As<Napi::Function>().Call(engine, args);
            };

            auto sourceValue = call("createFrameBuffer", {env.Null(), Napi::Number::New(env, 16), Napi::Number::New(env, 16),
                Napi::Boolean::New(env, false), Napi::Boolean::New(env, true), Napi::Number::New(env, 1)});
            auto* source = sourceValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
            const bgfx::TextureHandle generatedDepth = bgfx::getTexture(source->Handle(), 0);

            auto depthValue = call("createTexture", {});
            auto* depth = depthValue.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
            EXPECT_TRUE(call("shareFrameBufferDepthStencil", {sourceValue, depthValue}).As<Napi::Boolean>().Value());
            // The texture aliases the depth the source already renders to, so its contents carry over.
            EXPECT_EQ(depth->Handle().idx, generatedDepth.idx);
            EXPECT_EQ(bgfx::getTexture(source->Handle(), 0).idx, generatedDepth.idx);
            EXPECT_EQ(depth->Width(), 16u);
            EXPECT_EQ(depth->Height(), 16u);
            EXPECT_NE(depth->Flags() & BGFX_TEXTURE_RT_WRITE_ONLY, 0u);

            // The depth was handed over once; a second request has nothing to share.
            auto otherValue = call("createTexture", {});
            EXPECT_FALSE(call("shareFrameBufferDepthStencil", {sourceValue, otherValue}).As<Napi::Boolean>().Value());
            EXPECT_FALSE(otherValue.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get()->IsValid());

            auto destinationValue = call("createMultiFrameBuffer", {Napi::Array::New(env), Napi::Number::New(env, 16),
                Napi::Number::New(env, 16), Napi::Boolean::New(env, false), Napi::Boolean::New(env, true),
                Napi::Number::New(env, 1), env.Undefined(), depthValue});
            auto* destination = destinationValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
            EXPECT_EQ(bgfx::getTexture(destination->Handle(), 0).idx, generatedDepth.idx);

            // The texture now owns the depth, so it outlives the framebuffer that generated it.
            source->Dispose();
            EXPECT_TRUE(depth->IsValid());
            destination->Clear(*context.GetActiveEncoder(), BGFX_CLEAR_DEPTH, 0, 0, 0, 0, 1.0f, 0);
            destination->Dispose();
            depth->Dispose();
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

TEST(NativeEngineDepthTextures, SharesOneGeneratedDepthAcrossCubeFacesAndArrayLayers)
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
            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            auto frameScope = context.AcquireFrameCompletionScope();
            auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
            const auto call = [&](const char* name, const std::vector<Napi::Value>& args) {
                return engine.Get(name).As<Napi::Function>().Call(engine, args);
            };

            constexpr uint16_t size{16};
            const auto createTarget = [&](bool cube) {
                auto value = call("createTexture", {});
                auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                if (cube)
                {
                    texture->CreateCube(size, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
                }
                else
                {
                    texture->Create2D(size, size, false, 3, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
                }
                return value;
            };
            const auto createLayerFrameBuffers = [&](Napi::Value texture, uint32_t count) {
                std::vector<Napi::Value> frameBuffers;
                for (uint32_t layer = 0; layer < count; ++layer)
                {
                    frameBuffers.push_back(call("createFrameBuffer", {texture, Napi::Number::New(env, size), Napi::Number::New(env, size),
                        Napi::Boolean::New(env, false), Napi::Boolean::New(env, true), Napi::Number::New(env, 1), Napi::Number::New(env, layer)}));
                }
                return frameBuffers;
            };
            const auto frameBufferOf = [](Napi::Value value) {
                return value.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
            };
            // The color texture is attachment 0 and the generated depth attachment 1.
            const auto depthOf = [&](Napi::Value value) {
                return bgfx::getTexture(frameBufferOf(value)->Handle(), 1);
            };

            for (const bool cube : {true, false})
            {
                SCOPED_TRACE(cube ? "cube" : "array");
                const uint32_t count = cube ? 6u : 3u;
                auto colorValue = createTarget(cube);
                auto frameBuffers = createLayerFrameBuffers(colorValue, count);
                const bgfx::TextureHandle generatedDepth = depthOf(frameBuffers[0]);
                ASSERT_TRUE(bgfx::isValid(generatedDepth));
                for (const auto& frameBuffer : frameBuffers)
                {
                    EXPECT_EQ(depthOf(frameBuffer).idx, generatedDepth.idx);
                }

                // Another render target gets a depth of its own.
                auto otherColorValue = createTarget(cube);
                auto otherFrameBuffers = createLayerFrameBuffers(otherColorValue, 1);
                EXPECT_NE(depthOf(otherFrameBuffers[0]).idx, generatedDepth.idx);

                // Sharing from any face or layer hands over the depth of the whole render target, once.
                auto depthValue = call("createTexture", {});
                auto* depth = depthValue.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                EXPECT_TRUE(call("shareFrameBufferDepthStencil", {frameBuffers[1], depthValue}).As<Napi::Boolean>().Value());
                EXPECT_EQ(depth->Handle().idx, generatedDepth.idx);
                auto secondValue = call("createTexture", {});
                EXPECT_FALSE(call("shareFrameBufferDepthStencil", {frameBuffers[0], secondValue}).As<Napi::Boolean>().Value());

                // A handed-over depth is owned by the texture and is not attached by framebuffers created later.
                auto laterFrameBuffers = createLayerFrameBuffers(colorValue, 1);
                EXPECT_NE(depthOf(laterFrameBuffers[0]).idx, generatedDepth.idx);

                // The texture now owns the depth, so it outlives every framebuffer that attached it.
                for (auto& frameBuffer : frameBuffers)
                {
                    frameBufferOf(frameBuffer)->Clear(*context.GetActiveEncoder(), BGFX_CLEAR_DEPTH, 0, 0, 0, 0, 1.0f, 0);
                    frameBufferOf(frameBuffer)->Dispose();
                }
                EXPECT_TRUE(depth->IsValid());
                frameBufferOf(laterFrameBuffers[0])->Dispose();
                frameBufferOf(otherFrameBuffers[0])->Dispose();
                depth->Dispose();
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
