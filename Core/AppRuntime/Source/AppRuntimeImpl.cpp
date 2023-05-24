#include "AppRuntimeImpl.h"
#include <Babylon/JsRuntime.h>

namespace Babylon
{
    AppRuntimeImpl::AppRuntimeImpl(std::function<void(const std::exception&)> unhandledExceptionHandler)
        : m_unhandledExceptionHandler{std::move(unhandledExceptionHandler)}
        , m_thread{[this] { RunPlatformTier(); }}
    {
        Dispatch([this](Napi::Env env) {
            JsRuntime::CreateForJavaScript(env, [this](auto func) { Dispatch(std::move(func)); });
        });
    }

    AppRuntimeImpl::~AppRuntimeImpl()
    {
        if (m_suspensionLock.has_value())
        {
            m_suspensionLock.reset();
        }

        Dispatch([this](Napi::Env env) {
            // Notify the JsRuntime on the JavaScript thread that the JavaScript runtime shutdown sequence has
            // begun. The JsRuntimeScheduler will use this signal to gracefully cancel asynchronous operations.
            JsRuntime::NotifyDisposing(JsRuntime::GetFromJavaScript(env));

            // Cancel on the JavaScript thread to signal the Run function to gracefully end. It must be
            // dispatched and not canceled directly to ensure that existing work is executed and executed in
            // the correct order.
            m_cancellationSource.cancel();
        });

        m_thread.join();
    }

    void AppRuntimeImpl::Suspend()
    {
        auto suspensionMutex = std::make_shared<std::mutex>();
        m_suspensionLock.emplace(*suspensionMutex);
        Append([suspensionMutex = std::move(suspensionMutex)](Napi::Env) {
            std::scoped_lock lock{*suspensionMutex};
        });
    }

    void AppRuntimeImpl::Resume()
    {
        m_suspensionLock.reset();
    }

    void AppRuntimeImpl::Dispatch(Dispatchable<void(Napi::Env)> func)
    {
        Append([this, func{std::move(func)}](Napi::Env env) mutable {
            Execute([this, env, func{std::move(func)}]() mutable {
                try
                {
                    func(env);
                }
                catch (const std::exception& error)
                {
                    m_unhandledExceptionHandler(error);
                }
                catch (...)
                {
                    std::abort();
                }
            });
        });
    }

    void AppRuntimeImpl::Run(Napi::Env env)
    {
        m_env = std::make_optional(env);

        m_dispatcher.set_affinity(std::this_thread::get_id());

        while (!m_cancellationSource.cancelled())
        {
            m_dispatcher.blocking_tick(m_cancellationSource);
        }

        // The dispatcher can be non-empty if something is dispatched after cancellation.
        // For example, Chakra's JsSetPromiseContinuationCallback may potentially dispatch
        // a continuation after cancellation.
        m_dispatcher.clear();

        m_env.reset();
    }
}
