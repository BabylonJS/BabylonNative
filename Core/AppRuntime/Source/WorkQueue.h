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
        WorkQueue(std::function<void()> threadProcedure, std::function<void(std::exception_ptr)> unhandledExceptionHandler);
        ~WorkQueue();

        template<typename CallableT>
        void Append(CallableT callable)
        {
            std::scoped_lock lock{m_appendMutex};
            m_task = m_task.then(m_dispatcher, m_cancelSource, [this, callable = std::move(callable)]() mutable noexcept {
                try
                {
                    callable(m_env.value());
                }
                catch (...)
                {
                    m_unhandledExceptionHandler(std::current_exception());
                }
            });
        }

        void Suspend();
        void Resume();
        void Run(Napi::Env);

    private:
        std::optional<Napi::Env> m_env{};

        std::mutex m_appendMutex{};

        std::optional<std::scoped_lock<std::mutex>> m_suspensionLock{};

        arcana::cancellation_source m_cancelSource{};
        arcana::task<void, std::error_code> m_task = arcana::task_from_result<std::error_code>();
        arcana::manual_dispatcher<128> m_dispatcher{};

        std::thread m_thread;

        std::function<void(std::exception_ptr)> m_unhandledExceptionHandler;
    };
}
