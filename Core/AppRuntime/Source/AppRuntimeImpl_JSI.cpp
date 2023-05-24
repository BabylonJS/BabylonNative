#include "AppRuntimeImpl.h"

#include <Babylon/JsRuntime.h>

#include <V8JsiRuntime.h>
#include <ScriptStore.h>

namespace
{
    class TaskRunnerAdapter : public v8runtime::JSITaskRunner
    {
    public:
        TaskRunnerAdapter(Babylon::AppRuntimeImpl& appRuntimeImpl)
            : m_appRuntimeImpl(appRuntimeImpl)
        {
        }

        void postTask(std::unique_ptr<v8runtime::JSITask> task) override
        {
            std::shared_ptr<v8runtime::JSITask> shared_task(task.release());
            m_appRuntimeImpl.Dispatch([shared_task2 = std::move(shared_task)](Napi::Env) {
                shared_task2->run();
            });
        }

    private:
        TaskRunnerAdapter(const TaskRunnerAdapter&) = delete;
        TaskRunnerAdapter& operator=(const TaskRunnerAdapter&) = delete;

        Babylon::AppRuntimeImpl& m_appRuntimeImpl;
    };
}

namespace Babylon
{
    void AppRuntimeImpl::RunEnvironmentTier(const char*)
    {
        v8runtime::V8RuntimeArgs args{};
        args.inspectorPort = 5643;
        args.foreground_task_runner = std::make_shared<TaskRunnerAdapter>(*this);

        const auto runtime{v8runtime::makeV8Runtime(std::move(args))};
        const auto env{Napi::Attach<facebook::jsi::Runtime&>(*runtime)};
        Dispatch([&runtime](Napi::Env env) {
            JsRuntime::NativeObject::GetFromJavaScript(env)
                .Set("_JSIRuntime", Napi::External<facebook::jsi::Runtime>::New(env, runtime.get()));
        });
        Run(env);
        Napi::Detach(env);
    }
}
