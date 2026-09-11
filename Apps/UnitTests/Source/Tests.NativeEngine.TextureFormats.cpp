#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <napi/env.h>
#include <napi/pointer.h>

#include <chrono>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <stdexcept>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    void RunTextureTest(const std::function<void(Napi::Object, Napi::Value)>& test)
    {
        Babylon::Graphics::Device device{g_deviceConfig};
        device.StartRenderingCurrentFrame();
        std::promise<void> completed;
        auto completion = completed.get_future();
        {
            Babylon::AppRuntime runtime{};
            runtime.Dispatch([&](Napi::Env env) {
                try
                {
                    device.AddToJavaScript(env);
                    Babylon::Plugins::NativeEngine::Initialize(env);
                    auto run = Napi::Eval(env, R"(
                        (function(test) {
                            const engine = new _native.Engine();
                            const texture = engine.createTexture();
                            try {
                                test(engine, texture);
                            } finally {
                                engine.deleteTexture(texture);
                                engine.dispose();
                            }
                        })
                    )", "native-texture-format-test.js").As<Napi::Function>();
                    run.Call({Napi::Function::New(env, [&](const Napi::CallbackInfo& info) {
                        test(info[0].As<Napi::Object>(), info[1]);
                    })});
                    completed.set_value();
                }
                catch (const Napi::Error& error)
                {
                    completed.set_exception(std::make_exception_ptr(std::runtime_error{error.Message() + "\n" + Napi::GetErrorString(error)}));
                }
                catch (...)
                {
                    completed.set_exception(std::current_exception());
                }
            });
            if (completion.wait_for(std::chrono::seconds{30}) != std::future_status::ready)
            {
                // Do not enter a potentially blocked runtime destructor after a GPU assertion.
                std::cerr << "Timed out waiting for NativeEngine texture format test" << std::endl;
                std::quick_exit(1);
            }
        }
        device.FinishRenderingCurrentFrame();
        ASSERT_NO_THROW(completion.get());
    }

    void InitializeTexture(Napi::Object engine, Napi::Value texture, double format, bool renderTarget, bool srgb = false, uint32_t samples = 1)
    {
        const auto env = engine.Env();
        engine.Get("initializeTexture").As<Napi::Function>().Call(engine, {
            texture, Napi::Number::New(env, 16), Napi::Number::New(env, 16), Napi::Boolean::New(env, false),
            Napi::Number::New(env, format), Napi::Boolean::New(env, renderTarget), Napi::Boolean::New(env, srgb),
            Napi::Number::New(env, samples)});
    }

    void ExpectInitializationError(Napi::Object engine, Napi::Value texture, double format, bool renderTarget, bool srgb, const std::string& message, uint32_t samples = 1)
    {
        try
        {
            InitializeTexture(engine, texture, format, renderTarget, srgb, samples);
            FAIL() << "initializeTexture accepted an invalid texture request";
        }
        catch (const Napi::Error& error)
        {
            EXPECT_NE(error.Message().find(message), std::string::npos) << error.Message();
        }
    }

    bool IsSupported(bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        return bgfx::isTextureValid(0, false, 1, format, flags | BGFX_TEXTURE_BLIT_DST);
    }
}

TEST(NativeEngineTextureFormats, D24RenderTargetUsesSupportedBackingStorage)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        for (const uint32_t samples : {1u, 4u})
        {
            SCOPED_TRACE(samples);
            texture->Dispose();
            const auto flags = BGFX_TEXTURE_RT | (samples == 4 ? BGFX_TEXTURE_RT_MSAA_X4 : BGFX_TEXTURE_NONE);
            const auto expectedFormat = IsSupported(bgfx::TextureFormat::D24, flags)
                ? bgfx::TextureFormat::D24 : bgfx::TextureFormat::D24S8;
            if (!IsSupported(expectedFormat, flags))
            {
                ExpectInitializationError(engine, value, bgfx::TextureFormat::D24, true, false, "Unsupported texture format", samples);
                EXPECT_FALSE(texture->IsValid());
                continue;
            }

            InitializeTexture(engine, value, bgfx::TextureFormat::D24, true, false, samples);
            ASSERT_TRUE(texture->IsValid());
            EXPECT_EQ(texture->Format(), expectedFormat);
            EXPECT_EQ(texture->Width(), 16);
            EXPECT_EQ(texture->Height(), 16);
            EXPECT_EQ(texture->Flags(), flags);

            if (samples == 1)
            {
                bgfx::Attachment attachment{};
                attachment.init(texture->Handle(), bgfx::Access::Write, 0, 1, 0, BGFX_RESOLVE_NONE);
                const auto frameBuffer = bgfx::createFrameBuffer(1, &attachment, false);
                ASSERT_TRUE(bgfx::isValid(frameBuffer));
                bgfx::destroy(frameBuffer);
            }
        }
    });
}

TEST(NativeEngineTextureFormats, DoesNotSubstituteNonRenderTargetD24)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        if (!IsSupported(bgfx::TextureFormat::D24, BGFX_TEXTURE_NONE))
        {
            ExpectInitializationError(engine, value, bgfx::TextureFormat::D24, false, false, "Unsupported texture format");
            EXPECT_FALSE(texture->IsValid());
            return;
        }
        InitializeTexture(engine, value, bgfx::TextureFormat::D24, false);
        EXPECT_TRUE(texture->IsValid());
        EXPECT_EQ(texture->Format(), bgfx::TextureFormat::D24);
    });
}

TEST(NativeEngineTextureFormats, PreservesSupportedColorFormats)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        for (const bool renderTarget : {false, true})
        {
            InitializeTexture(engine, value, bgfx::TextureFormat::RGBA8, renderTarget);
            auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
            EXPECT_TRUE(texture->IsValid());
            EXPECT_EQ(texture->Format(), bgfx::TextureFormat::RGBA8);
            EXPECT_EQ(texture->Flags(), renderTarget ? BGFX_TEXTURE_RT : BGFX_TEXTURE_NONE);
        }
    });
}

TEST(NativeEngineTextureFormats, RejectedInitializationPreservesExistingTexture)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        InitializeTexture(engine, value, bgfx::TextureFormat::RGBA8, true);
        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        const auto originalHandle = texture->Handle();
        ExpectInitializationError(engine, value, std::numeric_limits<double>::quiet_NaN(), true, false,
            "Invalid texture format NaN: expected a finite integer in [0, " + std::to_string(bgfx::TextureFormat::Count) + ")");
        for (const double format : {
            static_cast<double>(bgfx::TextureFormat::Count), static_cast<double>(UINT32_MAX),
            4294967296.0 + static_cast<double>(bgfx::TextureFormat::RGBA8), -1.0,
            static_cast<double>(bgfx::TextureFormat::RGBA8) + 0.5,
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity(),
            -std::numeric_limits<double>::infinity()})
        {
            SCOPED_TRACE(format);
            ExpectInitializationError(engine, value, format, true, false, "Invalid texture format");
            EXPECT_EQ(texture->Handle().idx, originalHandle.idx);
            EXPECT_EQ(texture->Format(), bgfx::TextureFormat::RGBA8);
        }

        const auto flags = BGFX_TEXTURE_RT | BGFX_TEXTURE_SRGB;
        if (!IsSupported(bgfx::TextureFormat::D24, flags) && !IsSupported(bgfx::TextureFormat::D24S8, flags))
        {
            ExpectInitializationError(engine, value, bgfx::TextureFormat::D24, true, true, "Unsupported texture format");
            EXPECT_EQ(texture->Handle().idx, originalHandle.idx);
            EXPECT_EQ(texture->Format(), bgfx::TextureFormat::RGBA8);
        }
    });
}
