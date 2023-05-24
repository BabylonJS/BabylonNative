#include "AppRuntimeImpl.h"

#include <winrt/base.h>
#include <roapi.h>

#include <gsl/gsl>
#include <cassert>
#include <exception>
#include <sstream>

namespace Babylon
{
    void AppRuntimeImpl::RunPlatformTier()
    {
        winrt::check_hresult(Windows::Foundation::Initialize(RO_INIT_MULTITHREADED));

        char executablePath[1024];
        winrt::check_bool(GetModuleFileNameA(nullptr, executablePath, ARRAYSIZE(executablePath)) != 0);

        RunEnvironmentTier(executablePath);
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
