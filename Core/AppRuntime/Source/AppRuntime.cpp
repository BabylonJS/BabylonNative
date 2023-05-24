#include "AppRuntime.h"
#include "AppRuntimeImpl.h"

namespace Babylon
{
    AppRuntime::AppRuntime()
        : m_impl{std::make_unique<AppRuntimeImpl>()}
    {
    }

    AppRuntime::AppRuntime(std::function<void(const std::exception&)> unhandledExceptionHandler)
        : m_impl{std::make_unique<AppRuntimeImpl>(unhandledExceptionHandler)}
    {
    }

    AppRuntime::~AppRuntime() = default;

    // Move semantics
    AppRuntime::AppRuntime(AppRuntime&&) noexcept = default;
    AppRuntime& AppRuntime::operator=(AppRuntime&&) noexcept = default;

    void AppRuntime::Suspend()
    {
        m_impl->Suspend();
    }

    void AppRuntime::Resume()
    {
        m_impl->Resume();
    }

    void AppRuntime::Dispatch(Dispatchable<void(Napi::Env)> callback)
    {
        m_impl->Dispatch(std::move(callback));
    }
}
