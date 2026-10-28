#include "V8ForegroundTaskRunner.h"

#include <algorithm>
#include <chrono>
#include <mutex>
#include <unordered_set>
#include <utility>

namespace Babylon::Internal
{
    struct V8ForegroundTaskRunner::State
    {
        struct Pending
        {
            DelayedTaskScheduler::Id id{};
            bool completed{};
        };

        State(DispatchFunction dispatch, ScheduleFunction schedule, CancelFunction cancel)
            : dispatch{std::move(dispatch)}
            , schedule{std::move(schedule)}
            , cancel{std::move(cancel)}
        {
        }

        // Scheduling may complete synchronously; dispatch must only enqueue work.
        std::recursive_mutex mutex;
        DispatchFunction dispatch;
        ScheduleFunction schedule;
        CancelFunction cancel;
        std::unordered_set<std::shared_ptr<Pending>> pending;
    };

    V8ForegroundTaskRunner::V8ForegroundTaskRunner(DispatchFunction dispatch, ScheduleFunction schedule, CancelFunction cancel)
        : m_state{std::make_shared<State>(std::move(dispatch), std::move(schedule), std::move(cancel))}
    {
    }

    V8ForegroundTaskRunner::~V8ForegroundTaskRunner()
    {
        Shutdown();
    }

    void V8ForegroundTaskRunner::Shutdown()
    {
        std::scoped_lock lock{m_state->mutex};
        m_state->dispatch = {};
        auto pendingTasks = std::move(m_state->pending);
        m_state->pending.clear();
        for (const auto& pending : pendingTasks)
        {
            m_state->cancel(pending->id);
        }
    }

    void V8ForegroundTaskRunner::PostTask(std::unique_ptr<v8::Task> task)
    {
        std::scoped_lock lock{m_state->mutex};
        if (m_state->dispatch)
        {
            m_state->dispatch(std::move(task));
        }
    }

    void V8ForegroundTaskRunner::PostNonNestableTask(std::unique_ptr<v8::Task> task)
    {
        PostTask(std::move(task));
    }

    DelayedTaskScheduler::TimePoint V8ForegroundTaskRunner::GetScheduledTime(std::chrono::steady_clock::time_point now, double delayInSeconds)
    {
        return std::chrono::ceil<DelayedTaskScheduler::TimePoint::duration>(
            now + std::chrono::duration<double>{std::max(0.0, delayInSeconds)});
    }

    void V8ForegroundTaskRunner::PostDelayedTask(std::unique_ptr<v8::Task> task, double delayInSeconds)
    {
        const auto when = GetScheduledTime(std::chrono::steady_clock::now(), delayInSeconds);
        const auto state = m_state;
        std::scoped_lock lock{state->mutex};
        if (!state->dispatch)
        {
            return;
        }

        const auto pending = std::make_shared<State::Pending>();
        pending->id = state->schedule(when, [state, pending, task = std::shared_ptr<v8::Task>{std::move(task)}]() {
            std::scoped_lock callbackLock{state->mutex};
            pending->completed = true;
            state->pending.erase(pending);
            if (state->dispatch)
            {
                state->dispatch(task);
            }
        });
        // A callback that fired before Schedule returned must not be reinserted.
        if (!pending->completed)
        {
            state->pending.insert(pending);
        }
    }

    void V8ForegroundTaskRunner::PostNonNestableDelayedTask(std::unique_ptr<v8::Task> task, double delayInSeconds)
    {
        PostDelayedTask(std::move(task), delayInSeconds);
    }

    void V8ForegroundTaskRunner::PostIdleTask(std::unique_ptr<v8::IdleTask>)
    {
    }

    bool V8ForegroundTaskRunner::IdleTasksEnabled()
    {
        return false;
    }

    bool V8ForegroundTaskRunner::NonNestableTasksEnabled() const
    {
        return true;
    }

    bool V8ForegroundTaskRunner::NonNestableDelayedTasksEnabled() const
    {
        return true;
    }
}
