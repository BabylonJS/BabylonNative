#include "AppRuntime.h"

#include <arcana/threading/cancellation.h>
#include <arcana/threading/dispatcher.h>

#include <cassert>
#include <optional>
#include <mutex>
#include <thread>
#include <type_traits>

namespace Babylon
{
    class AppRuntime::Impl
    {
    public:
        template<typename CallableT>
        void Append(CallableT callable)
        {
            if constexpr (std::is_copy_constructible<CallableT>::value)
            {
                m_dispatcher.queue([this, callable = std::move(callable)]() {
                    callable(m_env.value());
                });
            }
            else
            {
                m_dispatcher.queue([this, callablePtr = std::make_shared<CallableT>(std::move(callable))]() {
                    (*callablePtr)(m_env.value());
                });
            }
        }

        std::optional<Napi::Env> m_env{};
        std::optional<std::scoped_lock<std::mutex>> m_suspensionLock{};
        arcana::cancellation_source m_cancelSource{};
        arcana::manual_dispatcher<128> m_dispatcher{};
        std::thread m_thread;
    };

    AppRuntime::AppRuntime() :
        AppRuntime{{}}
    {
    }

    AppRuntime::AppRuntime(Options options)
        : m_options{std::move(options)}
        , m_impl{std::make_unique<Impl>()}
    {
        m_impl->m_thread = std::thread{[this] { RunPlatformTier(); }};

        Dispatch([this](Napi::Env env) {
            JsRuntime::CreateForJavaScript(env, [this](auto func) { Dispatch(std::move(func)); });
        });
    }

    AppRuntime::~AppRuntime()
    {
        if (m_impl->m_suspensionLock.has_value())
        {
            m_impl->m_suspensionLock.reset();
        }

        // Cancel immediately so pending work is dropped promptly, then append
        // a no-op work item to wake the worker thread from blocking_tick. The
        // no-op goes through push() which acquires the queue mutex, avoiding
        // the race where a bare notify_all() can be missed by wait().
        //
        // NOTE: This preserves the existing shutdown behavior where pending
        // callbacks are dropped on cancellation. A more complete solution
        // would add cooperative shutdown (e.g. NotifyDisposing/Rundown) so
        // consumers can finish cleanup work before the runtime is destroyed.
        m_impl->m_cancelSource.cancel();
        m_impl->Append([](Napi::Env) {});

        m_impl->m_thread.join();
    }

    void AppRuntime::Run(Napi::Env env)
    {
        m_impl->m_env = std::make_optional(env);

        m_impl->m_dispatcher.set_affinity(std::this_thread::get_id());

        while (!m_impl->m_cancelSource.cancelled())
        {
            m_impl->m_dispatcher.blocking_tick(m_impl->m_cancelSource);
        }

        // The dispatcher can be non-empty if something is dispatched after cancellation.
        m_impl->m_dispatcher.clear();
    }

    void AppRuntime::Suspend()
    {
        auto suspensionMutex = std::make_shared<std::mutex>();
        m_impl->m_suspensionLock.emplace(*suspensionMutex);
        m_impl->Append([suspensionMutex{std::move(suspensionMutex)}](Napi::Env) {
            std::scoped_lock lock{*suspensionMutex};
        });
    }

    void AppRuntime::Resume()
    {
        m_impl->m_suspensionLock.reset();
    }

    void AppRuntime::Dispatch(Dispatchable<void(Napi::Env)> func)
    {
        m_impl->Append([this, func{std::move(func)}](Napi::Env env) mutable {
            Execute([this, env, func{std::move(func)}]() mutable {
                try
                {
                    func(env);
                }
                catch (const Napi::Error& error)
                {
                    m_options.UnhandledExceptionHandler(error);
                }
                catch (...)
                {
                    assert(false);
                    std::abort();
                }
            });
        });
    }
}
