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

        // Reinterpret the platform-erased `void*` from ViewDescriptor as the
        // platform's Babylon::Graphics::WindowT. WindowT varies by
        // platform:
        //
        //   Win32        : HWND  (pointer)
        //   Android      : ANativeWindow*  (pointer)
        //   Apple        : CA::MetalLayer*  (pointer; metal-cpp wrapper)
        //   X11 (Linux)  : Window  (XID — `unsigned long`)
        //   UWP / WinRT  : winrt::Windows::Foundation::IInspectable
        //                  (a value type wrapping a refcounted COM pointer)
        //
        // For UWP we reconstruct the wrapper from the ABI pointer the
        // host stuffed in (typically via `winrt::get_abi(...)`); for
        // every other platform a single `reinterpret_cast` covers
        // pointer-to-pointer and void*-to-XID.
        Babylon::Graphics::WindowT ToWindowT(void* nativeWindow)
        {
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
            Babylon::Graphics::WindowT result{nullptr};
            if (nativeWindow != nullptr)
            {
                winrt::copy_from_abi(result, nativeWindow);
            }
            return result;
#else
            return reinterpret_cast<Babylon::Graphics::WindowT>(nativeWindow);
#endif
        }
    }

    // ---------------------------------------------------------------------
    // First-Attach engine initialization: dispatched onto the JS thread by
    // the first View::Attach call. Runs all plugin/polyfill Initialize()
    // calls in the same order as Apps/Playground/Shared/AppContext.cpp,
    // then flushes any LoadScript / Eval / RunOnJsThread calls the host
    // queued before the first Attach.
    //
    // After this lambda returns, m_initialized is true and subsequent
    // Runtime::LoadScript / Eval / RunOnJsThread calls dispatch directly.
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

            // 4. Flush pending LoadScript / Eval / RunOnJsThread calls
            //    in submission order. Holding m_pendingMutex across the
            //    iteration ensures any concurrent host-thread call lands
            //    *after* the queued ones in the JS thread queue (the
            //    host call blocks on the mutex until the iteration
            //    finishes appending each pending action to the
            //    ScriptLoader's task chain).
            std::lock_guard<std::mutex> lock{implPtr->m_pendingMutex};
            for (auto& action : implPtr->m_pending)
            {
                action();
            }
            implPtr->m_pending.clear();
            implPtr->m_initialized = true;
        });
    }

    // ---------------------------------------------------------------------
    // View::Attach (first time and subsequent)
    // ---------------------------------------------------------------------
    std::unique_ptr<View> View::Attach(Runtime& runtime, const ViewDescriptor& descriptor)
    {
        RuntimeImpl& impl = *runtime.m_impl;

        assert(impl.m_currentView == nullptr && "Only one View may be attached at a time.");
        if (impl.m_currentView != nullptr)
        {
            return nullptr;
        }

        const auto window = ToWindowT(descriptor.nativeWindow);

        if (!impl.m_device)
        {
            // First Attach on this Runtime: construct the Device and
            // open the first frame, then dispatch the engine-init lambda
            // onto the JS thread.
            Babylon::Graphics::Configuration config{};
            config.Window = window;
            config.Width = descriptor.width;
            config.Height = descriptor.height;
            config.MSAASamples = impl.m_options.msaaSamples;

            impl.m_device.emplace(config);
            impl.m_deviceUpdate.emplace(impl.m_device->GetUpdate("update"));

#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
            Babylon::Plugins::ShaderCache::Enable();
#endif

            // Open the first frame *before* dispatching the init lambda
            // so the Device::AddToJavaScript inside the lambda sees an
            // open frame to record into.
            impl.m_device->StartRenderingCurrentFrame();
            impl.m_deviceUpdate->Start();

            RunFirstAttachInit(impl, window);
        }
        else
        {
            // Subsequent Attach: reuse the existing Device, just rebind
            // the surface and re-enable rendering. Plugins, polyfills,
            // and any loaded scripts are preserved on the JS side.
            impl.m_device->UpdateWindow(window);
            impl.m_device->UpdateSize(descriptor.width, descriptor.height);
            impl.m_device->EnableRendering();
            impl.m_device->StartRenderingCurrentFrame();
            impl.m_deviceUpdate->Start();
        }

        std::unique_ptr<View> view{new View{std::make_unique<ViewImpl>(runtime)}};
        impl.m_currentView = view.get();
        return view;
    }

    View::View(std::unique_ptr<ViewImpl> impl)
        : m_impl{std::move(impl)}
    {
    }

    View::~View()
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;

        // Symmetric counterpart to Attach: close the in-flight frame and
        // disable rendering. The Device persists on the Runtime so the
        // next Attach is cheap.
        if (impl.m_device)
        {
            impl.m_deviceUpdate->Finish();
            impl.m_device->FinishRenderingCurrentFrame();
            impl.m_device->DisableRendering();
        }

        impl.m_currentView = nullptr;
    }

    void View::RenderFrame()
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;

        // Cheap suspended check (own mutex). Skip the GPU work entirely
        // while the runtime is suspended; the host can keep calling
        // RenderFrame() from its draw callback unconditionally.
        if (m_impl->m_runtime.IsSuspended())
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

    void View::Resize(uint32_t width, uint32_t height, float devicePixelRatio)
    {
        // devicePixelRatio is informational at the C++ layer for now —
        // Device computes its own DPR internally via GetDevicePixelRatio().
        // The parameter is part of the API for ViewDescriptor parity and so
        // future Device-level DPR plumbing has a place to land.
        (void)devicePixelRatio;

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
            impl.m_input->TouchDown(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(x),
                                     static_cast<int32_t>(y));
        }
    }

    void View::OnPointerMove(int32_t pointerId, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            impl.m_input->TouchMove(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(x),
                                     static_cast<int32_t>(y));
        }
    }

    void View::OnPointerUp(int32_t pointerId, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            impl.m_input->TouchUp(static_cast<uint32_t>(pointerId),
                                   static_cast<int32_t>(x),
                                   static_cast<int32_t>(y));
        }
    }

    void View::OnMouseDown(uint32_t buttonIndex, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            impl.m_input->MouseDown(buttonIndex,
                                     static_cast<int32_t>(x),
                                     static_cast<int32_t>(y));
        }
    }

    void View::OnMouseUp(uint32_t buttonIndex, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            impl.m_input->MouseUp(buttonIndex,
                                   static_cast<int32_t>(x),
                                   static_cast<int32_t>(y));
        }
    }

    void View::OnMouseMove(float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            impl.m_input->MouseMove(static_cast<int32_t>(x),
                                     static_cast<int32_t>(y));
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
