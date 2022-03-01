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
            m_dispatcher.queue([this, callable = std::move(callable)]() {
                callable(m_env.value());
            });
        }

        void Suspend();
        void Resume();
        void Run(Napi::Env);

    private:
        std::optional<Napi::Env> m_env{};

        std::optional<std::scoped_lock<std::mutex>> m_suspensionLock{};

        arcana::cancellation_source m_cancelSource{};
        arcana::manual_dispatcher<128> m_dispatcher{};

        std::thread m_thread;
    };
}
