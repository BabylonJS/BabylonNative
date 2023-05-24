#include "AppRuntimeImpl.h"

#include <winrt/base.h>
#include <roapi.h>

#include <exception>
#include <sstream>

namespace Babylon
{
    void AppRuntimeImpl::RunPlatformTier()
    {
        winrt::check_hresult(Windows::Foundation::Initialize(RO_INIT_MULTITHREADED));

        RunEnvironmentTier();
    }

    void AppRuntimeImpl::DefaultUnhandledExceptionHandler(const std::exception& error)
    {
        std::stringstream ss{};
        ss << "Uncaught Error: " << error.what() << std::endl;
        OutputDebugStringA(ss.str().data());
    }

    void AppRuntimeImpl::Execute(Dispatchable<void()> callback)
    {
        callback();
    }
}
