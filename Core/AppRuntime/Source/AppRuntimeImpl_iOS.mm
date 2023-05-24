#include "AppRuntimeImpl.h"
#include <exception>

#import <Foundation/NSObjCRuntime.h>

namespace Babylon
{
    void AppRuntimeImpl::RunPlatformTier()
    {
        RunEnvironmentTier();
    }

    void AppRuntimeImpl::DefaultUnhandledExceptionHandler(const std::exception& error)
    {
        NSLog(@"Uncaught Error: %s", error.what());
    }

    void AppRuntimeImpl::Execute(Dispatchable<void()> callback)
    {
        @autoreleasepool
        {
            callback();
        }
    }
}
