// gtest.h-style include ordering: X11 headers #define None (which collides
// with lots of C++ code) so they must come before anything that cares.
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#undef None

#include "App.h"
#include "ModuleSnapshot.h"

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/Device.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

namespace ModuleLoadTest
{
    // Parse /proc/self/status for a non-zero TracerPid. Non-invasive
    // (contrast with ptrace(PTRACE_TRACEME), which *creates* trace state
    // if none exists).
    bool IsBeingTraced()
    {
        std::ifstream status{"/proc/self/status"};
        std::string line;
        while (std::getline(status, line))
        {
            constexpr std::string_view prefix{"TracerPid:"};
            if (line.size() >= prefix.size() && line.compare(0, prefix.size(), prefix) == 0)
            {
                const char* p = line.c_str() + prefix.size();
                while (*p == ' ' || *p == '\t') ++p;
                return *p != '\0' && *p != '0';
            }
        }
        return false;
    }

    // See App.Win32.cpp for the full rationale of the asymmetric
    // (permissive superset) check.
    const ModuleSnapshot& GetExpectedBootModules()
    {
        // Seeded from CI on ubuntu-latest (Mesa software renderer via xvfb).
        // Append entries here as new configs surface deltas.
        static const ModuleSnapshot kModules{
            "libdrm.so.2",
            "libdrm_intel.so.1",
            "libedit.so.2",
            "libegl_mesa.so.0",
            "libelf.so.1",
            "libexpat.so.1",
            "libgbm.so.1",
            "libgldispatch.so.0",
            "libpciaccess.so.0",
            "libsensors.so.5",
            "libtinfo.so.6",
            "libwayland-client.so.0",
            "libx11-xcb.so.1",
            "libxcb-dri3.so.0",
            "libxcb-present.so.0",
            "libxcb-randr.so.0",
            "libxcb-shm.so.0",
            "libxcb-sync.so.1",
            "libxcb-xfixes.so.0",
            "libxml2.so.2",
            "libxshmfence.so.1",
        };
        return kModules;
    }

    // Name patterns for modules whose presence in the delta is allowed but
    // not required. On Linux the graphics stack varies a lot across runners:
    // Mesa DRI drivers, llvmpipe/swrast, NVIDIA/AMD userspace libs, Vulkan
    // ICD loaders, GL ABI variants. Seed a broad carve-out up front so the
    // golden list doesn't have to re-list every distro/GPU permutation.
    bool IsAllowedOptionalModule(std::string_view name)
    {
        static constexpr std::string_view kPrefixes[] = {
            // Mesa and DRI driver shims (swrast, llvmpipe, iris, radeonsi, ...)
            "libglapi",
            "libdri",
            "swrast",
            "llvmpipe",
            "iris",
            "radeonsi",
            "i965",
            "i915",
            "nouveau",
            "virtio",
            "vmwgfx",
            "kms",
            // NVIDIA proprietary
            "libnvidia",
            "libcuda",
            "libgl_nvidia",
            "libegl_nvidia",
            // AMD proprietary
            "libamdgpu",
            "libdrm_amdgpu",
            // Vulkan ICD loaders
            "libvulkan",
            // Mesa shared renderer shim (libgallium-<mesa>-<ubuntu>.so) and
            // the LLVM library it uses for shader compilation — both carry a
            // version suffix that drifts with Ubuntu image updates.
            "libgallium-",
            "libllvm.so.",
            // GLVND variants
            "libgl.",
            "libglx",
            "libegl.",
            "libgles",
            "libopengl",
        };
        for (const auto& prefix : kPrefixes)
        {
            if (name.size() >= prefix.size() && name.compare(0, prefix.size(), prefix) == 0)
            {
                return true;
            }
        }
        return false;
    }
    std::optional<Babylon::Graphics::Configuration> CreateGraphicsConfig()
    {
        constexpr const char* kApplicationName = "Babylon Native ModuleLoadTest";
        constexpr int kWidth = 640;
        constexpr int kHeight = 480;

        XInitThreads();

        // Display and Window parked in function-local static storage so they
        // live for the duration of the process; cleanup happens at process
        // exit.
        static Display* display = XOpenDisplay(nullptr);
        if (display == nullptr)
        {
            // Headless environments without an X display (e.g. local runs outside
            // xvfb-run) cannot bring up bgfx's GL backend. CI wraps this binary
            // in xvfb-run so the path below is the normal case there.
            std::cout << "ModuleLoadTest: SKIP - no X display (set DISPLAY or run "
                         "under xvfb-run)." << std::endl;
            return std::nullopt;
        }

        const int screen = DefaultScreen(display);
        const int depth = DefaultDepth(display, screen);
        Visual* visual = DefaultVisual(display, screen);
        const ::Window root = RootWindow(display, screen);

        // Mirror Apps/UnitTests/Source/App.X11.cpp exactly: explicit zero-init of
        // window attributes, explicit clear-to-black via XChangeWindowAttributes,
        // and the same XMapWindow -> XStoreName order. bgfx's GL/EGL backend is
        // sensitive to this sequencing under xvfb-run; deviations have been
        // observed to cause "Failed to create surface" aborts.
        XSetWindowAttributes windowAttrs;
        windowAttrs.background_pixel = 0;
        windowAttrs.background_pixmap = 0;
        windowAttrs.border_pixel = 0;
        windowAttrs.event_mask = 0;

        static const ::Window window = XCreateWindow(display, root, 0, 0, kWidth, kHeight, 0, depth,
            InputOutput, visual, CWBorderPixel | CWEventMask, &windowAttrs);

        // Clear window to black.
        XSetWindowAttributes attr;
        std::memset(&attr, 0, sizeof(attr));
        XChangeWindowAttributes(display, window, CWBackPixel, &attr);

        char wmDeleteWindowName[] = "WM_DELETE_WINDOW";
        char* wmDeleteWindowNames[] = {wmDeleteWindowName};
        Atom wmDeleteWindow;
        XInternAtoms(display, wmDeleteWindowNames, 1, False, &wmDeleteWindow);
        XSetWMProtocols(display, window, &wmDeleteWindow, 1);

        XMapWindow(display, window);
        XStoreName(display, window, kApplicationName);

        Babylon::DebugTrace::EnableDebugTrace(true);
        Babylon::DebugTrace::SetTraceOutput([](const char* trace) { std::printf("%s\n", trace); std::fflush(stdout); });

        Babylon::Graphics::Configuration config{};
        config.Window = window;
        config.Width = static_cast<size_t>(kWidth);
        config.Height = static_cast<size_t>(kHeight);
        return config;
    }
}
