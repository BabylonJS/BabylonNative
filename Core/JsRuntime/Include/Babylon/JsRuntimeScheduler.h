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
    // handling for when the JsRuntime begins shutting down, i.e., when JsRuntime::NotifyDisposing is called:
    //   1. The destructor blocks if there are outstanding schedulers not yet invoked on the JavaScript thread.
    //   2. Once the JsRuntime begins shutting down, all schedulers will reroute its dispatch calls from the
    //      JsRuntime to a separate dispatcher owned by the JsRuntimeScheduler itself.  This class will then be able
    //      to pump this dispatcher in its destructor to prevent deadlocks.
    //
    // The typical pattern for an arcana task will look something like this:
    //
    //   class MyClass
    //   {
    //   public:
    //       void MyFunction(const Napi::CallbackInfo& info);
    //
    //   private:
    //       arcana::cancellation_source m_cancellationSource;
    // 
    //       // Put this last so that it gets destructed first.
    //       JsRuntimeScheduler m_runtimeScheduler;
    //   };
    //
    //   void MyClass::MyFunction(const Napi::CallbackInfo& info)
    //   {
    //       const auto callback{info[0].As<Napi::Function>()};
    //
    //       arcana::make_task(arcana::threadpool_scheduler, m_cancellationSource, []()
    //       {
    //           // do some asynchronous work
    //       }).then(m_runtimeScheduler.Get(), m_cancelSource, [thisRef = Napi::Persistent(info.This()), callback = Napi::Persistent(callback)]() {
    //       {
    //           callback.Call({});
    //       });
    //    }
    //
    // **IMPORTANT**:
    //   1. To prevent continuations from accessing destructed objects, declare the JsRuntimeScheduler at the end of
    //      the N-API class.  The destructor of the JsRuntimeScheduler will call `Rundown()` which will block until
    //      all of its schedulers are invoked.  If this is not possible, call `Rundown()` manually in the destructor
    //      of the N-API class.
    //   2. The last continuation that accesses members of the N-API object, including the cancellation associated with
    //      the continuation must capture a persistent reference to the N-API object itself to prevent GC from collecting
    //      the N-API object during the asynchronous operation.
    class JsRuntimeScheduler
    {
    public:
        explicit JsRuntimeScheduler(JsRuntime& runtime)
            : m_runtime{&runtime}
            , m_scheduler{*this}
        {
            JsRuntime::RegisterDisposing(*m_runtime, [this]()
            {
                m_runtime = nullptr;
            });
        }

        ~JsRuntimeScheduler()
        {
            Rundown();
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

        JsRuntime* m_runtime;
        SchedulerImpl m_scheduler;
        std::atomic<int> m_count{0};
        arcana::manual_dispatcher<128> m_disposingDispatcher{};
    };
}
