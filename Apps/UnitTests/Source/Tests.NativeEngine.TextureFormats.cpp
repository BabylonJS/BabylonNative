#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <bgfx/bgfx.h>
#include <napi/env.h>
#include <napi/pointer.h>

#include <chrono>
#include <array>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

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
            device.FinishRenderingCurrentFrame();
        }
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
        for (const uint32_t samples : {1u, 2u, 4u})
        {
            SCOPED_TRACE(samples);
            texture->Dispose();
            const auto flags = samples == 1 ? BGFX_TEXTURE_RT :
                (samples == 2 ? BGFX_TEXTURE_RT_MSAA_X2 : BGFX_TEXTURE_RT_MSAA_X4) | BGFX_TEXTURE_MSAA_SAMPLE;
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

            bgfx::Attachment attachment{};
            attachment.init(texture->Handle(), bgfx::Access::Write, 0, 1, 0, BGFX_ATTACHMENT_NONE);
            const auto frameBuffer = bgfx::createFrameBuffer(1, &attachment, false);
            ASSERT_TRUE(bgfx::isValid(frameBuffer));
            bgfx::destroy(frameBuffer);
        }
    });
}

TEST(NativeEngineTextureFormats, DoesNotSubstituteNonRenderTargetD24)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        if (!IsSupported(bgfx::TextureFormat::D24, BGFX_TEXTURE_NONE))
        {
            ExpectInitializationError(engine, value, bgfx::TextureFormat::D24, false, false,
                "Unsupported texture format " + std::to_string(bgfx::TextureFormat::D24) +
                " for requested flags (renderTarget=false, cube=false, srgb=false, samples=1, createFlags=" +
                std::to_string(BGFX_TEXTURE_BLIT_DST) + ")");
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
            ExpectInitializationError(engine, value, bgfx::TextureFormat::D24, true, true,
                "Unsupported texture format " + std::to_string(bgfx::TextureFormat::D24) +
                " for requested flags (renderTarget=true, cube=false, srgb=true, samples=1, createFlags=" +
                std::to_string(flags | BGFX_TEXTURE_BLIT_DST) + ")");
            EXPECT_EQ(texture->Handle().idx, originalHandle.idx);
            EXPECT_EQ(texture->Format(), bgfx::TextureFormat::RGBA8);
        }
    });
}

TEST(NativeEngineTextureFormats, RejectedDimensionsAndLayersPreserveExistingTexture)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        const auto env = engine.Env();
        InitializeTexture(engine, value, bgfx::TextureFormat::RGBA8, true);
        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        const auto originalHandle = texture->Handle();
        for (const double invalid : {-1.0, 0.0, 0.5, 65536.0, 4294967296.0,
                 std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity()})
        {
            for (const size_t component : {size_t{1}, size_t{2}, size_t{9}})
            {
                if (component == 9 && invalid == 0)
                {
                    continue;
                }
                for (const bool volume : {false, true})
                {
                    std::vector<Napi::Value> args{
                        value, Napi::Number::New(env, 16), Napi::Number::New(env, 16),
                        Napi::Boolean::New(env, false), Napi::Number::New(env, bgfx::TextureFormat::RGBA8),
                        Napi::Boolean::New(env, true), Napi::Boolean::New(env, false), Napi::Number::New(env, 1),
                        Napi::Boolean::New(env, !volume), Napi::Number::New(env, 1), Napi::Boolean::New(env, volume)};
                    args[component] = Napi::Number::New(env, invalid);
                    if (!volume && component == 1)
                    {
                        args[2] = args[1];
                    }
                    EXPECT_THROW(engine.Get("initializeTexture").As<Napi::Function>().Call(engine, args), Napi::Error);
                    EXPECT_EQ(texture->Handle().idx, originalHandle.idx);
                    EXPECT_EQ(texture->Width(), 16u);
                    EXPECT_EQ(texture->Height(), 16u);
                }
            }
        }
    });
}

TEST(NativeEngineTextureFormats, TruncatesFractionalDimensionsBeforeAllocation)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        const auto env = engine.Env();
        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        for (const bool cube : {false, true})
        {
            engine.Get("initializeTexture").As<Napi::Function>().Call(engine, {
                value, Napi::Number::New(env, 16.9), Napi::Number::New(env, cube ? 16.9 : 12.4),
                Napi::Boolean::New(env, false), Napi::Number::New(env, bgfx::TextureFormat::RGBA8),
                Napi::Boolean::New(env, true), Napi::Boolean::New(env, false),
                Napi::Number::New(env, 1), Napi::Boolean::New(env, cube), Napi::Number::New(env, 0)});
            EXPECT_EQ(texture->Width(), 16u);
            EXPECT_EQ(texture->Height(), cube ? 16u : 12u);
        }
    });
}

TEST(NativeEngineTextureFormats, DepthOneVolumesPreserveLogicalBoundsAndPadding)
{
    Babylon::Graphics::Device device{g_deviceConfig};
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP() << "GPU rendering/readback is unavailable in this test configuration";
#endif
    device.StartRenderingCurrentFrame();
    if (!bgfx::isTextureValid(2, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | BGFX_TEXTURE_BLIT_DST))
    {
        GTEST_SKIP() << "Volume render targets are unavailable";
    }
    constexpr uint16_t width = 12;
    std::array<uint8_t, width * 2 * 4> pixels{};
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
            env.Global().Set("_volumeTestEngine", engine);
            auto textures = Napi::Array::New(env, 3);
            env.Global().Set("_volumeTestTextures", textures);
            auto readback = std::make_shared<Babylon::Graphics::Texture>(context);
            readback->Create2D(width, 2, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_READ_BACK);
            for (uint32_t test = 0; test < 3; ++test)
            {
                const auto value = engine.Get("createTexture").As<Napi::Function>().Call(engine, {});
                textures.Set(test, value);
                auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                engine.Get("initializeTexture").As<Napi::Function>().Call(engine, {
                    value, Napi::Number::New(env, 2), Napi::Number::New(env, 2),
                    Napi::Boolean::New(env, false), Napi::Number::New(env, bgfx::TextureFormat::RGBA8),
                    Napi::Boolean::New(env, test == 2), Napi::Boolean::New(env, false), Napi::Number::New(env, 1),
                    Napi::Boolean::New(env, false), Napi::Number::New(env, 1), Napi::Boolean::New(env, true)});
                EXPECT_EQ(texture->Depth(), 1);
                const auto createFrameBuffer = engine.Get("createFrameBuffer").As<Napi::Function>();
                std::vector<Napi::Value> frameBufferArgs{
                    value, Napi::Number::New(env, 2), Napi::Number::New(env, 2),
                    Napi::Boolean::New(env, false), Napi::Boolean::New(env, false),
                    Napi::Number::New(env, 1), Napi::Number::New(env, 1)};
                EXPECT_THROW(createFrameBuffer.Call(engine, frameBufferArgs), Napi::Error);
                frameBufferArgs.back() = Napi::Number::New(env, 0);
                if (test == 1)
                {
#ifdef HAS_NATIVE_IMAGE_LOADING
                    auto data = Napi::Uint8Array::New(env, 16);
                    std::fill_n(data.Data(), data.ElementLength(), uint8_t{255});
                    engine.Get("loadRawTexture3D").As<Napi::Function>().Call(engine, {
                        value, data, Napi::Number::New(env, 2), Napi::Number::New(env, 2), Napi::Number::New(env, 1),
                        Napi::Number::New(env, bgfx::TextureFormat::RGBA8), Napi::Boolean::New(env, false), Napi::Boolean::New(env, false)});
                    EXPECT_EQ(texture->Depth(), 1);
                    frameBufferArgs.back() = Napi::Number::New(env, 1);
                    EXPECT_THROW(createFrameBuffer.Call(engine, frameBufferArgs), Napi::Error);
#else
                    std::array<uint8_t, 16> data{};
                    data.fill(255);
                    texture->Update3D(0, 0, 0, 0, 2, 2, 1, bgfx::copy(data.data(), uint32_t(data.size())));
#endif
                    const std::array<uint8_t, 4> partial{17, 33, 65, 255};
                    texture->Update3D(0, 1, 1, 0, 1, 1, 1, bgfx::copy(partial.data(), uint32_t(partial.size())));
                }
                else if (test == 2)
                {
                    const auto frameBufferValue = createFrameBuffer.Call(engine, frameBufferArgs);
                    auto* frameBuffer = frameBufferValue.As<Napi::Pointer<Babylon::Graphics::FrameBuffer>>().Get();
                    const auto bind = Napi::Eval(env, R"(
                        (function(engine, frameBuffer) {
                            const stream = new _native.NativeDataStream(function() {});
                            engine.setCommandDataStream({_nativeDataStream: stream});
                            return function(bind) {
                                const command = bind ? _native.Engine.COMMAND_BINDFRAMEBUFFER : _native.Engine.COMMAND_UNBINDFRAMEBUFFER;
                                const words = new Uint32Array(command.length + frameBuffer.length);
                                words.set(command);
                                words.set(frameBuffer, command.length);
                                stream.writeBuffer(words.buffer, words.length);
                                engine.submitCommands();
                            };
                        })
                    )", "depth-one-volume-bind.js").As<Napi::Function>().Call({engine, frameBufferValue}).As<Napi::Function>();
                    bind.Call({Napi::Boolean::New(env, true)});
                    frameBuffer->Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR, 1.f, 0.f, 1.f, 1.f, 1.f, 0);
                    bind.Call({Napi::Boolean::New(env, false)});
                }
                for (uint16_t slice = 0; slice < 2; ++slice)
                {
                    bgfx::TextureRegion source{};
                    source.init(texture->Handle(), 0, 0, 2, 2);
                    source.z = slice;
                    source.depth = 1;
                    bgfx::TextureRegion destination{};
                    destination.init(readback->Handle(), uint16_t(test * 4 + slice * 2), 0, 2, 2);
                    context.GetActiveEncoder()->blit(context.AcquireNewViewId(), destination, source);
                }
            }
            context.ReadTextureAsync(readback->Handle(), gsl::make_span(pixels))
                .then(arcana::inline_scheduler, arcana::cancellation::none(), [readback, &completed](arcana::expected<void, std::exception_ptr> result) {
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
    EXPECT_NO_THROW(future.get());
    for (uint16_t y = 0; y < 2; ++y)
    {
        for (uint16_t x = 0; x < width; ++x)
        {
            const auto test = x / 4;
            const std::array<uint8_t, 4> expected = test == 0 ? std::array<uint8_t, 4>{} :
                test == 2 ? std::array<uint8_t, 4>{255, 0, 255, 255} :
                (x % 2 == 1 && y == 1) ? std::array<uint8_t, 4>{17, 33, 65, 255} :
                                       std::array<uint8_t, 4>{255, 255, 255, 255};
            for (size_t channel = 0; channel < 4; ++channel)
            {
                EXPECT_EQ(pixels[(y * width + x) * 4 + channel], expected[channel]) << x << ", " << y << ", " << channel;
            }
        }
    }
    device.FinishRenderingCurrentFrame();
}

TEST(NativeEngineTextureFormats, RejectsComparisonEnumsWithoutChangingSampler)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        const auto env = engine.Env();
        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        constexpr uint32_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_COMPARE_LESS;
        texture->SamplerFlags(flags);
        auto setComparison = engine.Get("setTextureComparisonFunction").As<Napi::Function>();
        for (const double invalid : {-1.0, 1.0, 520.0, 0x0201 + 0.5, 4294967296.0,
                 std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity()})
        {
            EXPECT_THROW(setComparison.Call(engine, {value, Napi::Number::New(env, invalid)}), Napi::Error);
            EXPECT_EQ(texture->SamplerFlags(), flags);
        }
        EXPECT_NO_THROW(setComparison.Call(engine, {value, Napi::Number::New(env, 0)}));
        EXPECT_EQ(texture->SamplerFlags(), BGFX_SAMPLER_U_CLAMP);
    });
}

TEST(NativeEngineTextureFormats, SamplingModesSeparateLodClampFromFilterFlags)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        EXPECT_EQ(texture->SamplerMaxLod(), UINT8_MAX);
        constexpr uint32_t preservedFlags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_COMPARE_LESS;
        texture->SamplerFlags(preservedFlags);

        auto setSampling = Napi::Eval(engine.Env(), R"(
            (function(engine, texture) {
                const stream = new _native.NativeDataStream(function() {});
                engine.setCommandDataStream({_nativeDataStream: stream});
                return function(mode) {
                    const command = _native.Engine.COMMAND_SETTEXTURESAMPLING;
                    const words = new Uint32Array(command.length + texture.length + 1);
                    words.set(command);
                    words.set(texture, command.length);
                    words[words.length - 1] = _native.Engine[mode];
                    stream.writeBuffer(words.buffer, words.length);
                    engine.submitCommands();
                };
            })
        )", "native-texture-sampling-test.js").As<Napi::Function>()
                               .Call({engine, value}).As<Napi::Function>();

        struct SamplingCase
        {
            const char* Name;
            uint32_t Flags;
            uint8_t MaxLod;
        };
        const SamplingCase cases[]{
            {"TEXTURE_NEAREST_NEAREST", BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIN_POINT, 0},
            {"TEXTURE_NEAREST_NEAREST_MIPNEAREST", BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MIP_POINT, UINT8_MAX},
            {"TEXTURE_LINEAR_LINEAR", 0, 0},
            {"TEXTURE_LINEAR_LINEAR_MIPLINEAR", 0, UINT8_MAX},
            {"TEXTURE_NEAREST_LINEAR", BGFX_SAMPLER_MAG_POINT, 0},
            {"TEXTURE_NEAREST_LINEAR_MIPNEAREST", BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT, UINT8_MAX},
            {"TEXTURE_LINEAR_NEAREST", BGFX_SAMPLER_MIN_POINT, 0},
            {"TEXTURE_LINEAR_NEAREST_MIPNEAREST", BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MIP_POINT, UINT8_MAX},
            {"TEXTURE_NEAREST_LINEAR_MIPLINEAR", BGFX_SAMPLER_MAG_POINT, UINT8_MAX},
            {"TEXTURE_NEAREST_NEAREST_MIPLINEAR", BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIN_POINT, UINT8_MAX},
            {"TEXTURE_LINEAR_NEAREST_MIPLINEAR", BGFX_SAMPLER_MIN_POINT, UINT8_MAX},
            {"TEXTURE_LINEAR_LINEAR_MIPNEAREST", BGFX_SAMPLER_MIP_POINT, UINT8_MAX},
        };
        for (const auto& test : cases)
        {
            SCOPED_TRACE(test.Name);
            setSampling.Call({Napi::String::New(engine.Env(), test.Name)});
            EXPECT_EQ(texture->SamplerFlags(), preservedFlags | test.Flags);
            EXPECT_EQ(texture->SamplerMaxLod(), test.MaxLod);
        }

    });
}

TEST(NativeEngineCommands, LegacyAndExtendedPayloadsKeepFollowingCommandsAligned)
{
    RunTextureTest([](Napi::Object engine, Napi::Value value) {
        auto* texture = value.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        auto run = Napi::Eval(engine.Env(), R"(
            (function(engine, texture) {
                if (_native.Engine.PROTOCOL_VERSION !== 9) throw new Error("Expected protocol 9");
                const commands = _native.Engine;
                const stream = new _native.NativeDataStream(function() {});
                engine.setCommandDataStream({_nativeDataStream: stream});
                const clear = [0, 0, 0, 0, 0x3f800000, 0, 0x3f800000, 0, 0];
                const stencil = [255, commands.STENCIL_OP_FAIL_S_KEEP, commands.STENCIL_OP_FAIL_Z_KEEP,
                    commands.STENCIL_OP_PASS_Z_KEEP, commands.STENCIL_TEST_ALWAYS, 0];
                const words = [];
                function append(command, values) {
                    words.push(...command, ...values);
                    words.push(...commands.COMMAND_SETTEXTURESAMPLING, ...texture, commands.TEXTURE_NEAREST_NEAREST);
                }
                append(commands.COMMAND_CLEAR, clear);
                append(commands.COMMAND_SETSTENCIL2, stencil.concat(3));
                append(commands.COMMAND_CLEAR2, clear.concat(0));
                append(commands.COMMAND_SETSTENCIL, stencil);
                const data = new Uint32Array(words);
                stream.writeBuffer(data.buffer, data.length);
                engine.submitCommands();
            })
        )", "native-command-versions.js").As<Napi::Function>();
        EXPECT_NO_THROW(run.Call({engine, value}));
        EXPECT_EQ(texture->SamplerFlags() & (BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT),
            BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        EXPECT_EQ(texture->SamplerMaxLod(), 0);
    });
}
