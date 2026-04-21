#pragma once

#include <Babylon/Graphics/Device.h>

#include <optional>
#include <string_view>

#include "ModuleSnapshot.h"

namespace ModuleLoadTest
{
    // Drive BabylonNative to a stable boot state, then return the snapshot of
    // loaded modules at that point.
    ModuleSnapshot RunBoot(const Babylon::Graphics::Configuration& config);

    // Provided by each platform's App.<Platform>.{cpp,mm}: set up the platform
    // window / graphics device and DebugTrace output, then return a populated
    // Graphics::Configuration. Platform-owned resources (HWND / Display / Metal
    // device) are parked in function-local static storage so they live for the
    // duration of the process; teardown happens at process exit.
    //
    // Returns nullopt for platform-level SKIPs (no X display, no Metal device)
    // after the platform has printed an explanatory message.
    std::optional<Babylon::Graphics::Configuration> CreateGraphicsConfig();

    // Set-difference helper: elements in lhs that are not in rhs.
    ModuleSnapshot Subtract(const ModuleSnapshot& lhs, const ModuleSnapshot& rhs);

    // Print a labeled module list to stdout.
    void PrintList(const char* label, const ModuleSnapshot& items);

    // Provided by each platform's App.<Platform>.{cpp,mm}: true if the
    // current process is being traced/debugged. On Windows this wraps
    // ::IsDebuggerPresent(); on Linux it reads /proc/self/status; on macOS
    // it uses sysctl(KERN_PROC). All three are non-invasive.
    bool IsBeingTraced();

    // Shared preflight for main(). Prints a SKIP message and returns true
    // when the test should not run in the current environment (Debug config,
    // debugger attached). Returns false when the test should proceed.
    bool ShouldSkipEnvironment();

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
    // returns 0 on pass, 1 on fail.
    int CompareAndReport(const ModuleSnapshot& postBoot);
}
