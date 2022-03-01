#include "AppRuntime.h"
#include <exception>
#include <sstream>
#include <android/log.h>

namespace Babylon
{
    void AppRuntime::RunPlatformTier()
    {
        RunEnvironmentTier();
    }

    void AppRuntime::DefaultUnhandledExceptionHandler(const std::exception& error)
    {
        std::stringstream ss{};
        ss << "Uncaught Error: " << error.what() << std::endl;
        __android_log_write(ANDROID_LOG_ERROR, "BabylonNative", ss.str().data());
    }

    void AppRuntime::Execute(std::function<void()> callback)
    {
        callback();
    }
}
