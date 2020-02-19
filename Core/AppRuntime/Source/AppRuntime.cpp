#include "AppRuntime.h"

#include "TaskChain.h"

namespace Babylon
{
    AppRuntime::AppRuntime(const char* rootUrl)
        : JsRuntime([this](auto func) { Dispatch(std::move(func)); })
        , m_rootUrl{rootUrl}
        , m_taskChain{std::make_unique<TaskChain>([this] { CreateEnvironmentAndRun(); })}
    {
    }

    AppRuntime::~AppRuntime()
    {
    }
    
    void AppRuntime::Run(Napi::Env env)
    {
        AddJavaScriptReference(env, false);
        m_taskChain->RunTaskChain(env);
    }

    const std::string& AppRuntime::GetRootUrl() const
    {
        return m_rootUrl;
    }

    void AppRuntime::Suspend()
    {
        m_taskChain->Suspend();
    }

    void AppRuntime::Resume()
    {
        m_taskChain->Resume();
    }

    void AppRuntime::Dispatch(std::function<void(Napi::Env)> func)
    {
        m_taskChain->Append(std::move(func));
    }
}