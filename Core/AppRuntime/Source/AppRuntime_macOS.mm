#include "AppRuntime.h"
#include <exception>

#import <Foundation/NSObjCRuntime.h>

namespace Babylon
{
    void AppRuntime::RunPlatformTier()
    {
        RunEnvironmentTier();
    }

    void AppRuntime::DefaultUnhandledExceptionHandler(const std::exception& error)
    {
        NSLog(@"Uncaught Error: %s", error.what());
    }

    void AppRuntime::Execute(std::function<void()> callback)
    {
        @autoreleasepool
        {
            callback();
        }
    }
}
