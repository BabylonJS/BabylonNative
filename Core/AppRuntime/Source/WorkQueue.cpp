#include "WorkQueue.h"

namespace Babylon
{
    WorkQueue::WorkQueue(std::function<void()> threadProcedure, std::function<void(std::exception_ptr)> unhandledExceptionHandler)
        : m_thread{std::move(threadProcedure)}
        , m_unhandledExceptionHandler{std::move(unhandledExceptionHandler)}
    {
    }

    WorkQueue::~WorkQueue()
    {
        if (m_suspensionLock.has_value())
        {
            Resume();
        }

        m_cancelSource.cancel();
        m_dispatcher.cancelled();

        m_thread.join();
    }

    void WorkQueue::Suspend()
    {
        auto suspensionMutex = std::make_unique<std::mutex>();
        m_suspensionLock.emplace(*suspensionMutex);
        Append([suspensionMutex{std::move(suspensionMutex)}](Napi::Env) mutable {
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

        while (!m_cancelSource.cancelled())
        {
            m_dispatcher.blocking_tick(m_cancelSource);
        }

        m_dispatcher.clear();
    }
}
