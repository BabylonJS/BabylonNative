#include "App.h"
#include "ModuleSnapshot.h"

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/Device.h>

#include <Windows.h>

#include <iostream>
#include <string_view>

namespace ModuleLoadTest
{
    namespace
    {
        LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            return ::DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }

    bool IsBeingTraced()
    {
        return ::IsDebuggerPresent() != FALSE;
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
    const ModuleSnapshot& GetExpectedBootModules()
    {
        // Seeded from a local RelWithDebInfo run on Windows 11 x64 with D3D11
        // and Chakra. CI may add more entries for other Win32 configs
        // (V8/JSI/D3D12) — those should be appended as the draft PR runs.
        static const ModuleSnapshot kModules{
            "bcryptprimitives.dll",
            "cfgmgr32.dll",
            "crypt32.dll",
            "cryptbase.dll",
            "cryptnet.dll",
            "d3d10warp.dll",
            "d3d11.dll",
            "d3d11_3sdklayers.dll",
            "d3d12.dll",
            "d3d12core.dll",
            "d3d12sdklayers.dll",
            "d3dscache.dll",
            // TODO: bgfx loads dbghelp.dll at boot (callstack/crash helper). Drop this
            // entry once bgfx stops pulling it in.
            "dbghelp.dll",
            "dcomp.dll",
            "devobj.dll",
            "directxdatabasehelper.dll",
            "drvstore.dll",
            "dwmapi.dll",
            "dxcore.dll",
            "dxgi.dll",
            "dxgidebug.dll",
            "dxilconv.dll",
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
            "userenv.dll",
            "uxtheme.dll",
            "version.dll",
            "windows.storage.dll",
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
    std::optional<Babylon::Graphics::Configuration> CreateGraphicsConfig()
    {
        ::SetConsoleOutputCP(CP_UTF8);

        // bgfx D3D12 implementation requires an HWND to avoid a device refcount
        // leak on shutdown. Create a hidden window to satisfy that requirement.
        // Parked in function-local static storage so the handle lives for the
        // duration of the process.
        static WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
            ::GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
            "BabylonNativeModuleLoadTest", nullptr };
        ::RegisterClassEx(&wc);
        static HWND hWnd = ::CreateWindow(wc.lpszClassName, "BabylonNativeModuleLoadTest",
            WS_OVERLAPPEDWINDOW, -1, -1, -1, -1, nullptr, nullptr, wc.hInstance, nullptr);

        Babylon::DebugTrace::EnableDebugTrace(true);
        Babylon::DebugTrace::SetTraceOutput([](const char* trace) {
            ::OutputDebugStringA(trace);
            ::OutputDebugStringA("\n");
        });

        Babylon::Graphics::Configuration config{};
        config.Window = hWnd;
        config.Width = 600;
        config.Height = 400;
        return config;
    }
}
