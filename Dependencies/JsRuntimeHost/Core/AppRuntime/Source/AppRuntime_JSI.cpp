#include "AppRuntime.h"

#include <napi/env.h>
#include <V8JsiRuntime.h>
#include <ScriptStore.h>

namespace
{
    class TaskRunnerAdapter : public v8runtime::JSITaskRunner
    {
    public:
        TaskRunnerAdapter(Babylon::AppRuntime& runtime)
            : m_runtime(runtime)
        {
        }

        void postTask(std::unique_ptr<v8runtime::JSITask> task) override
        {
            std::shared_ptr<v8runtime::JSITask> shared_task(std::move(task));
            m_runtime.Dispatch([shared_task2 = std::move(shared_task)](Napi::Env) {
                shared_task2->run();
            });
        }

    private:
        TaskRunnerAdapter(const TaskRunnerAdapter&) = delete;
        TaskRunnerAdapter& operator=(const TaskRunnerAdapter&) = delete;

        Babylon::AppRuntime& m_runtime;
    };
}

namespace Babylon
{
    void AppRuntime::RunEnvironmentTier(const char*)
    {
        v8runtime::V8RuntimeArgs args{};
        args.inspectorPort = 5643;
        args.foreground_task_runner = std::make_shared<TaskRunnerAdapter>(*this);
        const auto runtime = v8runtime::makeV8Runtime(std::move(args));

        const auto env = Napi::Attach(*runtime);

        Run(env);

        Napi::Detach(env);
    }
}
