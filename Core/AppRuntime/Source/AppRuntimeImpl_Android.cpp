#include "AppRuntimeImpl.h"
#include <exception>
#include <sstream>
#include <android/log.h>

namespace Babylon
{
    void AppRuntimeImpl::RunPlatformTier()
    {
        RunEnvironmentTier();
    }

    void AppRuntimeImpl::DefaultUnhandledExceptionHandler(const std::exception& error)
    {
        std::stringstream ss{};
        ss << "Uncaught Error: " << error.what() << std::endl;
        __android_log_write(ANDROID_LOG_ERROR, "BabylonNative", ss.str().data());
    }

    void AppRuntimeImpl::Execute(Dispatchable<void()> callback)
    {
        callback();
    }
}
