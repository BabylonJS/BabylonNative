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
        // Forward Babylon Console levels to the public LogLevel enum so
        // consumers don't need to depend on the Console polyfill header.
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
        // Forward DebugTrace through the host log callback (Verbose level
        // for easy filtering). DebugTrace is process-wide.
        if (m_options.log)
        {
            const auto& logCallback = m_options.log;
            Babylon::DebugTrace::SetTraceOutput([logCallback](const char* message) {
                logCallback(LogLevel::Verbose, message ? message : "");
            });
        }
        Babylon::DebugTrace::EnableDebugTrace(m_options.enableDebugTrace);

        // Construct AppRuntime: starts the JS thread and creates a Napi::Env.
        // Plugin Initialize() calls are dispatched onto this thread by the
        // first View attach.
        Babylon::AppRuntime::Options appRuntimeOptions{};
        appRuntimeOptions.EnableDebugger = m_options.enableDebugger;
        appRuntimeOptions.WaitForDebugger = m_options.waitForDebugger;

        // Route uncaught JS exceptions to the host log callback as Fatal.
        // If no callback is set, the AppRuntime default writes to program output.
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

        // ScriptLoader's dispatcher captures &m_appRuntime, so ~ScriptLoader
        // must run before ~AppRuntime.
        m_scriptLoader.emplace(*m_appRuntime);
    }

    RuntimeImpl::~RuntimeImpl()
    {
        // Host owns the ordering: destroy Views before their Runtime.
        assert(m_currentView == nullptr && "View must be destroyed before its Runtime.");

        // Teardown order:
        //   1. SaveShaderCache: ~ViewImpl already ran ViewImpl::Suspend, so
        //      the engine is quiescent and a host-thread Save is race-free.
        //   2. ~ScriptLoader before ~AppRuntime (dispatcher captures it).
        //   3. Canvas / NativeInput / NativeXr hold JS-thread-bound state;
        //      drop them before joining the JS thread.
        //   4. ~AppRuntime joins the JS thread.
        //   5. ShaderCache::Disable balances first-attach Enable.
        //   6. Device + DeviceUpdate last (JS thread referenced them).
        //
        // m_initTcs: if complete() was never called (no View ever attached),
        // queued continuations are dropped on destruction, which is correct.
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

    // Dispatched onto the JS thread by the first View attach. Initializes
    // all plugins/polyfills in the canonical Babylon Native order, then
    // completes m_initTcs to unblock host calls that were queued before the
    // first attach. Post-init, those host calls fire their continuation
    // synchronously via inline_scheduler and submit straight to ScriptLoader.
    void RuntimeImpl::RunFirstAttachInit(Babylon::Graphics::WindowT window)
    {
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
        // Enable + hydrate before any JS-thread shader compilation. Both
        // calls are host-thread safe since NativeEngine hasn't been
        // initialized yet.
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
                    [userLog, env](const char* message, Babylon::Polyfills::Console::LogLevel level) {
                        if (!userLog || !message)
                        {
                            return;
                        }

                        // Promote console.error to "message + JS callstack" so
                        // hosts get a usable diagnostic for recoverable JS
                        // errors (Babylon.js routes most of its non-fatal
                        // failures through console.error). Stack capture is
                        // best-effort: engines without stack support return
                        // empty, and we omit the trailer in that case.
                        if (level == Babylon::Polyfills::Console::LogLevel::Error)
                        {
                            const auto jsStack = Babylon::Polyfills::Console::CaptureCurrentJsStack(env);
                            if (!jsStack.empty())
                            {
                                std::string combined{message};
                                combined.append("\nJS callstack:\n");
                                combined.append(jsStack);
                                userLog(LogLevel::Error, combined);
                                return;
                            }
                        }

                        userLog(ToIntegrationsLogLevel(level), message);
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
            // Initialize NativeXr, apply any pending xr window from a prior
            // SetXrWindow, and wire the session-state callback.
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

            // 4. Fire any host calls queued before the first View attach.
            implPtr->m_initTcs.complete();
        });
    }

    // Persistent shader cache (no-ops when shaderCachePath is empty).
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
        // Missing/unreadable file: start with an empty cache.
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

    Runtime::Runtime(RuntimeOptions options)
        : m_impl{std::make_unique<RuntimeImpl>(std::move(options))}
    {
    }

    Runtime::~Runtime() = default;
    Runtime::Runtime(Runtime&&) noexcept = default;
    Runtime& Runtime::operator=(Runtime&&) noexcept = default;

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

    void Runtime::RunOnJsThread(std::function<void(Napi::Env)> callback, bool afterScriptLoad)
    {
        if (!callback)
        {
            return;
        }

        m_impl->m_initTcs.as_task().then(arcana::inline_scheduler, arcana::cancellation::none(),
            [appRuntime = &*m_impl->m_appRuntime,
             scriptLoader = &*m_impl->m_scriptLoader,
             cb = std::move(callback),
             afterScriptLoad]() mutable {
                if (afterScriptLoad)
                {
                    scriptLoader->Dispatch(std::move(cb));
                }
                else
                {
                    appRuntime->Dispatch(std::move(cb));
                }
            });
    }

    void Runtime::Suspend()
    {
        // Frame-thread only (see Runtime.h). The atomic is for cross-thread
        // IsSuspended() reads, not for protecting the count.
        if (m_impl->m_suspendCount.fetch_add(1, std::memory_order_relaxed) == 0)
        {
            // Close the in-flight frame on the attached View (if any) BEFORE
            // blocking the JS thread: keeps the GPU side clean (no held
            // drawable, no open DeviceUpdate safe-timespan). Composes
            // correctly with ~ViewImpl (re-Suspend is a no-op) and with
            // suspending while no View is attached.
            if (m_impl->m_currentView)
            {
                m_impl->m_currentView->Suspend();
            }
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
            // Engine is quiescent here (ViewImpl::Suspend just closed the
            // frame and locked the update safe-timespan), so the save is
            // race-free with any pending shader compilation.
            m_impl->SaveShaderCache();
#endif
            m_impl->m_appRuntime->Suspend();
        }
    }

    void Runtime::Resume()
    {
        // Frame-thread only (see Runtime.h).
        if (m_impl->m_suspendCount.load(std::memory_order_relaxed) == 0)
        {
            // Mismatched Resume — ignore rather than underflow.
            return;
        }
        if (m_impl->m_suspendCount.fetch_sub(1, std::memory_order_relaxed) == 1)
        {
            m_impl->m_appRuntime->Resume();
            // Re-open the frame on the attached View. On a view that was
            // attached but never sized, this also drives the deferred
            // first-Resize init via InitializeIfReady.
            if (m_impl->m_currentView)
            {
                m_impl->m_currentView->Resume();
            }
        }
    }

    bool Runtime::IsSuspended() const
    {
        return m_impl->m_suspendCount.load(std::memory_order_relaxed) > 0;
    }

#if BABYLON_NATIVE_PLUGIN_NATIVEXR
    void Runtime::SetXrWindow(void* nativeWindow)
    {
        std::lock_guard<std::mutex> lock{m_impl->m_xrMutex};
        m_impl->m_xrWindow = nativeWindow;
        if (m_impl->m_nativeXr)
        {
            // NativeXr's entry points are JS-thread-only; dispatch the
            // actual UpdateWindow call instead of touching it from
            // whatever thread invoked SetXrWindow.
            m_impl->m_appRuntime->Dispatch([implPtr = m_impl.get(), nativeWindow](Napi::Env) {
                if (implPtr->m_nativeXr)
                {
                    implPtr->m_nativeXr->UpdateWindow(nativeWindow);
                }
            });
        }
        // If NativeXr isn't initialized yet (no View attach has happened),
        // the value is applied by the first-attach init lambda.
    }

    bool Runtime::IsXrActive() const
    {
        return m_impl->m_isXrActive.load(std::memory_order_relaxed);
    }
#endif
}
