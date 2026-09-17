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
#include <memory>
#include <stdexcept>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(NativeEngineCubeRenderTargets, ClearsEachFaceIndependentlyAndPreserves2DDefaults)
{
    Babylon::Graphics::Device device{g_deviceConfig};
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP() << "GPU rendering/readback is unavailable in this test configuration";
#endif
    device.StartRenderingCurrentFrame();
    Babylon::AppRuntime runtime{};
    constexpr uint16_t size = 16;
    constexpr std::array<uint32_t, 6> colors{
        0xff0000ff, 0x00ff00ff, 0x0000ffff, 0xffff00ff, 0xff00ffff, 0x00ffffff};
    std::array<uint8_t, size * size * 4 * colors.size()> pixels{};
    std::promise<void> completed;
    auto future = completed.get_future();
    runtime.Dispatch([&](Napi::Env env) {
        try
        {
            device.AddToJavaScript(env);
            Babylon::Plugins::NativeEngine::Initialize(env);
            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            auto frameScope = context.AcquireFrameCompletionScope();
            auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
            env.Global().Set("_testEngine", engine);
            auto createTexture = engine.Get("createTexture").As<Napi::Function>();
            auto initializeTexture = engine.Get("initializeTexture").As<Napi::Function>();
            auto createFrameBuffer = engine.Get("createFrameBuffer").As<Napi::Function>();
            auto plain = createTexture.Call(engine, {});
            initializeTexture.Call(engine, {
                plain, Napi::Number::New(env, size), Napi::Number::New(env, size),
                Napi::Boolean::New(env, false), Napi::Number::New(env, bgfx::TextureFormat::RGBA8),
                Napi::Boolean::New(env, true), Napi::Boolean::New(env, false)});
            EXPECT_FALSE(plain.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get()->IsCube());
            auto plainFrameBuffer = createFrameBuffer.Call(engine, {
                plain, Napi::Number::New(env, size), Napi::Number::New(env, size),
                Napi::Boolean::New(env, false), Napi::Boolean::New(env, false)});
            EXPECT_TRUE(bgfx::isValid(plainFrameBuffer.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get()->Handle()));

            auto value = createTexture.Call(engine, {});
            env.Global().Set("_testCube", value);
            initializeTexture.Call(engine, {
                value, Napi::Number::New(env, size), Napi::Number::New(env, size),
                Napi::Boolean::New(env, false), Napi::Number::New(env, bgfx::TextureFormat::RGBA8),
                Napi::Boolean::New(env, true), Napi::Boolean::New(env, false),
                Napi::Number::New(env, 1), Napi::Boolean::New(env, true)});
            auto* cube = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
            if (!cube->IsCube())
            {
                throw std::runtime_error{"initializeTexture ignored isCube"};
            }
            auto frameBuffers = Napi::Array::New(env, colors.size());
            env.Global().Set("_testFrameBuffers", frameBuffers);
            for (uint32_t face = 0; face < colors.size(); ++face)
            {
                auto frameBufferValue = createFrameBuffer.Call(engine, {
                    value, Napi::Number::New(env, size), Napi::Number::New(env, size),
                    Napi::Boolean::New(env, false), Napi::Boolean::New(env, false),
                    Napi::Number::New(env, 1), Napi::Number::New(env, face)});
                frameBuffers.Set(face, frameBufferValue);
                auto* frameBuffer = frameBufferValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
                frameBuffer->Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR, colors[face], 1.0f, 0);
            }
            EXPECT_THROW(createFrameBuffer.Call(engine, {
                value, Napi::Number::New(env, size), Napi::Number::New(env, size),
                Napi::Boolean::New(env, false), Napi::Boolean::New(env, false),
                Napi::Number::New(env, 1), Napi::Number::New(env, 6)}), Napi::Error);
            auto readback = std::make_shared<Babylon::Graphics::Texture>(context);
            readback->Create2D(size * colors.size(), size, false, 1, bgfx::TextureFormat::RGBA8,
                BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK);
            for (uint16_t face = 0; face < colors.size(); ++face)
            {
                bgfx::TextureRegion destination{};
                destination.init(readback->Handle(), face * size, 0, size, size);
                bgfx::TextureRegion source{};
                source.init(cube->Handle(), 0, 0, size, size);
                source.z = face;
                source.depth = 1;
                context.GetActiveEncoder()->blit(context.AcquireNewViewId(), destination, source);
            }
            context.ReadTextureAsync(readback->Handle(), gsl::make_span(pixels))
                .then(arcana::inline_scheduler, arcana::cancellation::none(), [readback, &completed](arcana::expected<void, std::exception_ptr> result) {
                    readback->Dispose();
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
        catch (const std::exception& ex)
        {
            completed.set_exception(std::make_exception_ptr(std::runtime_error{ex.what()}));
        }
    });
    while (future.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready)
    {
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }
    EXPECT_NO_THROW(future.get());
    for (size_t face = 0; face < colors.size(); ++face)
    {
        SCOPED_TRACE(face);
        for (size_t pixel = 0; pixel < size * size; ++pixel)
        {
            for (size_t channel = 0; channel < 4; ++channel)
            {
                const size_t offset = (pixel / size * size * colors.size() + face * size + pixel % size) * 4;
                EXPECT_EQ(pixels[offset + channel],
                    (colors[face] >> (24 - channel * 8)) & 0xff);
            }
        }
    }
    device.FinishRenderingCurrentFrame();
}
