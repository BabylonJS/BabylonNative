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

    void AppRuntime::DefaultUnhandledExceptionHandler(std::exception_ptr ptr)
    {
        try
        {
            std::rethrow_exception(ptr);
        }
        catch (const Napi::Error& error)
        {
            std::stringstream ss{};
            ss << "Uncaught Error: " << error.Message() << std::endl;
            OutputDebugStringA(ss.str().data());
        }
    }

    void AppRuntime::Execute(Dispatchable<void()> callback)
    {
        callback();
    }
}
