#include "AppRuntime.h"
#include <Windows.h>
#include <sstream>

namespace Babylon
{
    void BABYLON_API AppRuntime::DefaultUnhandledExceptionHandler(const Napi::Error& error)
    {
        std::ostringstream ss{};
        ss << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        OutputDebugStringA(ss.str().data());
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
