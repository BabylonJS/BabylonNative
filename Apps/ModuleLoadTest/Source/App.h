#pragma once

#include <Babylon/Graphics/Device.h>

#include "ModuleSnapshot.h"

namespace ModuleLoadTest
{
    // Drive BabylonNative to a stable boot state, then return the snapshot of
    // loaded modules at that point. Platform-specific main() is responsible
    // for:
    //   1. Creating the platform window and Graphics::Configuration.
    //   2. Invoking RunBoot() with the config.
    //   3. Comparing the returned post-boot snapshot against GetPreInitBaseline()
    //      and the expected golden list.
    ModuleSnapshot RunBoot(const Babylon::Graphics::Configuration& config);
}
