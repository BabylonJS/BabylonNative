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
#include <future>
#include <limits>
#include <memory>
#include <stdexcept>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(NativeEngineCubeRenderTargets, GeneratedImageMipsPreserveRowsAndColumns)
{
    Babylon::Graphics::Device device{g_deviceConfig};
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP() << "GPU rendering/readback is unavailable in this test configuration";
#endif
    // An 8x8 PNG with red increasing across X and green increasing across Y.
    constexpr std::array<uint8_t, 86> png{
        0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
        0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0x0f, 0xbe,
        0x8b, 0x00, 0x00, 0x00, 0x1d, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x63, 0x60, 0x60, 0x60, 0xf8,
        0x8f, 0x8c, 0xff, 0xa3, 0x61, 0x06, 0x7a, 0x28, 0xf8, 0x8f, 0x0a, 0xff, 0xa3, 0x61, 0x3a, 0x28,
        0x00, 0x00, 0x6b, 0xb0, 0x7f, 0x81, 0x52, 0xa1, 0x36, 0x78, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
        0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};
    constexpr uint16_t readbackWidth = 6 * (8 + 4 + 2);
    std::array<uint8_t, readbackWidth * 8 * 4> pixels{};
    std::promise<void> completed;
    auto future = completed.get_future();
    device.StartRenderingCurrentFrame();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env env) {
        try
        {
            device.AddToJavaScript(env);
            Babylon::Plugins::NativeEngine::Initialize(env);
            auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
            auto value = engine.Get("createTexture").As<Napi::Function>().Call(engine, {});
            env.Global().Set("_testEngine", engine);
            env.Global().Set("_testCube", value);
            auto bytes = Napi::Uint8Array::New(env, png.size());
            std::copy(png.begin(), png.end(), bytes.Data());
            auto faces = Napi::Array::New(env, 6);
            for (uint32_t face = 0; face < 6; ++face)
            {
                faces.Set(face, bytes);
            }
            auto onLoaded = Napi::Function::New(env, [&](const Napi::CallbackInfo& info) {
                try
                {
                    auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(info.Env());
                    auto frameScope = context.AcquireFrameCompletionScope();
                    auto* cube = info.Env().Global().Get("_testCube").As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                    auto readback = std::make_shared<Babylon::Graphics::Texture>(context);
                    readback->Create2D(readbackWidth, 8, false, 1, bgfx::TextureFormat::RGBA8,
                        BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK);
                    uint16_t destinationX{};
                    for (uint16_t face = 0; face < 6; ++face)
                    {
                        for (uint8_t mip = 0; mip < 3; ++mip)
                        {
                            const uint16_t size = 8 >> mip;
                            bgfx::TextureRegion source{};
                            source.init(cube->Handle(), 0, 0, size, size);
                            source.z = face;
                            source.depth = 1;
                            source.mip = mip;
                            bgfx::TextureRegion destination{};
                            destination.init(readback->Handle(), destinationX, 0, size, size);
                            context.GetActiveEncoder()->blit(context.AcquireNewViewId(), destination, source);
                            destinationX += size;
                        }
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
                catch (...)
                {
                    completed.set_exception(std::current_exception());
                }
            });
            auto onError = Napi::Function::New(env, [&](const Napi::CallbackInfo&) {
                completed.set_exception(std::make_exception_ptr(std::runtime_error{"Cube image loading failed"}));
            });
            engine.Get("loadCubeTexture").As<Napi::Function>().Call(engine, {
                value, faces, Napi::Boolean::New(env, true), Napi::Boolean::New(env, false),
                Napi::Boolean::New(env, false), onLoaded, onError});
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
    EXPECT_NO_THROW(future.get());
    size_t sourceX{};
    for (uint16_t face = 0; face < 6; ++face)
    {
        SCOPED_TRACE(face);
        for (uint8_t mip = 0; mip < 3; ++mip)
        {
            SCOPED_TRACE(mip);
            const size_t size = 8 >> mip;
            for (size_t y = 0; y < size; ++y)
            {
                for (size_t x = 0; x < size; ++x)
                {
                    const size_t offset = (y * readbackWidth + sourceX + x) * 4;
                    EXPECT_EQ(pixels[offset], x < size / 2 ? 0 : 255);
                    EXPECT_EQ(pixels[offset + 1], y < size / 2 ? 255 : 0);
                    EXPECT_EQ(pixels[offset + 2], 0);
                    EXPECT_EQ(pixels[offset + 3], 255);
                }
            }
            sourceX += size;
        }
    }
    device.FinishRenderingCurrentFrame();
}

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
            auto depthOnlyFrameBuffer = createFrameBuffer.Call(engine, {
                env.Null(), Napi::Number::New(env, size), Napi::Number::New(env, size),
                Napi::Boolean::New(env, false), Napi::Boolean::New(env, true)});
            EXPECT_TRUE(bgfx::isValid(depthOnlyFrameBuffer.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get()->Handle()));

            auto value = createTexture.Call(engine, {});
            env.Global().Set("_testCube", value);
            for (const uint32_t height : {uint32_t{size} * 2, uint32_t{size} + 65536})
            {
                SCOPED_TRACE(height);
                EXPECT_THROW(initializeTexture.Call(engine, {
                    value, Napi::Number::New(env, size), Napi::Number::New(env, height),
                    Napi::Boolean::New(env, false), Napi::Number::New(env, bgfx::TextureFormat::RGBA8),
                    Napi::Boolean::New(env, true), Napi::Boolean::New(env, false),
                    Napi::Number::New(env, 1), Napi::Boolean::New(env, true)}), Napi::Error);
            }
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
                const auto color = colors[face];
                frameBuffer->Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR,
                    static_cast<float>((color >> 24) & 0xff) / 255.0f,
                    static_cast<float>((color >> 16) & 0xff) / 255.0f,
                    static_cast<float>((color >> 8) & 0xff) / 255.0f,
                    static_cast<float>(color & 0xff) / 255.0f, 1.0f, 0);
            }
            const auto expectInvalidLayer = [&](Napi::Value texture, double layer) {
                SCOPED_TRACE(layer);
                EXPECT_THROW(createFrameBuffer.Call(engine, {
                    texture, Napi::Number::New(env, size), Napi::Number::New(env, size),
                    Napi::Boolean::New(env, false), Napi::Boolean::New(env, true),
                    Napi::Number::New(env, 1), Napi::Number::New(env, layer)}), Napi::Error);
            };
            for (const double layer : {-1.0, 0.5, 6.0, 65536.0, 4294967296.0,
                     std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity()})
            {
                expectInvalidLayer(value, layer);
                expectInvalidLayer(plain, layer);
                expectInvalidLayer(env.Null(), layer);
            }
            for (const double layer : {1.0, 5.0})
            {
                expectInvalidLayer(plain, layer);
                expectInvalidLayer(env.Null(), layer);
            }
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
