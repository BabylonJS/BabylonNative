#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <Babylon/Graphics/Texture.h>
#ifdef HAS_TEST_UTILS
#include <Babylon/Plugins/TestUtils.h>
#endif

#include <chrono>
#include <array>
#include <future>
#include <memory>
#include <optional>

extern Babylon::Graphics::Configuration g_deviceConfig;

// Regression coverage for BabylonJS/BabylonNative#1767.
//
// NativeXr::Impl::BeginUpdate creates its per-view framebuffers in a continuation chained as
// make_task(AfterRenderScheduler).then(runtimeScheduler, ...), and then performed the WebXR
// implicit clear with:
//
//     frameBuffer.Clear(*GraphicsContext.GetActiveEncoder(), ...);
//
// AfterRenderScheduler is ticked at the tail of FinishRenderingCurrentFrame, i.e. after the frame
// encoder has already been ended and nulled, so that continuation runs on the runtime thread with
// no frame in flight and the dereference crashed (EXC_BAD_ACCESS on iOS/ARKit).
//
// NativeXr itself is only built for Android and iOS, so these tests pin the underlying
// DeviceContext contract that the fix relies on, which is reproducible on any desktop backend:
//
//   1. Outside a frame, GetActiveEncoder() is null -- the precondition that made the old code
//      dereference a null pointer.
//   2. Holding a FrameCompletionScope guarantees a non-null encoder, because
//      StartRenderingCurrentFrame publishes the encoder *before* it opens the gate that
//      FrameCompletionScope acquisition waits on.
//
// Both tests drive frames from the test thread only (BABYLON_NATIVE_CHECK_THREAD_AFFINITY is on in
// CI, and Start/FinishRenderingCurrentFrame are render-thread affine); the AppRuntime thread stands
// in for the runtime/JS thread that the XR continuation runs on.

namespace
{
    // Runs `callback` on the AppRuntime (runtime/JS) thread and blocks until it returns.
    template<typename CallbackT>
    void RunOnRuntimeThread(Babylon::AppRuntime& runtime, CallbackT callback)
    {
        std::promise<void> completed;
        auto future = completed.get_future();
        runtime.Dispatch([&completed, &callback](Napi::Env env) {
            callback(env);
            completed.set_value();
        });
        future.wait();
    }
}

// The state the crash depended on: between frames there is no encoder to dereference.
TEST(Device, ActiveEncoderIsNullOutsideFrame)
{
    Babylon::Graphics::Device device{g_deviceConfig};

    // Drive one complete frame so bgfx is initialized and a frame has genuinely started and
    // finished. FinishRenderingCurrentFrame ends the encoder and clears it.
    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();

    Babylon::AppRuntime runtime{};

    bgfx::Encoder* encoderOutsideFrame{reinterpret_cast<bgfx::Encoder*>(~uintptr_t{0})};
    RunOnRuntimeThread(runtime, [&device, &encoderOutsideFrame](Napi::Env env) {
        device.AddToJavaScript(env);
        auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
        encoderOutsideFrame = context.GetActiveEncoder();
    });

    // No frame is in flight (the test thread is the only frame driver and it is parked here), so
    // this is deterministic. Pre-#1767-fix NativeXr dereferenced exactly this value.
    EXPECT_EQ(encoderOutsideFrame, nullptr)
        << "GetActiveEncoder() must report null outside a frame so callers can tell that "
           "dereferencing it is unsafe";
}

TEST(Device, FrameBufferMultisamplingTracksItsOwnTarget)
{
    auto config = g_deviceConfig;
    config.MSAASamples = 4;
    Babylon::Graphics::Device device{config};
    device.StartRenderingCurrentFrame();
    Babylon::AppRuntime runtime{};
    RunOnRuntimeThread(runtime, [&device](Napi::Env env) {
        device.AddToJavaScript(env);
        auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
        Babylon::Graphics::FrameBuffer backBuffer{context, BGFX_INVALID_HANDLE, 0, 0, true, true, true};
        Babylon::Graphics::FrameBuffer singleSample{context, BGFX_INVALID_HANDLE, 64, 64, false, true, true};
        Babylon::Graphics::FrameBuffer multisample{context, BGFX_INVALID_HANDLE, 64, 64, false, true, true, -1, true};
        const auto xrHandle = bgfx::createFrameBuffer(64, 64, bgfx::TextureFormat::RGBA8);
        ASSERT_TRUE(bgfx::isValid(xrHandle));
        Babylon::Graphics::FrameBuffer xrBackBuffer{context, xrHandle, 64, 64, true, false, false};
        EXPECT_EQ(context.GetMSAASamples(), 4);
        EXPECT_TRUE(backBuffer.IsMultisampled());
#ifdef HAS_TEST_UTILS
        Babylon::Plugins::TestUtils::Initialize(env, g_deviceConfig.Window);
        const auto testUtils = env.Global().Get("TestUtils").As<Napi::Object>();
        const auto setSamples = testUtils.Get("setMSAASamples").As<Napi::Function>();
#endif
        for (const uint8_t samples : {0, 1, 2, 4, 8, 16})
        {
#ifdef HAS_TEST_UTILS
            setSamples.Call(testUtils, {Napi::Number::New(env, samples)});
#else
            context.UpdateMSAA(samples);
#endif
            EXPECT_EQ(context.GetMSAASamples(), samples == 0 ? 1 : samples);
            EXPECT_EQ(backBuffer.IsMultisampled(), samples > 1);
            EXPECT_FALSE(singleSample.IsMultisampled());
            EXPECT_TRUE(multisample.IsMultisampled());
            EXPECT_FALSE(xrBackBuffer.IsMultisampled());
        }
#ifdef HAS_TEST_UTILS
        const auto rejectsInvalid = Napi::Eval(env, R"(
            (function() {
                const invalid = [-1, 3, 4.5, 256, NaN, Infinity, undefined, null, "4",
                    true, false, {}, [], new Number(4)];
                return invalid.every(function(value) {
                    try {
                        TestUtils.setMSAASamples(value);
                        return false;
                    } catch (error) {
                        return error instanceof Error;
                    }
                });
            })()
        )", "validation-msaa-options.js");
        EXPECT_TRUE(rejectsInvalid.As<Napi::Boolean>().Value());
        EXPECT_THROW(setSamples.Call(testUtils, {}), Napi::Error);
        EXPECT_EQ(context.GetMSAASamples(), 16);
#endif
    });
    device.FinishRenderingCurrentFrame();
}

TEST(Device, ClearPalettePreservesPendingColorsAndResetsAfterSubmission)
{
    Babylon::Graphics::Device device{g_deviceConfig};
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP() << "GPU rendering/readback is unavailable in this test configuration";
#endif
    device.StartRenderingCurrentFrame();
    if (!bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_RT))
    {
        GTEST_SKIP() << "RGBA32F render targets are unavailable";
    }
    constexpr uint16_t width = 40;
    std::array<std::array<float, 4>, width> colors{};
    for (uint16_t x = 0; x < width; ++x)
    {
        colors[x] = x < 24 ? std::array<float, 4>{float(x) / 255.f, float(x * 3) / 255.f, 0.f, 1.f}
                          : std::array<float, 4>{float(x), -0.5f, 0.125f, 1.f};
    }
    std::array<float, width * 4> pixels{};
    std::promise<void> completed;
    auto future = completed.get_future();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env env) {
        try
        {
            device.AddToJavaScript(env);
            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            auto scope = context.AcquireFrameCompletionScope();
            Babylon::Graphics::Texture target{context};
            target.Create2D(width, 1, false, 1, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_RT);
            const auto handle = target.Handle();
            Babylon::Graphics::FrameBuffer frameBuffer{context, bgfx::createFrameBuffer(1, &handle), width, 1, false, false, false};
            for (uint16_t x = 0; x < width; ++x)
            {
                frameBuffer.SetScissor(float(x), 0, 1, 1);
                const auto& color = colors[x];
                frameBuffer.Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR, color[0], color[1], color[2], color[3], 1.f, 0);
            }
            const auto& last = colors.back();
            EXPECT_NO_THROW(frameBuffer.Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR, last[0], last[1], last[2], last[3], 1.f, 0));
            EXPECT_THROW(frameBuffer.Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR, 100.f, 0.f, 0.f, 1.f, 1.f, 0), std::runtime_error);
            auto readback = std::make_shared<Babylon::Graphics::Texture>(context);
            readback->Create2D(width, 1, false, 1, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_READ_BACK);
            bgfx::TextureRegion source{};
            source.init(handle, 0, 0, width, 1);
            auto destination = source;
            destination.handle = readback->Handle();
            context.GetActiveEncoder()->blit(context.AcquireNewViewId(), destination, source);
            context.ReadTextureAsync(readback->Handle(), gsl::make_span(reinterpret_cast<uint8_t*>(pixels.data()), sizeof(pixels)))
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
    for (size_t x = 0; x < width; ++x)
    {
        for (size_t channel = 0; channel < 4; ++channel)
        {
            EXPECT_NEAR(pixels[x * 4 + channel], colors[x][channel], 0.000001f) << x << ", " << channel;
        }
    }
    device.FinishRenderingCurrentFrame();
    RunOnRuntimeThread(runtime, [](Napi::Env env) {
        auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
        EXPECT_EQ(context.AcquireClearPaletteIndex({100.f, 0.f, 0.f, 1.f}), 0);
    });
    device.DisableRendering();
    device.EnableRendering();
    RunOnRuntimeThread(runtime, [](Napi::Env env) {
        auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
        EXPECT_EQ(context.AcquireClearPaletteIndex({200.f, 0.f, 0.f, 1.f}), 0);
    });
}

// The guarantee the fix relies on: acquiring a FrameCompletionScope blocks until a frame is live,
// and once acquired the encoder is valid and cannot be ended underneath the holder.
TEST(Device, FrameCompletionScopeProvidesEncoderOutsideFrame)
{
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();

    Babylon::AppRuntime runtime{};

    Babylon::Graphics::DeviceContext* context{};
    RunOnRuntimeThread(runtime, [&device, &context](Napi::Env env) {
        device.AddToJavaScript(env);
        context = &Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
    });
    ASSERT_NE(context, nullptr);

    // Model the NativeXr continuation: it wakes up on the runtime thread between frames, finds no
    // encoder, and acquires a scope to get one. The acquisition blocks until the test thread starts
    // the next frame below.
    std::promise<bgfx::Encoder*> encoderUnderScope;
    auto encoderFuture = encoderUnderScope.get_future();
    runtime.Dispatch([context, &encoderUnderScope](Napi::Env) {
        std::optional<Babylon::Graphics::FrameCompletionScope> scope;
        if (context->GetActiveEncoder() == nullptr)
        {
            scope.emplace(context->AcquireFrameCompletionScope());
        }

        encoderUnderScope.set_value(context->GetActiveEncoder());
        // scope is released here, unblocking FinishRenderingCurrentFrame below.
    });

    // The runtime thread should still be blocked: nothing has opened the gate yet.
    EXPECT_EQ(encoderFuture.wait_for(std::chrono::milliseconds(100)), std::future_status::timeout)
        << "AcquireFrameCompletionScope must block while no frame is in flight";

    // Starting the frame publishes the encoder and then opens the gate, releasing the runtime thread.
    device.StartRenderingCurrentFrame();

    ASSERT_EQ(encoderFuture.wait_for(std::chrono::seconds(30)), std::future_status::ready)
        << "AcquireFrameCompletionScope never unblocked after the frame started";

    // FinishRenderingCurrentFrame waits for outstanding scopes, so it cannot have ended the encoder
    // while the runtime thread was holding one.
    device.FinishRenderingCurrentFrame();

    EXPECT_NE(encoderFuture.get(), nullptr)
        << "holding a FrameCompletionScope must guarantee a usable encoder; this is what lets the "
           "NativeXr framebuffer-creation clear run safely off-frame";
}
