#include "WorkQueue.h"
#include "AppRuntime.h"
#include <exception>
#include <iostream>
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
        ss << error.what() << std::endl;
        ss << GetErrorInfos() << std::endl;

        std::cerr << "Uncaught Error: " << ss.str().data() << std::endl;
    }

    void AppRuntime::Execute(Dispatchable<void()> callback)
    {
        callback();
    }
}
