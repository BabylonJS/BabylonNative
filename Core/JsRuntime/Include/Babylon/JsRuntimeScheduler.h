#pragma once

#include "JsRuntime.h"
#include <arcana/threading/dispatcher.h>
#include <atomic>
#include <cassert>

namespace Babylon
{
    // This class encapsulates a coding pattern for invoking continuations on the JavaScript thread while properly
    // handling garbage collection and shutdown scenarios.  This class provides and manages the schedulers intended
    // for a N-API object to use with arcana tasks.  It is different than the typical scheduler as this class itself
    // is not a scheduler directly, but instead hands out scheduler via its `Get()` function.  It provides special
    // handling for when the JsRuntime begins shutting down, i.e., when JsRuntime::NotifyDisposing is called.
    //   1. Calling `Rundown` blocks execution until all outstanding schedulers are invoked on the JavaScript thread.
    //   2. Once the JsRuntime begins shutting down, all schedulers will reroute its dispatch calls from the
    //      JsRuntime to a separate dispatcher owned by the JsRuntimeScheduler itself.  This class will then be able
    //      to pump this dispatcher in its destructor to prevent deadlocks.
    //
    // The typical pattern for an arcana task will look something like this:
    //   class MyClass
    //   {
    //   public:
    //       ~MyClass()
    //       {
    //           m_cancellationSource.cancel();
    //
    //           // Wait for asynchronous operations to complete.
    //           m_runtimeScheduler.Rundown();
    //       }
    // 
    //       void MyFunction(const Napi::CallbackInfo& info)
    //       {
    //           const auto callback{info[0].As<Napi::Function>()};
    //           
    //           arcana::make_task(arcana::threadpool_scheduler, m_cancellationSource, []() {
    //               // do some asynchronous work
    //           }).then(m_runtimeScheduler.Get(), m_cancellationSource, [thisRef = Napi::Persistent(info.This()), callback = Napi::Persistent(callback)]() {
    //               callback.Call({});
    //           });
    //       }
    //
    //   private:
    //       arcana::cancellation_source m_cancellationSource;
    //       JsRuntimeScheduler m_runtimeScheduler;
    //   };
    //
    // **IMPORTANT**:
    //   1. To prevent continuations from accessing freed memory, the destructor of the N-API class is expected to call
    //      `Rundown()` which blocks execution until all of its schedulers are invoked. Failing to do so will result in
    //      an assert if there are outstanding schedulers not yet invoked.
    //   2. The last continuation that accesses members of the N-API object, including the cancellation associated with
    //      the continuation, must capture a persistent reference to the N-API object itself to prevent the GC from
    //      collecting the N-API object during the asynchronous operation. Failing to do so will result in a hang
    //      when `Rundown()` is called in the N-API class destructor.
    class JsRuntimeScheduler : public JsRuntime::IDisposingCallback
    {
    public:
        explicit JsRuntimeScheduler(JsRuntime& runtime)
            : m_runtime{&runtime}
            , m_scheduler{*this}
        {
            std::scoped_lock lock{m_mutex};
            JsRuntime::RegisterDisposing(*m_runtime, this);
        }

        ~JsRuntimeScheduler()
        {
            assert(m_count == 0 && "Schedulers for the JavaScript thread are not yet invoked");

            std::scoped_lock lock{m_mutex};
            if (m_runtime != nullptr)
            {
                JsRuntime::UnregisterDisposing(*m_runtime, this);
            }
        }

        // Wait until all of the schedulers are invoked.
        void Rundown()
        {
            while (m_count > 0)
            {
                m_disposingDispatcher.blocking_tick(arcana::cancellation::none());
            }
        }

        // Get a scheduler to invoke continuations on the JavaScript thread.
        const auto& Get()
        {
            ++m_count;
            return m_scheduler;
        }

    private:
        void Disposing() override
        {
            std::scoped_lock lock{m_mutex};
            m_runtime = nullptr;
        }

        class SchedulerImpl
        {
        public:
            explicit SchedulerImpl(JsRuntimeScheduler& parent) : m_parent{parent}
            {
            }

            template<typename CallableT>
            void operator()(CallableT&& callable) const
            {
                m_parent.Dispatch(callable);
            }

        private:
            JsRuntimeScheduler& m_parent;
        };

        template<typename CallableT>
        void Dispatch(CallableT&& callable)
        {
            std::scoped_lock lock{m_mutex};

            if (m_runtime != nullptr)
            {
                m_runtime->Dispatch([callable{std::forward<CallableT>(callable)}](Napi::Env)
                {
                    callable();
                });
            }
            else
            {
                m_disposingDispatcher(callable);
            }

            --m_count;
        }

        mutable std::mutex m_mutex{};
        JsRuntime* m_runtime{};
        std::function<void()> m_disposingCallback{};

        SchedulerImpl m_scheduler;
        std::atomic<int> m_count{0};
        arcana::manual_dispatcher<128> m_disposingDispatcher{};
    };
}
