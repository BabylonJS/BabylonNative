#include "AppRuntime.h"
#include "WorkQueue.h"
#include <cassert>

namespace Babylon
{
    AppRuntime::AppRuntime() :
        AppRuntime{{}}
    {
    }

    AppRuntime::AppRuntime(Options options)
        : m_workQueue{std::make_unique<WorkQueue>([this] { RunPlatformTier(); })}
        , m_options{std::move(options)}
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
                catch (const Napi::Error& error)
                {
                    m_options.UnhandledExceptionHandler(error);
                }
                catch (...)
                {
                    assert(false);
                    std::abort();
                }
            });
        });
    }
}
