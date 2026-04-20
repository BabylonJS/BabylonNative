#pragma once

#include <set>
#include <string>

namespace ModuleLoadTest
{
    // Loaded-module snapshot. Base names only (no paths, lower-cased on
    // case-insensitive platforms) for stable comparison across OS patch
    // versions and symlink differences.
    using ModuleSnapshot = std::set<std::string>;

    // Enumerate modules currently loaded into the process.
    ModuleSnapshot CaptureSnapshot();

    // Return the baseline captured BEFORE any C++ static initializer in this
    // binary has run. This is what lets the delta catch modules loaded by
    // bx-style `static` objects (e.g. dbghelp.dll via bx's debug.cpp), which
    // would otherwise already be in a main()-entry baseline.
    const ModuleSnapshot& GetPreInitBaseline();
}
