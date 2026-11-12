#include "AppRuntime.h"

#include "DelayedTaskScheduler.h"

#include <arcana/threading/cancellation.h>
#include <arcana/threading/dispatcher.h>

#include <cassert>
#include <optional>
#include <mutex>
#include <thread>
#include <type_traits>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

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

#ifdef __APPLE__
            WakeRunLoop();
#endif
        }

        void RunDispatchLoop();

#ifdef __APPLE__
        void WakeRunLoop()
        {
            std::scoped_lock lock{m_runLoopMutex};
            if (m_runLoopSource != nullptr)
            {
                CFRunLoopSourceSignal(m_runLoopSource);
                CFRunLoopWakeUp(m_runLoop);
            }
        }

        std::mutex m_runLoopMutex{};
        CFRunLoopRef m_runLoop{};
        CFRunLoopSourceRef m_runLoopSource{};
#endif

        std::optional<Napi::Env> m_env{};
        std::optional<std::scoped_lock<std::mutex>> m_suspensionLock{};
        arcana::cancellation_source m_cancelSource{};
        arcana::manual_dispatcher<128> m_dispatcher{};
        std::unique_ptr<Internal::DelayedTaskScheduler> m_delayedTaskScheduler{std::make_unique<Internal::DelayedTaskScheduler>()};
        bool m_delayedTaskSchedulerRegistered{};
        std::thread m_thread;
    };

#ifdef __APPLE__
    // JavaScriptCore schedules some engine work on the CFRunLoop of the thread that created its
    // VM, notably settling asynchronous WebAssembly compile/instantiate promises (its
    // DeferredWorkTimer). Run this thread's CFRunLoop so that work executes, and wake it through
    // a run loop source whenever work is dispatched.
    void AppRuntime::Impl::RunDispatchLoop()
    {
        CFRunLoopSourceContext context{};
        context.info = this;
        context.perform = [](void* info) {
            auto& impl = *static_cast<Impl*>(info);
            impl.m_dispatcher.tick(impl.m_cancelSource);
        };

        CFRunLoopRef runLoop = CFRunLoopGetCurrent();
        CFRunLoopSourceRef source = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &context);
        CFRunLoopAddSource(runLoop, source, kCFRunLoopDefaultMode);

        {
            std::scoped_lock lock{m_runLoopMutex};
            m_runLoop = runLoop;
            m_runLoopSource = source;
        }

        // Drain work queued before the source was published.
        CFRunLoopSourceSignal(source);

        while (!m_cancelSource.cancelled())
        {
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1.0e10, true);
        }

        {
            std::scoped_lock lock{m_runLoopMutex};
            m_runLoop = nullptr;
            m_runLoopSource = nullptr;
        }

        CFRunLoopSourceInvalidate(source);
        CFRelease(source);
    }
#else
    void AppRuntime::Impl::RunDispatchLoop()
    {
        while (!m_cancelSource.cancelled())
        {
            m_dispatcher.blocking_tick(m_cancelSource);
        }
    }
#endif

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

        m_impl->RunDispatchLoop();

        Napi::HandleScope scope{env};
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

    void AppRuntime::Dispatch(Dispatchable<void(Napi::Env)> func)
    {
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
