#include "AppRuntime.h"

#include "AppRuntimeImpl.h"

namespace Babylon
{
    AppRuntime::AppRuntime(const char* rootUrl)
        : JsRuntime([this](auto func) { Dispatch(std::move(func)); })
        , m_impl{std::make_unique<Impl>(*this, rootUrl)}
    {
    }

    const std::string& AppRuntime::GetRootUrl() const
    {
        return m_impl->RootUrl;
    }

    void AppRuntime::Suspend()
    {
        m_impl->TaskChain->Suspend();
    }

    void AppRuntime::Resume()
    {
        m_impl->TaskChain->Resume();
    }

    void AppRuntime::Dispatch(std::function<void(Napi::Env)> func)
    {
        m_impl->TaskChain->Append(std::move(func));
    }
}