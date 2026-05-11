#include "RuntimeImpl.h"

#if BABYLON_NATIVE_PLUGIN_NATIVEENGINE
#include <Babylon/Plugins/NativeEngine.h>
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVECAMERA
#include <Babylon/Plugins/NativeCamera.h>
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVECAPTURE
#include <Babylon/Plugins/NativeCapture.h>
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVEENCODING
#include <Babylon/Plugins/NativeEncoding.h>
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS
#include <Babylon/Plugins/NativeOptimizations.h>
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVETRACING
#include <Babylon/Plugins/NativeTracing.h>
#endif
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
#include <Babylon/Plugins/ShaderCache.h>
#endif
#if BABYLON_NATIVE_PLUGIN_TESTUTILS
#include <Babylon/Plugins/TestUtils.h>
#endif

#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Performance.h>
#include <Babylon/Polyfills/TextDecoder.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#if BABYLON_NATIVE_POLYFILL_WINDOW
#include <Babylon/Polyfills/Window.h>
#endif

#include <cassert>
#include <sstream>

namespace Babylon::Integrations
{
    namespace
    {
        // Forward Babylon Console levels to the LogLevel exposed on
        // RuntimeOptions::log so consumers don't have to depend on the
        // Console polyfill header to read log output.
        LogLevel ToIntegrationsLogLevel(Babylon::Polyfills::Console::LogLevel level)
        {
            switch (level)
            {
                case Babylon::Polyfills::Console::LogLevel::Log: return LogLevel::Log;
                case Babylon::Polyfills::Console::LogLevel::Warn: return LogLevel::Warn;
                case Babylon::Polyfills::Console::LogLevel::Error: return LogLevel::Error;
            }
            return LogLevel::Log;
        }
    }

    // ---------------------------------------------------------------------
    // First-Attach engine initialization: dispatched onto the JS thread by
    // the first View::Attach call. Runs all plugin/polyfill Initialize()
    // calls in the same order as Apps/Playground/Shared/AppContext.cpp,
    // then completes m_initTcs to unblock any LoadScript / Eval /
    // RunOnJsThread calls the host queued before the first Attach.
    //
    // After m_initTcs is complete, subsequent host calls to
    // Runtime::LoadScript / Eval / RunOnJsThread fire their continuation
    // synchronously on the calling thread (via inline_scheduler), which
    // then submits to ScriptLoader directly.
    // ---------------------------------------------------------------------
    static void RunFirstAttachInit(RuntimeImpl& impl, Babylon::Graphics::WindowT window)
    {
        impl.m_appRuntime->Dispatch([implPtr = &impl, window](Napi::Env env) {
            // 1. Make the Device available to JS.
            implPtr->m_device->AddToJavaScript(env);

            // 2. Polyfills (always-on).
            Babylon::Polyfills::Blob::Initialize(env);

            {
                const auto userLog = implPtr->m_options.log;
                Babylon::Polyfills::Console::Initialize(env,
                    [userLog](const char* message, Babylon::Polyfills::Console::LogLevel level) {
                        if (userLog && message)
                        {
                            userLog(ToIntegrationsLogLevel(level), message);
                        }
                    });
            }

            Babylon::Polyfills::Performance::Initialize(env);

#if BABYLON_NATIVE_POLYFILL_WINDOW
            Babylon::Polyfills::Window::Initialize(env);
#endif

            Babylon::Polyfills::TextDecoder::Initialize(env);
            Babylon::Polyfills::XMLHttpRequest::Initialize(env);

#if BABYLON_NATIVE_POLYFILL_CANVAS
            implPtr->m_canvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
#endif

            // 3. Plugins.
#if BABYLON_NATIVE_PLUGIN_NATIVETRACING
            Babylon::Plugins::NativeTracing::Initialize(env);
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVEENCODING
            Babylon::Plugins::NativeEncoding::Initialize(env);
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVEENGINE
            Babylon::Plugins::NativeEngine::Initialize(env);
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS
            Babylon::Plugins::NativeOptimizations::Initialize(env);
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVECAPTURE
            Babylon::Plugins::NativeCapture::Initialize(env);
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVECAMERA
            Babylon::Plugins::NativeCamera::Initialize(env);
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
            implPtr->m_input = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
#endif
#if BABYLON_NATIVE_PLUGIN_NATIVEXR
            // Initialize NativeXr; apply any pending xr window the host
            // may have already supplied via Runtime::SetXrWindow; wire
            // the session-state callback to keep m_isXrActive in sync.
            {
                std::lock_guard<std::mutex> xrLock{implPtr->m_xrMutex};
                implPtr->m_nativeXr.emplace(Babylon::Plugins::NativeXr::Initialize(env));
                if (implPtr->m_xrWindow)
                {
                    implPtr->m_nativeXr->UpdateWindow(implPtr->m_xrWindow);
                }
                implPtr->m_nativeXr->SetSessionStateChangedCallback(
                    [implPtr](bool isActive) {
                        implPtr->m_isXrActive.store(isActive, std::memory_order_relaxed);
                    });
            }
#endif
#if BABYLON_NATIVE_PLUGIN_TESTUTILS
            Babylon::Plugins::TestUtils::Initialize(env, window);
#else
            (void)window;
#endif

            // 4. Unblock any LoadScript / Eval / RunOnJsThread calls
            //    the host registered before first Attach. Each was
            //    chained off m_initTcs.as_task().then(inline_scheduler,
            //    ..., [...] { scriptLoader->...; });, so completing the
            //    TCS here causes those continuations to fire (in
            //    registration order) on the JS thread, each submitting
            //    to ScriptLoader's task chain.
            implPtr->m_initTcs.complete();
        });
    }

    // ---------------------------------------------------------------------
    // View::Attach (first time and subsequent)
    // ---------------------------------------------------------------------
    std::unique_ptr<View> View::Attach(Runtime& runtime, Babylon::Graphics::WindowT nativeWindow)
    {
        RuntimeImpl& impl = *runtime.m_impl;

        assert(impl.m_currentView == nullptr && "Only one View may be attached at a time.");
        if (impl.m_currentView != nullptr)
        {
            return nullptr;
        }

        const bool firstAttach = !impl.m_device;

        // Per-platform: query the surface's pixel-buffer size from the
        // native window handle. ViewImpl_*.cpp implements this; e.g.
        // ANativeWindow_getWidth on Android, GetClientRect on Win32,
        // CAMetalLayer.drawableSize on Apple.
        const auto [width, height] = ViewImpl::QuerySize(nativeWindow);

        if (firstAttach)
        {
            // First Attach on this Runtime: construct the Device.
            Babylon::Graphics::Configuration config{};
            config.Window = nativeWindow;
            config.Width = width;
            config.Height = height;
            config.MSAASamples = impl.m_options.msaaSamples;

            impl.m_device.emplace(config);
            impl.m_deviceUpdate.emplace(impl.m_device->GetUpdate("update"));

#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
            Babylon::Plugins::ShaderCache::Enable();
#endif
        }
        else
        {
            // Subsequent Attach: reuse the existing Device, just rebind
            // the surface. Plugins, polyfills, and any loaded scripts
            // are preserved on the JS side.
            impl.m_device->UpdateWindow(nativeWindow);
            impl.m_device->UpdateSize(width, height);
        }

        std::unique_ptr<View> view{new View{std::make_unique<ViewImpl>(runtime)}};
        impl.m_currentView = view.get();

        // Open the first frame via ViewImpl::Resume (which flips
        // m_suspended → false). On first Attach, this must happen
        // BEFORE dispatching the engine-init lambda so the
        // Device::AddToJavaScript inside the lambda sees an open
        // frame to record into.
        view->m_impl->Resume();

        if (firstAttach)
        {
            RunFirstAttachInit(impl, nativeWindow);
        }

        return view;
    }

    View::View(std::unique_ptr<ViewImpl> impl)
        : m_impl{std::move(impl)}
    {
    }

    View::~View()
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;

        // End the in-flight frame if one is open. Idempotent: if the
        // Runtime was already suspended (which closed the frame via
        // ViewImpl::Suspend), this is a no-op. The Device persists on
        // the Runtime so the next Attach is cheap.
        m_impl->Suspend();

        impl.m_currentView = nullptr;
    }

    // ---------------------------------------------------------------------
    // ViewImpl::Suspend / Resume
    //
    // Idempotent open/close of the in-flight Device frame. Called from:
    //   - View::Attach            → Resume (open frame after Device setup)
    //   - View::~View             → Suspend (close frame at teardown)
    //   - Runtime::Suspend / Resume → matching call on the currently
    //                                 attached view, if any, so the host's
    //                                 OS-level pause/resume signal cleanly
    //                                 brackets the GPU frame.
    //
    // The internal `m_suspended` flag means "no frame currently open."
    // Initial state is `true`; the very first Resume opens the first
    // frame. Multiple Suspend or Resume calls in a row are no-ops.
    // ---------------------------------------------------------------------
    void ViewImpl::Suspend()
    {
        if (m_suspended)
        {
            return;
        }
        RuntimeImpl& impl = *m_runtime.m_impl;
        if (impl.m_device && impl.m_deviceUpdate)
        {
            impl.m_deviceUpdate->Finish();
            impl.m_device->FinishRenderingCurrentFrame();
        }
        m_suspended = true;
    }

    void ViewImpl::Resume()
    {
        if (!m_suspended)
        {
            return;
        }
        RuntimeImpl& impl = *m_runtime.m_impl;
        if (impl.m_device && impl.m_deviceUpdate)
        {
            impl.m_device->StartRenderingCurrentFrame();
            impl.m_deviceUpdate->Start();
        }
        m_suspended = false;
    }

    void View::RenderFrame()
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;

        // Skip the GPU work entirely while this view is suspended;
        // the host can keep calling RenderFrame() from its draw
        // callback unconditionally. The view's `m_suspended` flag is
        // flipped by Runtime::Suspend/Resume (and by ~View / Attach
        // for the destruction / construction boundaries), so this
        // check covers every "frame is not currently open" case
        // including: pre-Attach, between Suspend and Resume, and
        // during teardown.
        if (m_impl->m_suspended)
        {
            return;
        }

        // Babylon's JS render loop (requestAnimationFrame / scene.render)
        // runs between Start and Finish, scheduled via DeviceUpdate onto
        // the JS thread. This call doesn't enter JS directly —
        // DeviceUpdate handles the cross-thread coordination via
        // SafeTimespanGuarantor.
        impl.m_deviceUpdate->Finish();
        impl.m_device->FinishRenderingCurrentFrame();
        impl.m_device->StartRenderingCurrentFrame();
        impl.m_deviceUpdate->Start();
    }

    void View::Resize(uint32_t width, uint32_t height)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_device)
        {
            impl.m_device->UpdateSize(width, height);
        }
    }

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    void View::OnPointerDown(int32_t pointerId, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->TouchDown(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnPointerMove(int32_t pointerId, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->TouchMove(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnPointerUp(int32_t pointerId, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->TouchUp(static_cast<uint32_t>(pointerId),
                                   static_cast<int32_t>(lx),
                                   static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseDown(uint32_t buttonIndex, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->MouseDown(buttonIndex,
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseUp(uint32_t buttonIndex, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->MouseUp(buttonIndex,
                                   static_cast<int32_t>(lx),
                                   static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseMove(float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->MouseMove(static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseWheel(uint32_t wheelAxis, int32_t scrollValue)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            impl.m_input->MouseWheel(wheelAxis, scrollValue);
        }
    }

    uint32_t View::LeftMouseButton()   { return Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID; }
    uint32_t View::MiddleMouseButton() { return Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID; }
    uint32_t View::RightMouseButton()  { return Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID; }
    uint32_t View::MouseWheelY()       { return Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID; }
#endif
}
