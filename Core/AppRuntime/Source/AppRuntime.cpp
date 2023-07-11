#include "AppRuntime.h"
#include "WorkQueue.h"
#include <Babylon/JsRuntime.h>

namespace Babylon
{
    AppRuntime::AppRuntime()
        : AppRuntime{DefaultUnhandledExceptionHandler}
    {
    }

    AppRuntime::AppRuntime(std::function<void(const std::exception&)> unhandledExceptionHandler)
        : m_workQueue{std::make_unique<WorkQueue>([this] { RunPlatformTier(); })}
        , m_unhandledExceptionHandler{unhandledExceptionHandler}
    {
        Dispatch([this](Napi::Env env) {
            JsRuntime::CreateForJavaScript(env, [this](auto func) { Dispatch(std::move(func)); });
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

    void AppRuntime::Dispatch(Dispatchable<void(Napi::Env)> func)
    {
        m_workQueue->Append([this, func{std::move(func)}](Napi::Env env) mutable {
            Execute([this, env, func{std::move(func)}]() mutable {
                try
                {
                    func(env);
                }
                catch (const std::exception& error)
                {
                    m_unhandledExceptionHandler(error);
                }
                catch (...)
                {
                    std::abort();
                }
            });
        });
    }
}
