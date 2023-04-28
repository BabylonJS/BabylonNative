#pragma once

#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>
#include <napi/env.h>

#include <future>
#include <functional>
#include <exception>

namespace Babylon
{
    class WorkQueue
    {
    public:
        WorkQueue(std::function<void()> threadProcedure);
        ~WorkQueue();

        template<typename CallableT>
        void Append(CallableT callable)
        {
            // Manual dispatcher queueing requires a copyable CallableT, we use a shared pointer trick to make a
            // copyable callable if necessary.
            if constexpr (std::is_copy_constructible<CallableT>::value)
            {
                if (m_cancellationSource.cancelled())
                {
                    m_shutdownQueue.push([callable = std::move(callable)] {});
                }
                else
                {
                    m_dispatcher.queue([this, callable = std::move(callable)]() {
                        callable(m_env.value());
                    });
                }
            }
            else
            {
                if (m_cancellationSource.cancelled())
                {
                    m_shutdownQueue.push([callablePtr = std::make_shared<CallableT>(std::move(callable))] {});
                }
                else
                {
                    m_dispatcher.queue([this, callablePtr = std::make_shared<CallableT>(std::move(callable))]() {
                        (*callablePtr)(m_env.value());
                    });
                }
            }
        }

        void Suspend();
        void Resume();
        void Run(Napi::Env);

    private:
        std::optional<Napi::Env> m_env{};

        std::optional<std::scoped_lock<std::mutex>> m_suspensionLock{};

        arcana::cancellation_source m_cancellationSource{};

        using DispatcherT = arcana::manual_dispatcher<128>;
        DispatcherT m_dispatcher{};

        // Put the callables in a separate queue during shutdown to ensure the callables are destroyed on the right thread.
        arcana::blocking_concurrent_queue<DispatcherT::callback_t> m_shutdownQueue{};

        std::thread m_thread{};
    };
}
