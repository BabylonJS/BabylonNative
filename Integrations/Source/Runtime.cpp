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
            // DebugTrace doesn't carry a level; treat it as Log.
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
        if (m_options.onUnhandledError)
        {
            const auto& userHandler = m_options.onUnhandledError;
            appRuntimeOptions.UnhandledExceptionHandler = [userHandler](const Napi::Error& error) {
                std::ostringstream ss{};
                ss << "[Uncaught Error] " << Napi::GetErrorString(error);
                userHandler(ss.str());
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

        // Discard any pending pre-init actions; we're tearing down.
        {
            std::lock_guard<std::mutex> lock{m_pendingMutex};
            m_pending.clear();
        }

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
        m_scriptLoader.reset();

#if BABYLON_NATIVE_POLYFILL_CANVAS
        m_canvas.reset();
#endif

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
        m_input = nullptr;
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
        std::string urlCopy{url};

        std::lock_guard<std::mutex> lock{m_impl->m_pendingMutex};
        if (!m_impl->m_initialized)
        {
            // Capture by value into a callable that the first-Attach
            // init lambda will invoke after plugin init completes.
            m_impl->m_pending.emplace_back(
                [scriptLoader = &*m_impl->m_scriptLoader, url = std::move(urlCopy)]() mutable {
                    scriptLoader->LoadScript(std::move(url));
                });
            return;
        }

        // Past init: dispatch directly. The scriptLoader serializes on
        // the JS thread.
        m_impl->m_scriptLoader->LoadScript(std::move(urlCopy));
    }

    void Runtime::Eval(std::string_view source, std::string_view sourceUrl)
    {
        std::string sourceCopy{source};
        std::string urlCopy{sourceUrl};

        std::lock_guard<std::mutex> lock{m_impl->m_pendingMutex};
        if (!m_impl->m_initialized)
        {
            m_impl->m_pending.emplace_back(
                [scriptLoader = &*m_impl->m_scriptLoader,
                 src = std::move(sourceCopy),
                 url = std::move(urlCopy)]() mutable {
                    scriptLoader->Eval(std::move(src), std::move(url));
                });
            return;
        }

        m_impl->m_scriptLoader->Eval(std::move(sourceCopy), std::move(urlCopy));
    }

    void Runtime::RunOnJsThread(std::function<void(Napi::Env)> callback)
    {
        if (!callback)
        {
            return;
        }

        std::lock_guard<std::mutex> lock{m_impl->m_pendingMutex};
        if (!m_impl->m_initialized)
        {
            // Defer through ScriptLoader so it stays serialized with
            // any LoadScript / Eval calls the host queued before us.
            m_impl->m_pending.emplace_back(
                [scriptLoader = &*m_impl->m_scriptLoader,
                 cb = std::move(callback)]() mutable {
                    scriptLoader->Dispatch(std::move(cb));
                });
            return;
        }

        m_impl->m_scriptLoader->Dispatch(std::move(callback));
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
}
