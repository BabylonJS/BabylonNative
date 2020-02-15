#include "TaskChain.h"

namespace Babylon
{
    TaskChain::TaskChain(std::function<void()> threadProcedure)
        : m_thread{ std::move(threadProcedure) }
    {
    }

    TaskChain::~TaskChain()
    {
        if (m_suspended)
        {
            Resume();
        }
        m_cancelSource.cancel();
        m_thread.join();
    }

    void TaskChain::Append(std::function<void(Napi::Env)> callable)
    {
        std::scoped_lock lock{ m_appendMutex };
        m_task = m_task.then(m_dispatcher, m_cancelSource, [this, callable = std::move(callable)]
        {
            callable(m_env.value());
        });
    }

    void TaskChain::Suspend()
    {
        // Lock m_blockingTickMutex as well as m_suspendMutex to ensure we do not 
        // suspend in the middle of a blocking tick.
        std::scoped_lock<std::mutex> lockTicking(m_blockingTickMutex);
        std::scoped_lock<std::mutex> lockSuspension(m_suspendMutex);
        m_suspended = true;
        m_suspendConditionVariable.notify_one();
    }

    void TaskChain::Resume()
    {
        std::scoped_lock<std::mutex> lock(m_suspendMutex);
        m_suspended = false;
        m_suspendConditionVariable.notify_one();
    }

    void TaskChain::RunTaskChain(Napi::Env env)
    {
        m_env = std::make_optional(env);
        m_dispatcher.set_affinity(std::this_thread::get_id());

        while (!m_cancelSource.cancelled())
        {
            // check if suspended
            {
                std::unique_lock<std::mutex> lock{ m_suspendMutex };
                m_suspendConditionVariable.wait(lock, [this]() { return !m_suspended; });
            }
            {
                std::scoped_lock<std::mutex> lock{ m_blockingTickMutex };
                m_dispatcher.blocking_tick(m_cancelSource);
            }
        }
    }
}