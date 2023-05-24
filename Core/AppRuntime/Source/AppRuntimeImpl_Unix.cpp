#include "AppRuntimeImpl.h"
#include <exception>
#include <iostream>

namespace Babylon
{
    void AppRuntimeImpl::RunPlatformTier()
    {
        RunEnvironmentTier();
    }

    void AppRuntimeImpl::DefaultUnhandledExceptionHandler(const std::exception& error)
    {
        std::cerr << "Uncaught Error: " << error.what() << std::endl;
    }

    void AppRuntimeImpl::Execute(Dispatchable<void()> callback)
    {
        callback();
    }
}
