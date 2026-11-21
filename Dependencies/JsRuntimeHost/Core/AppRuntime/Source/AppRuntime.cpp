#include "AppRuntime.h"

#include "DelayedTaskScheduler.h"

#include <arcana/threading/cancellation.h>
#include <arcana/threading/dispatcher.h>

#include <cassert>
#include <atomic>
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

            std::scoped_lock lock{m_dispatchLoopWakerMutex};
            if (m_dispatchLoopWaker)
            {
                m_dispatchLoopWaker();
            }
        }

        std::mutex m_dispatchLoopWakerMutex{};
        std::function<void()> m_dispatchLoopWaker{};

        std::optional<Napi::Env> m_env{};
        std::shared_ptr<JsRuntime::InternalState> m_jsRuntimeState{};
        std::optional<std::scoped_lock<std::mutex>> m_suspensionLock{};
        arcana::cancellation_source m_cancelSource{};
        arcana::manual_dispatcher<128> m_dispatcher{};
        std::unique_ptr<Internal::DelayedTaskScheduler> m_delayedTaskScheduler{std::make_unique<Internal::DelayedTaskScheduler>()};
        bool m_delayedTaskSchedulerRegistered{};
        std::thread m_thread;
        std::atomic_bool m_terminationRequested{false};
        std::atomic_bool m_executionTerminationRequested{false};
    };

    void AppRuntime::RunDispatcher()
    {
        while (!m_impl->m_cancelSource.cancelled())
        {
            m_impl->m_dispatcher.blocking_tick(m_impl->m_cancelSource);
        }
    }

    void AppRuntime::TickDispatcher()
    {
        m_impl->m_dispatcher.tick(m_impl->m_cancelSource);
    }

    bool AppRuntime::IsDispatchLoopCancelled() const
    {
        return m_impl->m_cancelSource.cancelled();
    }

    void AppRuntime::SetDispatchLoopWaker(std::function<void()> waker)
    {
        std::scoped_lock lock{m_impl->m_dispatchLoopWakerMutex};
        m_impl->m_dispatchLoopWaker = std::move(waker);
    }

    AppRuntime::AppRuntime() :
        AppRuntime{{}}
    {
    }

    AppRuntime::AppRuntime(Options options)
        : m_options{std::move(options)}
        , m_impl{std::make_unique<Impl>()}
    {
        m_impl->m_thread = std::thread{[this] {
            RunPlatformTier();
            if (m_options.ThreadExitHandler)
            {
                m_options.ThreadExitHandler();
            }
        }};

        Dispatch([this](Napi::Env env) {
            m_impl->m_jsRuntimeState = JsRuntime::CreateForJavaScript(env, [this](auto func) { Dispatch(std::move(func)); }).m_state;
            Internal::DelayedTaskScheduler::SetForJavaScript(env, GetDelayedTaskScheduler());
            m_impl->m_delayedTaskSchedulerRegistered = true;
        });
    }

    AppRuntime::~AppRuntime()
    {
        if (m_impl->m_suspensionLock.has_value())
        {
            m_impl->m_suspensionLock.reset();
        }

        Terminate();

        m_impl->m_thread.join();
    }

    void AppRuntime::Run(Napi::Env env)
    {
        m_impl->m_env = std::make_optional(env);

        m_impl->m_dispatcher.set_affinity(std::this_thread::get_id());

        RunDispatchLoop();

        Napi::HandleScope scope{env};

        // Stop native completions before discarding work, while captures can still
        // safely release environment-owned values. Do not rely on JS finalizer order.
        JsRuntime::Close(m_impl->m_jsRuntimeState);

        ShutdownEnvironment(env);

        if (m_impl->m_delayedTaskSchedulerRegistered)
        {
            Internal::DelayedTaskScheduler::ClearFromJavaScript(env);
            m_impl->m_delayedTaskSchedulerRegistered = false;
        }
        GetDelayedTaskScheduler().Shutdown();

        // The dispatcher can be non-empty if something is dispatched after cancellation.
        m_impl->m_dispatcher.clear();
    }

    Internal::DelayedTaskScheduler& AppRuntime::GetDelayedTaskScheduler()
    {
        return *m_impl->m_delayedTaskScheduler;
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

    void AppRuntime::Terminate()
    {
        m_impl->m_executionTerminationRequested.store(true);
        Close();
    }

    void AppRuntime::Close()
    {
        if (m_impl->m_terminationRequested.exchange(true))
        {
            return;
        }

        m_impl->m_cancelSource.cancel();

        // Queueing under the dispatcher's mutex makes the wake-up immune to
        // the missed-notification race covered by DestroyDoesNotDeadlock.
        // The cancelled run loop drops this no-op rather than executing it.
        m_impl->Append([](Napi::Env) {});
    }

    bool AppRuntime::IsTerminationRequested() const noexcept
    {
        return m_impl->m_terminationRequested.load();
    }

    bool AppRuntime::IsExecutionTerminationRequested() const noexcept
    {
        return m_impl->m_executionTerminationRequested.load();
    }

    void AppRuntime::Dispatch(Dispatchable<void(Napi::Env)> func)
    {
        if (IsTerminationRequested())
        {
            return;
        }

        m_impl->Append([this, func{std::move(func)}](Napi::Env env) mutable {
            Execute([this, env, func{std::move(func)}]() mutable {
                // Some engines (notably Hermes) require an open NAPI handle
                // scope before any napi_* call that materializes a value.
                // The other engines (V8/Chakra/JSC) already provide an outer
                // scope at the RunEnvironmentTier level, so this extra
                // scope is harmless there but mandatory for Hermes.
                Napi::HandleScope scope{env};

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

                // Drain engine-level microtasks/jobs queued during the
                // callback (Promise continuations, queueMicrotask, etc.) so
                // they run before the next top-level Dispatch.  No-op for
                // engines that drain automatically; Hermes needs an explicit
                // pump.
                DrainMicrotasks(env);
            });
        });
    }
}
