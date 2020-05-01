#include "AppRuntime.h"

#include "WorkQueue.h"

#include <Babylon/AppLifeCycle.h>

namespace Babylon
{
    AppRuntime::AppRuntime()
        : m_workQueue{std::make_unique<WorkQueue>([this] { RunPlatformTier(); })}
    {
        Dispatch([this](Napi::Env env) {
            JsRuntime::CreateForJavaScript(env, [this](auto func) { m_workQueue->Append(std::move(func)); });
            AppLifeCycle::CreateForJavaScript(env);
        });
    }

    AppRuntime::~AppRuntime()
    {
    }

    void AppRuntime::Run(Napi::Env env)
    {
        m_workQueue->Run(env);
    }

    std::future<void> AppRuntime::Suspend()
    {
        Dispatch([](auto env) {
            AppLifeCycle::GetFromJavaScript(env).Suspend();
        });
        return m_workQueue->Suspend();
    }

    void AppRuntime::Resume()
    {
        Dispatch([](auto env) {
            AppLifeCycle::GetFromJavaScript(env).Resume();
        });
        m_workQueue->Resume();
    }

    void AppRuntime::Dispatch(std::function<void(Napi::Env)> func)
    {
        m_workQueue->Append(std::move(func));
    }
}
