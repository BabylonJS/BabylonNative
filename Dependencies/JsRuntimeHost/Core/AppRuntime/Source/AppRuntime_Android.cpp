#include "AppRuntime.h"
#include <android/log.h>

namespace Babylon
{
    void AppRuntime::DefaultUnhandledExceptionHandler(const Napi::Error& error)
    {
        __android_log_print(ANDROID_LOG_ERROR, "BabylonNative", "[Uncaught Error] %s", error.Get("stack").As<Napi::String>().Utf8Value().data());
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
