#include "ModuleSnapshot.h"

#include <Windows.h>
#include <Psapi.h>
#include <Shlwapi.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <vector>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shlwapi.lib")

// Register a TLS callback so we capture the module list BEFORE any C++ static
// initializer in this binary runs. The linker would normally drop unreferenced
// TLS directory entries, so we force the symbols in.
#ifdef _WIN64
    #pragma comment(linker, "/INCLUDE:_tls_used")
    #pragma comment(linker, "/INCLUDE:p_module_load_test_tls_cb")
#else
    #pragma comment(linker, "/INCLUDE:__tls_used")
    #pragma comment(linker, "/INCLUDE:_p_module_load_test_tls_cb")
#endif

extern "C" void NTAPI ModuleLoadTest_OnTlsCallback(PVOID dllHandle, DWORD reason, PVOID reserved);

extern "C"
{
    #pragma const_seg(push)
    #pragma const_seg(".CRT$XLB")
    extern const PIMAGE_TLS_CALLBACK p_module_load_test_tls_cb;
    const PIMAGE_TLS_CALLBACK p_module_load_test_tls_cb = ModuleLoadTest_OnTlsCallback;
    #pragma const_seg(pop)
}

namespace ModuleLoadTest
{
    namespace
    {
        std::string ToLower(std::string value)
        {
            std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            return value;
        }

        std::string WideToUtf8(const wchar_t* wide)
        {
            if (wide == nullptr || *wide == L'\0')
            {
                return {};
            }

            const int size = ::WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
            if (size <= 0)
            {
                return {};
            }

            std::string result(static_cast<size_t>(size), '\0');
            const int converted = ::WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), size, nullptr, nullptr);
            if (converted <= 0)
            {
                return {};
            }
            result.resize(static_cast<size_t>(converted - 1));
            return result;
        }
    }

    namespace
    {
        // Zero-initialized in the PE .bss — valid before any code in this
        // binary runs. The TLS callback below allocates the snapshot before
        // any C++ static initializer fires.
        ModuleSnapshot* g_preInitSnapshot = nullptr;
    }

    const ModuleSnapshot& GetPreInitBaseline()
    {
        static const ModuleSnapshot empty{};
        return g_preInitSnapshot != nullptr ? *g_preInitSnapshot : empty;
    }

    // Called from the TLS callback. Defined in this TU so the anonymous
    // g_preInitSnapshot is reachable.
    void CapturePreInitSnapshot()
    {
        if (g_preInitSnapshot == nullptr)
        {
            // At this point the exe's import DLLs (kernel32, Psapi, Shlwapi,
            // ucrtbase) are mapped and the process heap is available, so
            // new/std::set/std::string are safe to use even though the rest of
            // the CRT has not fully initialized.
            g_preInitSnapshot = new ModuleSnapshot(CaptureSnapshot());
        }
    }

    void DisposePreInitSnapshot()
    {
        delete g_preInitSnapshot;
        g_preInitSnapshot = nullptr;
    }

    ModuleSnapshot CaptureSnapshot()
    {
        ModuleSnapshot snapshot{};

        const HANDLE process = ::GetCurrentProcess();

        // Large up-front buffer avoids the documented race in the
        // two-call sizing pattern: EnumProcessModules "may fail or return
        // incorrect information" if the module list changes during the
        // call. The pre-init + post-boot module sets are on the order of
        // 50-150 entries on realistic SKUs, so 512 gives ample headroom.
        constexpr size_t kMaxModules = 512;
        std::vector<HMODULE> modules(kMaxModules);

        DWORD requiredBytes = 0;
        if (!::EnumProcessModules(process, modules.data(),
                static_cast<DWORD>(modules.size() * sizeof(HMODULE)), &requiredBytes))
        {
            std::fprintf(stderr,
                "ModuleLoadTest FAIL: EnumProcessModules failed (error %lu).\n",
                ::GetLastError());
            ::ExitProcess(1);
        }

        const size_t count = requiredBytes / sizeof(HMODULE);
        if (count > modules.size())
        {
            // Silent truncation would hide exactly the kind of regression
            // this test exists to catch. Bail loudly instead.
            std::fprintf(stderr,
                "ModuleLoadTest FAIL: module buffer too small (need %zu, have %zu). "
                "Increase kMaxModules in ModuleSnapshot.Win32.cpp.\n",
                count, modules.size());
            ::ExitProcess(1);
        }

        for (size_t i = 0; i < count; ++i)
        {
            wchar_t path[MAX_PATH]{};
            const DWORD length = ::GetModuleFileNameW(modules[i], path, static_cast<DWORD>(std::size(path)));
            if (length == 0 || length >= std::size(path))
            {
                continue;
            }

            const wchar_t* baseName = ::PathFindFileNameW(path);
            if (baseName == nullptr || *baseName == L'\0')
            {
                continue;
            }

            snapshot.insert(ToLower(WideToUtf8(baseName)));
        }

        return snapshot;
    }
}

extern "C" void NTAPI ModuleLoadTest_OnTlsCallback(PVOID /*dllHandle*/, DWORD reason, PVOID /*reserved*/)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        ModuleLoadTest::CapturePreInitSnapshot();
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        ModuleLoadTest::DisposePreInitSnapshot();
    }
}
