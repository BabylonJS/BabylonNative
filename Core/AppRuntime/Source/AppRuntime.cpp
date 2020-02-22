#include "AppRuntime.h"

#include "WorkQueue.h"

namespace Babylon
{
    AppRuntime::AppRuntime(const char* rootUrl)
        : JsRuntime([this](auto func) { Dispatch(std::move(func)); })
        , RootUrl{rootUrl}
        , m_workQueue{std::make_unique<WorkQueue>([this] { CreateEnvironmentAndRun(); })}
    {
    }

    AppRuntime::~AppRuntime()
    {
    }

    void AppRuntime::Run(Napi::Env env)
    {
        AddJavaScriptReference(env, false);
        m_workQueue->Run(env);
    }

    void AppRuntime::Suspend()
    {
        m_workQueue->Suspend();
    }

    void AppRuntime::Resume()
    {
        m_workQueue->Resume();
    }

    void AppRuntime::Dispatch(std::function<void(Napi::Env)> func)
    {
        m_workQueue->Append(std::move(func));
    }
}
