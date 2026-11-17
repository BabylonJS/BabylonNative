#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <bgfx/bgfx.h>
#include <napi/env.h>
#include <napi/pointer.h>

#include <array>
#include <chrono>
#include <future>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(NativeEngineCommands, TextureDeleteIsOrderedAfterEarlierTextureCopy)
{
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP() << "GPU rendering/readback is unavailable in this test configuration";
#endif
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();

    constexpr uint16_t size = 2;
    constexpr std::array<uint8_t, size * size * 4> expected{
        17, 33, 65, 255, 17, 33, 65, 255,
        17, 33, 65, 255, 17, 33, 65, 255};
    std::array<uint8_t, expected.size()> pixels{};
    std::promise<void> completed;
    auto future = completed.get_future();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env env) {
        try
        {
            device.AddToJavaScript(env);
            Babylon::Plugins::NativeEngine::Initialize(env);
            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            auto scope = context.AcquireFrameCompletionScope();
            const auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
            const auto source = engine.Get("createTexture").As<Napi::Function>().Call(engine, {});
            const auto destination = engine.Get("createTexture").As<Napi::Function>().Call(engine, {});
            env.Global().Set("_textureDeleteOrderEngine", engine);
            env.Global().Set("_textureDeleteOrderSource", source);
            env.Global().Set("_textureDeleteOrderDestination", destination);
            auto* sourceTexture = source.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
            sourceTexture->Create2D(size, size, false, 1, bgfx::TextureFormat::RGBA8, 0);
            sourceTexture->Update2D(0, 0, 0, 0, size, size, bgfx::copy(expected.data(), uint32_t(expected.size())));
            auto* destinationTexture = destination.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
            destinationTexture->Create2D(size, size, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_READ_BACK);

            Napi::Eval(env, R"(
                (function(engine, source, destination) {
                    const stream = new _native.NativeDataStream(function() {});
                    engine.setCommandDataStream({_nativeDataStream: stream});
                    const copy = _native.Engine.COMMAND_COPYTEXTURE;
                    const remove = _native.Engine.COMMAND_DELETETEXTURE;
                    const words = new Uint32Array(
                        copy.length + source.length + destination.length + remove.length + source.length);
                    let offset = 0;
                    words.set(copy, offset); offset += copy.length;
                    words.set(source, offset); offset += source.length;
                    words.set(destination, offset); offset += destination.length;
                    words.set(remove, offset); offset += remove.length;
                    words.set(source, offset);
                    stream.writeBuffer(words.buffer, words.length);
                    engine.submitCommands();
                })
            )", "native-texture-delete-ordering.js").As<Napi::Function>().Call({engine, source, destination});

            EXPECT_FALSE(bgfx::isValid(sourceTexture->Handle()));
            context.ReadTextureAsync(destinationTexture->Handle(), gsl::make_span(pixels))
                .then(arcana::inline_scheduler, arcana::cancellation::none(),
                    [&completed](arcana::expected<void, std::exception_ptr> result) {
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
        catch (...)
        {
            completed.set_exception(std::current_exception());
        }
    });

    while (future.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready)
    {
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }
    ASSERT_NO_THROW(future.get());
    EXPECT_EQ(pixels, expected);
    runtime.Dispatch([](Napi::Env env) {
        const auto engine = env.Global().Get("_textureDeleteOrderEngine").As<Napi::Object>();
        engine.Get("deleteTexture").As<Napi::Function>().Call(
            engine, {env.Global().Get("_textureDeleteOrderDestination")});
        engine.Get("dispose").As<Napi::Function>().Call(engine, {});
        env.Global().Set("_textureDeleteOrderEngine", env.Undefined());
        env.Global().Set("_textureDeleteOrderSource", env.Undefined());
        env.Global().Set("_textureDeleteOrderDestination", env.Undefined());
    });
    device.FinishRenderingCurrentFrame();
}
