#include "AppRuntime.h"
#import <Foundation/NSObjCRuntime.h>

namespace Babylon
{
    void AppRuntime::DefaultUnhandledExceptionHandler(const Napi::Error& error)
    {
        NSLog(@"[Uncaught Error] %s", error.Get("stack").As<Napi::String>().Utf8Value().data());
    }

    void AppRuntime::RunPlatformTier()
    {
        RunEnvironmentTier();
    }

    void AppRuntime::Execute(Dispatchable<void()> callback)
    {
        @autoreleasepool
        {
            callback();
        }
    }
}
