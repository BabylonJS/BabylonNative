#include "AppRuntime.h"
#include <iostream>

namespace Babylon
{
    void AppRuntime::DefaultUnhandledExceptionHandler(const Napi::Error& error)
    {
        std::cerr << "[Uncaught Error] " << error.Get("stack").As<Napi::String>().Utf8Value().data() << std::endl;
    }

    void AppRuntime::RunPlatformTier()
    {
        RunEnvironmentTier();
    }

    void AppRuntime::Execute(Dispatchable<void()> callback)
    {
        callback();
    }
}
