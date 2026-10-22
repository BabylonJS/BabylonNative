#include "V8Platform.h"
#include "V8ForegroundTaskRunner.h"

#include "AppRuntime.h"
#include <v8.h>

#include <stdexcept>

namespace Babylon::Internal
{
    V8Platform::V8Platform(std::unique_ptr<v8::Platform> inner)
        : m_inner{std::move(inner)}
        , m_inactiveRunner{std::make_shared<V8ForegroundTaskRunner>(nullptr, nullptr, nullptr)}
    {
    }

    void V8Platform::RegisterHost(v8::Isolate* isolate, AppRuntime& runtime, DelayedTaskScheduler& scheduler)
    {
        auto runner = std::make_shared<V8ForegroundTaskRunner>(
            [&runtime, isolate](std::shared_ptr<v8::Task> task) {
                runtime.Dispatch([task = std::move(task), isolate](Napi::Env) {
                    v8::Isolate::Scope isolateScope{isolate};
                    task->Run();
                });
            },
            [&scheduler](DelayedTaskScheduler::TimePoint when, DelayedTaskScheduler::Callback callback) {
                return scheduler.Schedule(when, std::move(callback));
            },
            [&scheduler](DelayedTaskScheduler::Id id) {
                scheduler.Cancel(id);
            });

        std::scoped_lock lock{m_mutex};
        if (!m_taskRunners.try_emplace(isolate, std::move(runner)).second)
        {
            throw std::logic_error{"V8 host already registered"};
        }
    }

    void V8Platform::UnregisterHost(v8::Isolate* isolate)
    {
        std::shared_ptr<V8ForegroundTaskRunner> runner;
        {
            std::scoped_lock lock{m_mutex};
            const auto entry = m_taskRunners.find(isolate);
            if (entry == m_taskRunners.end())
            {
                throw std::logic_error{"V8 host is not registered"};
            }
            runner = std::move(entry->second);
            m_taskRunners.erase(entry);
        }
        runner->Shutdown();
    }

    std::shared_ptr<v8::TaskRunner> V8Platform::GetForegroundTaskRunner(v8::Isolate* isolate)
    {
        std::scoped_lock lock{m_mutex};
        const auto entry = m_taskRunners.find(isolate);
        // Disposal can request a runner after unregistration. Never recreate routing.
        return entry == m_taskRunners.end() ? m_inactiveRunner : entry->second;
    }
}
