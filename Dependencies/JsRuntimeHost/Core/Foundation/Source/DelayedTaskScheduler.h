#pragma once

#include <napi/env.h>

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

namespace Babylon::Internal
{
    /// Native delayed-work queue used by setTimeout/setInterval and by host
    /// task runners. Callbacks run on the scheduler thread; callers that need
    /// the JavaScript thread must dispatch there themselves.
    class DelayedTaskScheduler final
    {
    public:
        using Id = int32_t;
        using Callback = std::function<void()>;
        using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>;

        DelayedTaskScheduler();
        ~DelayedTaskScheduler();

        DelayedTaskScheduler(const DelayedTaskScheduler&) = delete;
        DelayedTaskScheduler& operator=(const DelayedTaskScheduler&) = delete;

        // Environment association is non-owning and accessed on the JavaScript
        // thread. The associated scheduler must outlive its borrowers.
        static void SetForJavaScript(Napi::Env env, DelayedTaskScheduler& scheduler);
        static void ClearFromJavaScript(Napi::Env env);
        static DelayedTaskScheduler* GetFromJavaScript(Napi::Env env);

        Id Schedule(TimePoint when, Callback callback);
        Id Schedule(std::chrono::milliseconds delay, Callback callback);

        // Does not wait for a callback already extracted by the worker.
        void Cancel(Id id);

        // Drops queued work, waits for an extracted callback to finish, and
        // rejects subsequent Schedule calls.
        void Shutdown();

    private:
        friend struct DelayedTaskSchedulerTestAccess;
        explicit DelayedTaskScheduler(Id lastId);

        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
