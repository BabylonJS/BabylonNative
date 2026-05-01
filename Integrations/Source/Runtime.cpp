#include "RuntimeImpl.h"

#include <Babylon/DebugTrace.h>

#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
#include <Babylon/Plugins/ShaderCache.h>
#endif

#include <cassert>
#include <sstream>
#include <utility>

namespace Babylon::Integrations
{
    RuntimeImpl::RuntimeImpl(RuntimeOptions options)
        : m_options{std::move(options)}
    {
        // Wire DebugTrace through to the host's log callback (if any).
        // DebugTrace is process-wide so this affects any concurrent
        // Runtime instances; that matches AppContext's behavior today.
        if (m_options.log)
        {
            Babylon::DebugTrace::EnableDebugTrace(true);
            const auto& logCallback = m_options.log;
            Babylon::DebugTrace::SetTraceOutput([logCallback](const char* message) {
                logCallback(LogLevel::Log, message ? message : "");
            });
        }

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
        //   1. ScriptLoader's dispatcher captures &m_appRuntime, so
        //      ~ScriptLoader must run before ~AppRuntime.
        //   2. The Canvas polyfill and NativeInput pointer are referenced
        //      from JS-thread state; clear them before joining the JS
        //      thread, but only after ScriptLoader has drained.
        //   3. ~AppRuntime joins the JS thread. After it returns, no
        //      JS-thread task is running. If the first-Attach init
        //      lambda was queued but not yet run when ~Impl began, it
        //      will run during the AppRuntime drain (if AppRuntime
        //      drains its queue before joining); m_canvas / m_input
        //      etc. may then be re-populated and discarded when ~Impl
        //      itself destroys the optionals.
        //   4. ShaderCache::Disable() balances the Enable() that
        //      View::Attach calls on first attach.
        //   5. Device + DeviceUpdate destroyed last because the JS
        //      thread referenced them via Device::AddToJavaScript.
        //
        // m_initTcs is destroyed when this struct's members are
        // destroyed. If complete() was never called (no View ever
        // attached), the queued continuations are dropped without
        // firing, which is the desired behavior on shutdown.
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
