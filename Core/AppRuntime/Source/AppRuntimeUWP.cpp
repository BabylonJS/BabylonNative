#include <napi/napi.h>
#include "AppRuntime.h"

namespace Babylon
{
    void AppRuntime::RunPlatformTier()
    {
        RunEnvironmentTier();
    }
}
