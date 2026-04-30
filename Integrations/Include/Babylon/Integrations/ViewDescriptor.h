#pragma once

#include <cstdint>

namespace Babylon::Integrations
{
    // Description of a platform surface that a `View` will render
    // into. Populated by the platform interop layer (or directly by a
    // C++ host on Win32 / Linux) from whatever native object the
    // host's UI framework provides:
    //
    //   Win32                : HWND
    //   Linux (X11)          : Window  (X11 XID; reinterpret_cast through void*)
    //   Android              : ANativeWindow*
    //   iOS / macOS / visionOS : CAMetalLayer*
    //   UWP                  : IInspectable*  (e.g. SwapChainPanel)
    //
    // `width` and `height` are in **logical pixels**; `devicePixelRatio`
    // is the physical-to-logical ratio (e.g. 2.0 for a Retina display).
    // The platform interop layer (Integrations/Android, Integrations/Apple,
    // Integrations/Uwp) is responsible for converting whatever its UI
    // framework hands the host into this convention.
    struct ViewDescriptor
    {
        void* nativeWindow{nullptr};
        uint32_t width{0};
        uint32_t height{0};
        float devicePixelRatio{1.0f};
    };
}
