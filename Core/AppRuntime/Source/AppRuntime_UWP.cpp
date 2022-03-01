#include "AppRuntime.h"

#include <Windows.h>

#include <exception>
#include <sstream>

namespace Babylon
{
    void AppRuntime::RunPlatformTier()
    {
        RunEnvironmentTier();
    }

    void AppRuntime::DefaultUnhandledExceptionHandler(const std::exception& error)
    {
        std::stringstream ss{};
        ss << "Uncaught Error: " << error.what() << std::endl;
        OutputDebugStringA(ss.str().data());
    }

    void AppRuntime::Execute(std::function<void()> callback)
    {
        callback();
    }
}
