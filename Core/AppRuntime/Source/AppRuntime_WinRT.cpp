#include "AppRuntime.h"

#include <winrt/base.h>
#include <roapi.h>

#include <exception>
#include <sstream>

namespace Babylon
{
    void AppRuntime::RunPlatformTier()
    {
        winrt::check_hresult(Windows::Foundation::Initialize(RO_INIT_MULTITHREADED));

        RunEnvironmentTier();
    }

    void AppRuntime::DefaultUnhandledExceptionHandler(const std::exception& error)
    {
        std::stringstream ss{};
        ss << "Uncaught Error: " << error.what() << std::endl;
        OutputDebugStringA(ss.str().data());
    }

    void AppRuntime::Execute(Dispatchable<void()> callback)
    {
        callback();
    }
}
