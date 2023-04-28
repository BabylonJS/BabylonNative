#include "WorkQueue.h"

namespace Babylon
{
    WorkQueue::WorkQueue(std::function<void()> threadProcedure)
        : m_thread{std::move(threadProcedure)}
    {
    }

    WorkQueue::~WorkQueue()
    {
        if (m_suspensionLock.has_value())
        {
            Resume();
        }

        // Dispatch a cancel to signal the Run function to gracefully end.
        // It must be dispatched and not canceled directly to ensure that
        // existing work is executed and executed in the correct order.
        m_dispatcher([this]() {
            m_cancellationSource.cancel();
        });

        m_thread.join();
    }

    void WorkQueue::Suspend()
    {
        auto suspensionMutex = std::make_shared<std::mutex>();
        m_suspensionLock.emplace(*suspensionMutex);
        Append([suspensionMutex{std::move(suspensionMutex)}](Napi::Env) {
            std::scoped_lock lock{*suspensionMutex};
        });
    }

    void WorkQueue::Resume()
    {
        m_suspensionLock.reset();
    }

    void WorkQueue::Run(Napi::Env env)
    {
        m_dispatcher.set_affinity(std::this_thread::get_id());

        m_env = std::make_optional(env);

        while (!m_cancellationSource.cancelled())
        {
            m_dispatcher.blocking_tick(m_cancellationSource);
        }

        // Drain the queue to complete work dispatched after cancellation.
        m_dispatcher.tick(arcana::cancellation::none());

        // There should no longer be any outstanding work once the queue is drained.
        assert(m_dispatcher.empty());

        // Clear the shutdown queue to make sure the callables are destroyed on this thread.
        m_shutdownQueue.clear();

        m_env.reset();
    }
}
