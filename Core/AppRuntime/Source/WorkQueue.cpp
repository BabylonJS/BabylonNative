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

    std::future<void> WorkQueue::Suspend()
    {
        std::promise<void> promise{};
        std::future<void> future = promise.get_future();

        auto suspensionMutex = std::make_unique<std::mutex>();
        m_suspensionLock = std::make_unique<std::scoped_lock<std::mutex>>(*suspensionMutex);
        Append([suspensionMutex{std::move(suspensionMutex)}, promise{std::move(promise)}](Napi::Env) mutable {
            promise.set_value();
            std::scoped_lock lock{*suspensionMutex};
        });

        return future;
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
            m_dispatcher.blocking_tick(m_cancelSource);
        }

        m_dispatcher.clear();
        m_task = arcana::task_from_result<std::exception_ptr>();
    }
}
