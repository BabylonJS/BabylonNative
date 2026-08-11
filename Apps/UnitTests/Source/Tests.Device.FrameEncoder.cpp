#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <chrono>
#include <future>
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
