#pragma once

#include <Babylon/Graphics/Device.h>

#include <string_view>

#include "ModuleSnapshot.h"

namespace ModuleLoadTest
{
    // Drive BabylonNative to a stable boot state, then return the snapshot of
    // loaded modules at that point. Platform-specific main() is responsible
    // for:
    //   1. Creating the platform window and Graphics::Configuration.
    //   2. Invoking RunBoot() with the config.
    //   3. Calling CompareAndReport() with the returned snapshot.
    ModuleSnapshot RunBoot(const Babylon::Graphics::Configuration& config);

    // Set-difference helper: elements in lhs that are not in rhs.
    ModuleSnapshot Subtract(const ModuleSnapshot& lhs, const ModuleSnapshot& rhs);

    // Print a labeled module list to stdout.
    void PrintList(const char* label, const ModuleSnapshot& items);

    // Provided by each platform's App.<Platform>.{cpp,mm}: the golden list of
    // modules expected to be loaded during boot, as a delta from the baseline
    // returned by GetPreInitBaseline(). Base names only, lower case.
    const ModuleSnapshot& GetExpectedBootModules();

    // Provided by each platform's App.<Platform>.{cpp,mm}: returns true for
    // modules whose presence in the delta is allowed but not required (e.g.
    // GPU-driver ICDs whose names vary by runner SKU, launch-environment
    // noise). See platform-specific comments for scope.
    bool IsAllowedOptionalModule(std::string_view name);

    // Compare a post-boot snapshot against the baseline + platform-specific
    // expected list and allowed-optional filter. Prints baseline+delta and
    // returns 0 on pass, 1 on fail. Called by each platform's main() after
    // RunBoot().
    int CompareAndReport(const ModuleSnapshot& postBoot);
}
