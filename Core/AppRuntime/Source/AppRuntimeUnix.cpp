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

    void AppRuntime::DefaultUnhandledExceptionHandler(std::exception_ptr ptr)
    {
        try
        {
            std::rethrow_exception(ptr);
        }
        catch (const Napi::Error& error)
        {
            std::cerr << "Uncaught Error: " << error.Message() << std::endl;
        }
    }
}
