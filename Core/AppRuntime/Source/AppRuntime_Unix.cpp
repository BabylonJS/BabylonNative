#include "WorkQueue.h"
#include "AppRuntime.h"
#include <exception>
#include <iostream>

namespace Babylon
{
    void AppRuntime::RunPlatformTier()
    {
        RunEnvironmentTier();
    }

    void AppRuntime::DefaultUnhandledExceptionHandler(const std::exception& error)
    {
        std::cerr << "Uncaught Error: " << error.what() << std::endl;
    }

    void AppRuntime::Execute(std::function<void()> callback)
    {
        callback();
    }
}
