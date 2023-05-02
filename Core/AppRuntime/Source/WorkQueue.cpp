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
        m_env = std::make_optional(env);

        m_dispatcher.set_affinity(std::this_thread::get_id());

        while (!m_cancellationSource.cancelled())
        {
            m_dispatcher.blocking_tick(m_cancellationSource);
        }

        // The dispatcher can be non-empty if something is dispatched after cancellation.
        // For example, Chakra's JsSetPromiseContinuationCallback may potentially dispatch
        // a continuation after cancellation.
        m_dispatcher.clear();

        m_env.reset();
    }
}
