#include "AppRuntime.h"
#include <exception>

#import <Foundation/NSObjCRuntime.h>

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
            NSLog(@"Uncaught Error: %s", error.what());
        }
    }
}
