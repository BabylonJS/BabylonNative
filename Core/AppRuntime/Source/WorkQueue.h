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

        void Suspend();
        void Resume();
        void Run(Napi::Env);

    private:
        std::optional<Napi::Env> m_env{};
        std::optional<std::scoped_lock<std::mutex>> m_suspensionLock{};
        arcana::cancellation_source m_cancellationSource{};
        arcana::manual_dispatcher<128> m_dispatcher{};
        std::thread m_thread{};
    };
}
