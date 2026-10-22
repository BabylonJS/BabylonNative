#pragma once

#include "DelayedTaskScheduler.h"
#include <napi/env.h>

#include <functional>
#include <memory>

namespace Babylon::Internal
{
    class V8ForegroundTaskRunner final : public v8::TaskRunner
    {
    public:
        using DispatchFunction = std::function<void(std::shared_ptr<v8::Task>)>;
        using ScheduleFunction = std::function<DelayedTaskScheduler::Id(DelayedTaskScheduler::TimePoint, DelayedTaskScheduler::Callback)>;
        using CancelFunction = std::function<void(DelayedTaskScheduler::Id)>;

        V8ForegroundTaskRunner(DispatchFunction dispatch, ScheduleFunction schedule, CancelFunction cancel);
        ~V8ForegroundTaskRunner() override;

        static DelayedTaskScheduler::TimePoint GetScheduledTime(std::chrono::steady_clock::time_point now, double delayInSeconds);

        // After shutdown, even references retained by V8 discard new posts.
        void Shutdown();

        void PostTask(std::unique_ptr<v8::Task> task) override;
        void PostNonNestableTask(std::unique_ptr<v8::Task> task) override;
        void PostDelayedTask(std::unique_ptr<v8::Task> task, double delayInSeconds) override;
        void PostNonNestableDelayedTask(std::unique_ptr<v8::Task> task, double delayInSeconds) override;
        void PostIdleTask(std::unique_ptr<v8::IdleTask>) override;
        bool IdleTasksEnabled() override;
        bool NonNestableTasksEnabled() const override;
        bool NonNestableDelayedTasksEnabled() const override;

    private:
        struct State;
        std::shared_ptr<State> m_state;
    };
}
