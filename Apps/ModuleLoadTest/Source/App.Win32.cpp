#include "App.h"
#include "ModuleSnapshot.h"

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/Device.h>

#include <Windows.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <string_view>

namespace
{
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

    ModuleLoadTest::ModuleSnapshot Subtract(const ModuleLoadTest::ModuleSnapshot& lhs, const ModuleLoadTest::ModuleSnapshot& rhs)
    {
        ModuleLoadTest::ModuleSnapshot result;
        std::set_difference(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
            std::inserter(result, result.end()));
        return result;
    }

    void PrintList(const char* label, const ModuleLoadTest::ModuleSnapshot& items)
    {
        std::cout << label << " (" << items.size() << "):" << std::endl;
        for (const auto& item : items)
        {
            std::cout << "  " << item << std::endl;
        }
    }

    // Expected set of modules loaded during BabylonNative boot, as a delta
    // from the baseline snapshot captured by the TLS callback before any C++
    // static initializer in this binary has run. Base names only, lower case.
    //
    // This list targets optimized builds (Release and RelWithDebInfo, which
    // produce identical module sets). Debug builds load additional debug CRT
    // and diagnostic DLLs and are not supported — main() returns a SKIP in
    // that config. CI runs only in RelWithDebInfo (see CMakeLists.txt's
    // `add_test ... CONFIGURATIONS Release RelWithDebInfo`).
    //
    // Launch-environment noise (e.g. VS's Start Without Debugging injecting
    // kernel.appcore.dll) is filtered via IsAllowedOptionalModule so devs
    // see the same verdict from Ctrl-F5 as CI does from a plain cmd launch.
    //
    // The assertion is asymmetric: we FAIL on modules not in this list (a new
    // module was pulled in — the regression signal we want), and we IGNORE
    // modules in this list that did not load (environmental variance across
    // GPU SKUs, Windows patch levels, VS vs cmd launch, etc.). This lets the
    // list be a permissive superset that works on both dev machines and CI.
    //
    // Motivating example: a dependency quietly pulling in dbghelp.dll on boot.
    // dbghelp.dll lives in System32, so a path-based OS filter would miss it;
    // we therefore golden-list the full delta (including OS modules) and only
    // allow a narrow name-pattern carve-out for GPU driver ICDs whose exact
    // names differ per runner SKU (see IsAllowedOptionalModule).
    const ModuleLoadTest::ModuleSnapshot& GetExpectedBootModules()
    {
        // Seeded from a local RelWithDebInfo run on Windows 11 x64 with D3D11
        // and Chakra. CI may add more entries for other Win32 configs
        // (V8/JSI/D3D12) — those should be appended as the draft PR runs.
        static const ModuleLoadTest::ModuleSnapshot kModules{
            "cfgmgr32.dll",
            "crypt32.dll",
            "cryptbase.dll",
            "cryptnet.dll",
            "d3d11.dll",
            "d3d11_3sdklayers.dll",
            "dbghelp.dll",
            "dcomp.dll",
            "devobj.dll",
            "directxdatabasehelper.dll",
            "drvstore.dll",
            "dwmapi.dll",
            "dxcore.dll",
            "dxgi.dll",
            "dxgidebug.dll",
            "iertutil.dll",
            "imagehlp.dll",
            "msasn1.dll",
            "msctf.dll",
            "netutils.dll",
            "ntmarta.dll",
            "powrprof.dll",
            "profapi.dll",
            "rsaenh.dll",
            "setupapi.dll",
            "shcore.dll",
            "shell32.dll",
            "srvcli.dll",
            "umpdc.dll",
            "uxtheme.dll",
            "version.dll",
            "winmm.dll",
            "wintrust.dll",
            "wintypes.dll",
            "wldp.dll",
        };
        return kModules;
    }

    // Name patterns for modules whose presence in the delta is allowed but
    // not required. Covers two classes:
    //
    //   * GPU driver ICDs, whose exact base name depends on which GPU the
    //     runner has (NVIDIA/Intel/AMD/ATI).
    //   * Environmental/ambient DLLs that some launchers (e.g. Visual Studio's
    //     Start Without Debugging) inject into the process but which a plain
    //     cmd.exe launch does not. These are not introduced by BabylonNative.
    //
    // Prefixes match at the start of the base name; exacts match the whole
    // name.
    bool IsAllowedOptionalModule(std::string_view name)
    {
        static constexpr std::string_view kPrefixes[] = {
            // GPU driver ICDs
            "nv",    // NVIDIA (nvoglv64.dll, nvapi64.dll, nvd3dum.dll, ...)
            "ig",    // Intel (igdumd64.dll, igxelpicd64.dll, ...)
            "amd",   // AMD (amdvlk64.dll, amdxc64.dll, ...)
            "atio",  // AMD/ATI (atio6axx.dll, ...)
            // Ambient WARP variants
            "microsoft.internal.warppal",
        };
        static constexpr std::string_view kExact[] = {
            // GPU driver ICD
            "dxil.dll",
            // VS Start-Without-Debugging launch environment
            "kernel.appcore.dll",
        };
        for (const auto& prefix : kPrefixes)
        {
            if (name.size() >= prefix.size() && name.compare(0, prefix.size(), prefix) == 0)
            {
                return true;
            }
        }
        for (const auto& exact : kExact)
        {
            if (name == exact)
            {
                return true;
            }
        }
        return false;
    }
}

int main(int /*argc*/, char* /*argv*/[])
{
#if defined(_DEBUG)
    // Debug builds load a different set of modules than RelWithDebInfo (debug
    // CRT, heavier diagnostic DLLs, etc). The golden list in this file
    // targets RelWithDebInfo only. Rather than produce a confusing FAIL, make
    // the skip explicit.
    std::cout << "ModuleLoadTest: SKIP - Debug config is not supported. "
                 "Build with Release or RelWithDebInfo." << std::endl;
    return 0;
#else
    // Running under a debugger injects additional modules (and changes some
    // timing) that would cause spurious FAIL diagnostics. CI runs headless
    // from the CLI, so this only affects local debugging. Non-debugger
    // ambient modules (e.g. VS Ctrl-F5) are handled by IsAllowedOptionalModule.
    if (::IsDebuggerPresent())
    {
        std::cout << "ModuleLoadTest: SKIP - running under a debugger. "
                     "Launch ModuleLoadTest.exe directly (no debugger attached) "
                     "to exercise the full assertion." << std::endl;
        return 0;
    }

    // Baseline was captured by a TLS callback before any C++ static
    // initializer in this binary ran, so modules loaded by bx-style static
    // objects (e.g. dbghelp.dll from bx's DbgHelpSymbolResolve) appear in the
    // delta below rather than in the baseline.
    const ModuleLoadTest::ModuleSnapshot& baseline = ModuleLoadTest::GetPreInitBaseline();

    ::SetConsoleOutputCP(CP_UTF8);

    // bgfx D3D12 implementation requires an HWND to avoid a device refcount
    // leak on shutdown. Create a hidden window to satisfy that requirement.
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
        ::GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
        "BabylonNativeModuleLoadTest", nullptr };
    ::RegisterClassEx(&wc);
    HWND hWnd = ::CreateWindow(wc.lpszClassName, "BabylonNativeModuleLoadTest",
        WS_OVERLAPPEDWINDOW, -1, -1, -1, -1, nullptr, nullptr, wc.hInstance, nullptr);

    Babylon::Graphics::Configuration config{};
    config.Window = hWnd;
    config.Width = 600;
    config.Height = 400;

    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) {
        ::OutputDebugStringA(trace);
        ::OutputDebugStringA("\n");
    });

    const ModuleLoadTest::ModuleSnapshot postBoot = ModuleLoadTest::RunBoot(config);

    // Delta = what boot caused to load. Filter out GPU ICD and
    // launch-environment noise (see IsAllowedOptionalModule).
    ModuleLoadTest::ModuleSnapshot delta = Subtract(postBoot, baseline);
    for (auto it = delta.begin(); it != delta.end();)
    {
        if (IsAllowedOptionalModule(*it))
        {
            it = delta.erase(it);
        }
        else
        {
            ++it;
        }
    }

    const ModuleLoadTest::ModuleSnapshot& expected = GetExpectedBootModules();

    // Only fail on NEW modules. Missing-from-delta is environmental variance
    // (GPU SKU, Windows patch, launch env, config) and is not a regression.
    const ModuleLoadTest::ModuleSnapshot unexpected = Subtract(delta, expected);

    // Always print both snapshots so CI logs can be used to extend the golden
    // list. The actual pass/fail is decided by the `unexpected` diff below.
    PrintList("Baseline (modules loaded before C++ static init)", baseline);
    PrintList("Delta (modules loaded during BN boot)", delta);

    if (!unexpected.empty())
    {
        std::cout << std::endl;
        std::cout << "FAIL: ModuleLoadTest detected unexpected modules loaded on boot." << std::endl;
        PrintList("Unexpected new modules", unexpected);
        std::cout << std::endl;
        std::cout << "If these are intentional, add them to GetExpectedBootModules() "
                     "in Apps/ModuleLoadTest/Source/App.Win32.cpp." << std::endl;
        return 1;
    }

    std::cout << "ModuleLoadTest: PASS" << std::endl;
    return 0;
#endif
}
