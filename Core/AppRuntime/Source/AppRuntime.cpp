#include "AppRuntime.h"
#include "WorkQueue.h"

namespace Babylon
{
    AppRuntime::AppRuntime()
        : AppRuntime{DefaultUnhandledExceptionHandler}
    {
    }

    AppRuntime::AppRuntime(std::function<void(const Napi::Error&)> unhandledExceptionHandler)
        : m_workQueue{std::make_unique<WorkQueue>([this] { RunPlatformTier(); })}
    {
        Dispatch([this, unhandledExceptionHandler{std::move(unhandledExceptionHandler)}](Napi::Env env) {
            JsRuntime::CreateForJavaScript(env, [this](auto func) { m_workQueue->Append(std::move(func)); }, unhandledExceptionHandler);
        });
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
        m_workQueue->Append([this, func{std::move(func)}] (Napi::Env env) {
            Execute({[env, func{std::move(func)}] { func(env); }});
        });
    }
}
