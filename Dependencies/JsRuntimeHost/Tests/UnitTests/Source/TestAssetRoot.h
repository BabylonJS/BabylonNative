#pragma once

#include <filesystem>
#include <string>
#include <system_error>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

// Worker scripts resolve against a filesystem ScriptRoot. The test assets are staged next to
// the executable (see the UnitTests CMakeLists), which is not necessarily the working
// directory: an iOS app starts at "/", and launching the binary from the repository root
// makes every worker script "unable to load".
inline std::filesystem::path TestAssetRoot()
{
#if defined(__APPLE__)
    uint32_t size{0};
    _NSGetExecutablePath(nullptr, &size);
    std::string path(size, '\0');
    if (_NSGetExecutablePath(path.data(), &size) == 0)
    {
        return std::filesystem::weakly_canonical(std::filesystem::path{path.c_str()}).parent_path();
    }
#elif defined(_WIN32)
    // Not _get_wpgmptr: the CRT only fills _wpgmptr for wide entry points and fast-fails
    // (STATUS_STACK_BUFFER_OVERRUN) when it is asked for the uninitialized one.
    wchar_t path[MAX_PATH]{};
    const auto length = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (length > 0 && length < MAX_PATH)
    {
        return std::filesystem::path{path}.parent_path();
    }
#elif defined(__linux__) && !defined(__ANDROID__)
    std::error_code error;
    const auto path = std::filesystem::read_symlink("/proc/self/exe", error);
    if (!error)
    {
        return path.parent_path();
    }
#endif
    return std::filesystem::current_path();
}
