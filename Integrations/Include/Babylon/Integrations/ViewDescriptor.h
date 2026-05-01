#pragma once

#include <Babylon/Graphics/Platform.h>

#include <cstdint>

namespace Babylon::Integrations
{
    // Description of a platform surface that a `View` will render
    // into. Populated by the platform interop layer (or directly by a
    // C++ host on Win32 / Linux) from whatever native object the
    // host's UI framework provides.
    //
    // `nativeWindow` is `Babylon::Graphics::WindowT`, which is the
    // platform-specific native window handle that `Babylon::Graphics`
    // already understands:
    //
    //   Win32        : HWND
    //   Android      : ANativeWindow*
    //   iOS / macOS / visionOS : CA::MetalLayer*  (metal-cpp wrapper)
    //   X11 (Linux)  : Window  (X11 XID — `unsigned long`)
    //   UWP / WinRT  : winrt::Windows::Foundation::IInspectable
    //                  (e.g. ICoreWindow, ISwapChainPanel)
    //
    // `width` and `height` are in **logical pixels**; `devicePixelRatio`
    // is the physical-to-logical ratio (e.g. 2.0 for a Retina display).
    // The platform interop layer (Integrations/Android, Integrations/Apple)
    // is responsible for converting whatever its UI framework hands the
    // host into this convention.
    struct ViewDescriptor
    {
        Babylon::Graphics::WindowT nativeWindow{};
        uint32_t width{0};
        uint32_t height{0};
        float devicePixelRatio{1.0f};
    };
}
