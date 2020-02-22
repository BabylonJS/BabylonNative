#include "AppRuntime.h"

#include "WorkQueue.h"

namespace Babylon
{
    AppRuntime::AppRuntime(const char* rootUrl)
        : AppRuntime(rootUrl, std::make_unique<WorkQueue>([this] { RunPlatformTier(); }))
    {
    }

    AppRuntime::AppRuntime(const char* rootUrl, std::unique_ptr<WorkQueue> workQueue)
        : JsRuntime([& workQueue = *workQueue](auto func) { workQueue.Append(std::move(func)); })
        , RootUrl{rootUrl}
        , m_workQueue{std::move(workQueue)}
    {
    }

    AppRuntime::~AppRuntime()
    {
    }

    void AppRuntime::Run(Napi::Env env)
    {
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
