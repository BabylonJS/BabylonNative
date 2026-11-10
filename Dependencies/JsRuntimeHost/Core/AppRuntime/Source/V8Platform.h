#pragma once

#include <napi/env.h>
#include <v8-version.h>

#include <map>
#include <memory>
#include <mutex>
#include <utility>

// Android uses V8 11.0; desktop uses 11.9.
#define JSRH_V8_AT_LEAST(major, minor) \
    (V8_MAJOR_VERSION > (major) || (V8_MAJOR_VERSION == (major) && V8_MINOR_VERSION >= (minor)))

namespace Babylon
{
    class AppRuntime;

    namespace Internal
    {
        class DelayedTaskScheduler;
        class V8ForegroundTaskRunner;

        class V8Platform final : public v8::Platform
        {
        public:
            explicit V8Platform(std::unique_ptr<v8::Platform> inner);

            void RegisterHost(v8::Isolate* isolate, AppRuntime& runtime, DelayedTaskScheduler& scheduler);
            void UnregisterHost(v8::Isolate* isolate);

            v8::PageAllocator* GetPageAllocator() override { return m_inner->GetPageAllocator(); }
#if JSRH_V8_AT_LEAST(11, 9)
            v8::ThreadIsolatedAllocator* GetThreadIsolatedAllocator() override { return m_inner->GetThreadIsolatedAllocator(); }
#endif
            v8::ZoneBackingAllocator* GetZoneBackingAllocator() override { return m_inner->GetZoneBackingAllocator(); }
            void OnCriticalMemoryPressure() override { m_inner->OnCriticalMemoryPressure(); }
            int NumberOfWorkerThreads() override { return m_inner->NumberOfWorkerThreads(); }
            std::shared_ptr<v8::TaskRunner> GetForegroundTaskRunner(v8::Isolate* isolate) override;
#if JSRH_V8_AT_LEAST(11, 9)
            std::shared_ptr<v8::TaskRunner> GetForegroundTaskRunner(v8::Isolate* isolate, v8::TaskPriority) override
            {
                return GetForegroundTaskRunner(isolate);
            }
#endif
            void CallOnWorkerThread(std::unique_ptr<v8::Task> task) override { m_inner->CallOnWorkerThread(std::move(task)); }
            void CallBlockingTaskOnWorkerThread(std::unique_ptr<v8::Task> task) override { m_inner->CallBlockingTaskOnWorkerThread(std::move(task)); }
            void CallLowPriorityTaskOnWorkerThread(std::unique_ptr<v8::Task> task) override { m_inner->CallLowPriorityTaskOnWorkerThread(std::move(task)); }
            void CallDelayedOnWorkerThread(std::unique_ptr<v8::Task> task, double delayInSeconds) override { m_inner->CallDelayedOnWorkerThread(std::move(task), delayInSeconds); }
            bool IdleTasksEnabled(v8::Isolate*) override { return false; }
            std::unique_ptr<v8::JobHandle> PostJob(v8::TaskPriority priority, std::unique_ptr<v8::JobTask> jobTask) override { return m_inner->PostJob(priority, std::move(jobTask)); }
            std::unique_ptr<v8::JobHandle> CreateJob(v8::TaskPriority priority, std::unique_ptr<v8::JobTask> jobTask) override { return m_inner->CreateJob(priority, std::move(jobTask)); }
#if JSRH_V8_AT_LEAST(11, 9)
            std::unique_ptr<v8::ScopedBlockingCall> CreateBlockingScope(v8::BlockingType blockingType) override { return m_inner->CreateBlockingScope(blockingType); }
#endif
            double MonotonicallyIncreasingTime() override { return m_inner->MonotonicallyIncreasingTime(); }
#if JSRH_V8_AT_LEAST(11, 9)
            int64_t CurrentClockTimeMilliseconds() override { return m_inner->CurrentClockTimeMilliseconds(); }
#endif
            double CurrentClockTimeMillis() override { return m_inner->CurrentClockTimeMillis(); }
#if JSRH_V8_AT_LEAST(11, 9)
            double CurrentClockTimeMillisecondsHighResolution() override { return m_inner->CurrentClockTimeMillisecondsHighResolution(); }
#endif
            StackTracePrinter GetStackTracePrinter() override { return m_inner->GetStackTracePrinter(); }
            v8::TracingController* GetTracingController() override { return m_inner->GetTracingController(); }
            void DumpWithoutCrashing() override { m_inner->DumpWithoutCrashing(); }
            v8::HighAllocationThroughputObserver* GetHighAllocationThroughputObserver() override { return m_inner->GetHighAllocationThroughputObserver(); }

        private:
            std::unique_ptr<v8::Platform> m_inner;
            std::mutex m_mutex;
            std::map<v8::Isolate*, std::shared_ptr<V8ForegroundTaskRunner>> m_taskRunners;
            std::shared_ptr<V8ForegroundTaskRunner> m_inactiveRunner;
        };
    }
}

#undef JSRH_V8_AT_LEAST
