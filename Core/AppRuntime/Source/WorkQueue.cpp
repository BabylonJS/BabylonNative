#include "WorkQueue.h"

namespace Babylon
{
    WorkQueue::WorkQueue(std::function<void()> threadProcedure)
        : m_thread{std::move(threadProcedure)}
    {
    }

    WorkQueue::~WorkQueue()
    {
        if (m_suspensionLock != nullptr)
        {
            Resume();
        }

        m_cancelSource.cancel();
        m_dispatcher.cancelled();

        m_thread.join();
    }

    void WorkQueue::Append(std::function<void(Napi::Env)> callable)
    {
        std::scoped_lock lock{m_appendMutex};
        m_task = m_task.then(m_dispatcher, m_cancelSource, [this, callable = std::move(callable)] {
            callable(m_env.value());
        });
    }

    void WorkQueue::Suspend()
    {
        auto suspensionMutex = new std::mutex();
        m_suspensionLock = std::make_unique<std::scoped_lock<std::mutex>>(*suspensionMutex);
        Append([suspensionMutex](Napi::Env) {
            std::scoped_lock{*suspensionMutex};
            delete suspensionMutex;
        });
    }

    void WorkQueue::Resume()
    {
        m_suspensionLock.reset();
    }

    void WorkQueue::Run(Napi::Env env)
    {
        m_env = std::make_optional(env);
        m_dispatcher.set_affinity(std::this_thread::get_id());

        while (!m_cancelSource.cancelled())
        {
            {
                std::scoped_lock<std::mutex> lock{m_blockingTickMutex};
                m_dispatcher.blocking_tick(m_cancelSource);
            }
        }

        m_dispatcher.clear();
        m_task = arcana::task_from_result<std::exception_ptr>();
    }
}
