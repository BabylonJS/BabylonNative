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

            // Call the unhandled exception handler if an exception is thrown in one of the tasks.
            m_task = m_task.then(arcana::inline_scheduler, arcana::cancellation::none(), [this](const arcana::expected<void, std::exception_ptr>& result) {
                if (result.has_error())
                {
                    m_unhandledExceptionHandler(result.error());
                }
            });
        }

        m_dispatcher.clear();
        m_task = arcana::task_from_result<std::exception_ptr>();
    }
}
