#include "RuntimeImpl.h"

#include <Babylon/DebugTrace.h>

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

#if BABYLON_NATIVE_POLYFILL_ABORTCONTROLLER
#include <Babylon/Polyfills/AbortController.h>
#endif

#if BABYLON_NATIVE_POLYFILL_SCHEDULING
#include <Babylon/Polyfills/Scheduling.h>
#endif

#if BABYLON_NATIVE_POLYFILL_URL
#include <Babylon/Polyfills/URL.h>
#endif

#if BABYLON_NATIVE_POLYFILL_WEBSOCKET
#include <Babylon/Polyfills/WebSocket.h>
#endif

#if BABYLON_NATIVE_POLYFILL_WINDOW
#include <Babylon/Polyfills/Window.h>
#endif

#include <cassert>
#include <fstream>
#include <sstream>
#include <utility>

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
    RuntimeImpl::RuntimeImpl(RuntimeOptions options)
        : m_options{std::move(options)}
    {
        // Wire DebugTrace through to the host's log callback (if any),
        // and enable it only when the host explicitly opts in.
        // DebugTrace is process-wide so this affects any concurrent
        // Runtime instances; that matches AppContext's behavior today.
        if (m_options.log)
        {
            const auto& logCallback = m_options.log;
            Babylon::DebugTrace::SetTraceOutput([logCallback](const char* message) {
                logCallback(LogLevel::Log, message ? message : "");
            });
        }
        Babylon::DebugTrace::EnableDebugTrace(m_options.enableDebugTrace);

        // Construct AppRuntime. This starts the JS thread and creates a
        // Napi::Env. Plugin Initialize() calls will be dispatched onto
        // this thread by the first View::Attach.
        Babylon::AppRuntime::Options appRuntimeOptions{};
        appRuntimeOptions.EnableDebugger = m_options.enableDebugger;
        appRuntimeOptions.WaitForDebugger = m_options.waitForDebugger;

        // Route uncaught JS exceptions through the host's log callback
        // with LogLevel::Fatal. If no log callback is set, leave the
        // AppRuntime default in place (writes to program output).
        if (m_options.log)
        {
            const auto& logCallback = m_options.log;
            appRuntimeOptions.UnhandledExceptionHandler = [logCallback](const Napi::Error& error) {
                std::ostringstream ss{};
                ss << "[Uncaught Error] " << Napi::GetErrorString(error);
                logCallback(LogLevel::Fatal, ss.str());
            };
        }

        m_appRuntime.emplace(std::move(appRuntimeOptions));

        // ScriptLoader serializes LoadScript / Eval / Dispatch onto the
        // AppRuntime's JS thread. Its dispatcher captures a reference to
        // m_appRuntime, so ~ScriptLoader must complete before ~AppRuntime.
        m_scriptLoader.emplace(*m_appRuntime);
    }

    RuntimeImpl::~RuntimeImpl()
    {
        // Precondition: no View is currently attached. The host owns the
        // ordering: destroy Views before destroying their Runtime.
        assert(m_currentView == nullptr && "View must be destroyed before its Runtime.");

        // Order matters here:
        //   1. Persist the shader cache. The View precondition above
        //      guarantees `ViewImpl::Suspend()` already ran via
        //      `~View`, so the engine is quiescent and a host-thread
        //      Save is race-free.
        //   2. ScriptLoader's dispatcher captures &m_appRuntime, so
        //      ~ScriptLoader must run before ~AppRuntime.
        //   3. The Canvas polyfill and NativeInput pointer are referenced
        //      from JS-thread state; clear them before joining the JS
        //      thread, but only after ScriptLoader has drained.
        //   4. ~AppRuntime joins the JS thread.
        //   5. ShaderCache::Disable() balances the Enable() that
        //      RunFirstAttachInit calls on first attach.
        //   6. Device + DeviceUpdate destroyed last because the JS
        //      thread referenced them via Device::AddToJavaScript.
        //
        // m_initTcs is destroyed when this struct's members are
        // destroyed. If complete() was never called (no View ever
        // attached), the queued continuations are dropped without
        // firing, which is the desired behavior on shutdown.
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
        SaveShaderCache();
#endif

        m_scriptLoader.reset();

#if BABYLON_NATIVE_POLYFILL_CANVAS
        m_canvas.reset();
#endif

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
        m_input = nullptr;
#endif

#if BABYLON_NATIVE_PLUGIN_NATIVEXR
        // NativeXr holds JS-thread-bound resources and a strong ref to
        // the Napi::Env it was initialized with. Destroy it before the
        // AppRuntime joins the JS thread; same reason as ScriptLoader.
        m_nativeXr.reset();
#endif

        m_appRuntime.reset();

#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
        if (m_device)
        {
            Babylon::Plugins::ShaderCache::Disable();
        }
#endif

        m_deviceUpdate.reset();
        m_device.reset();
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
    void RuntimeImpl::RunFirstAttachInit(Babylon::Graphics::WindowT window)
    {
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
        // Enable the process-wide shader cache singleton and hydrate
        // it from disk before any JS-thread shader compilation can
        // begin. Both calls are host-thread safe (the singleton is
        // not yet referenced by NativeEngine, which gets initialized
        // on the JS thread below).
        Babylon::Plugins::ShaderCache::Enable();
        LoadShaderCache();
#endif

        m_appRuntime->Dispatch([implPtr = this, window](Napi::Env env) {
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

#if BABYLON_NATIVE_POLYFILL_ABORTCONTROLLER
            Babylon::Polyfills::AbortController::Initialize(env);
#endif
#if BABYLON_NATIVE_POLYFILL_SCHEDULING
            Babylon::Polyfills::Scheduling::Initialize(env);
#endif
#if BABYLON_NATIVE_POLYFILL_URL
            Babylon::Polyfills::URL::Initialize(env);
#endif
#if BABYLON_NATIVE_POLYFILL_WEBSOCKET
            Babylon::Polyfills::WebSocket::Initialize(env);
#endif

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
    // Persistent shader cache (no-ops when `shaderCachePath` is empty).
    // ---------------------------------------------------------------------
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
    void RuntimeImpl::LoadShaderCache()
    {
        if (m_options.shaderCachePath.empty() || !Babylon::Plugins::ShaderCache::IsEnabled())
        {
            return;
        }
        std::ifstream stream{m_options.shaderCachePath, std::ios::binary};
        if (stream.good())
        {
            Babylon::Plugins::ShaderCache::Load(stream);
        }
        // Missing or unreadable file is fine — we just start with an empty cache.
    }

    void RuntimeImpl::SaveShaderCache()
    {
        if (m_options.shaderCachePath.empty() || !Babylon::Plugins::ShaderCache::IsEnabled())
        {
            return;
        }
        std::ofstream stream{m_options.shaderCachePath, std::ios::binary};
        if (stream.good())
        {
            Babylon::Plugins::ShaderCache::Save(stream);
        }
    }
#endif

    std::unique_ptr<Runtime> Runtime::Create(RuntimeOptions options)
    {
        // Private ctor + manual unique_ptr because make_unique can't see it.
        std::unique_ptr<Runtime> runtime{new Runtime()};
        runtime->m_impl = std::make_unique<RuntimeImpl>(std::move(options));
        return runtime;
    }

    Runtime::Runtime() = default;
    Runtime::~Runtime() = default;

    void Runtime::LoadScript(std::string_view url)
    {
        m_impl->m_initTcs.as_task().then(arcana::inline_scheduler, arcana::cancellation::none(),
            [scriptLoader = &*m_impl->m_scriptLoader, url = std::string{url}]() mutable {
                scriptLoader->LoadScript(std::move(url));
            });
    }

    void Runtime::Eval(std::string_view source, std::string_view sourceUrl)
    {
        m_impl->m_initTcs.as_task().then(arcana::inline_scheduler, arcana::cancellation::none(),
            [scriptLoader = &*m_impl->m_scriptLoader,
             source = std::string{source},
             url = std::string{sourceUrl}]() mutable {
                scriptLoader->Eval(std::move(source), std::move(url));
            });
    }

    void Runtime::RunOnJsThread(std::function<void(Napi::Env)> callback)
    {
        if (!callback)
        {
            return;
        }

        m_impl->m_initTcs.as_task().then(arcana::inline_scheduler, arcana::cancellation::none(),
            [scriptLoader = &*m_impl->m_scriptLoader, cb = std::move(callback)]() mutable {
                scriptLoader->Dispatch(std::move(cb));
            });
    }

    void Runtime::Suspend()
    {
        std::lock_guard<std::mutex> lock{m_impl->m_suspendMutex};
        if (m_impl->m_suspendCount++ == 0)
        {
            // Close the in-flight frame on the currently attached View
            // (if any) BEFORE blocking the JS thread. This keeps the
            // GPU side clean across the suspension: no held drawable,
            // no open DeviceUpdate safe-timespan. The View tracks its
            // own state, so this composes with the subsequent ~View
            // (which Suspends again — no-op) and with hosts that
            // suspend the Runtime while no View is attached.
            if (m_impl->m_currentView)
            {
                m_impl->m_currentView->m_impl->Suspend();
            }
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
            // Persist the shader cache while the engine is known
            // quiescent: the view's Suspend just closed the current
            // frame and locked the update safe-timespan, so no engine
            // shader compilation can be in flight.
            m_impl->SaveShaderCache();
#endif
            m_impl->m_appRuntime->Suspend();
        }
    }

    void Runtime::Resume()
    {
        std::lock_guard<std::mutex> lock{m_impl->m_suspendMutex};
        if (m_impl->m_suspendCount == 0)
        {
            // Mismatched Resume; ignore rather than underflow the count.
            return;
        }
        if (--m_impl->m_suspendCount == 0)
        {
            m_impl->m_appRuntime->Resume();
            // Re-open the frame on the attached View (if any) so the
            // next RenderFrame call has something to Finish.
            if (m_impl->m_currentView)
            {
                m_impl->m_currentView->m_impl->Resume();
            }
        }
    }

    bool Runtime::IsSuspended() const
    {
        std::lock_guard<std::mutex> lock{m_impl->m_suspendMutex};
        return m_impl->m_suspendCount > 0;
    }

#if BABYLON_NATIVE_PLUGIN_NATIVEXR
    void Runtime::SetXrWindow(void* nativeWindow)
    {
        std::lock_guard<std::mutex> lock{m_impl->m_xrMutex};
        m_impl->m_xrWindow = nativeWindow;
        if (m_impl->m_nativeXr)
        {
            m_impl->m_nativeXr->UpdateWindow(nativeWindow);
        }
        // If NativeXr isn't initialized yet (no View::Attach has
        // happened), the value is stashed in m_xrWindow and applied
        // by the first-Attach init lambda when it constructs NativeXr.
    }

    bool Runtime::IsXrActive() const
    {
        return m_impl->m_isXrActive.load(std::memory_order_relaxed);
    }
#endif
}
