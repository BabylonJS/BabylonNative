#pragma once

#include <Babylon/Graphics/Platform.h>

namespace Babylon::Graphics
{
    // Query the screen's device-pixel-ratio for the given native
    // window. Free function so callers can obtain the ratio before a
    // `Device` has been constructed (e.g. an interop layer converting
    // physical → logical pixels for `Configuration::Width/Height`
    // ahead of `Device::Device(config)`).
    //
    // For an existing `Device`, prefer `Device::GetDevicePixelRatio()`
    // — that's the value this function returned when `UpdateWindow`
    // was last called, cached on the `Device`.
    //
    // Visibility:
    //   - Declared in `InternalInclude/Babylon/Graphics/` rather than
    //     in the public `Device.h` because the implementation depends
    //     on platform-specific window types that are also internal
    //     (CAMetalLayer, ANativeWindow, HWND, X11 `Window`, etc.) and
    //     because most cross-platform host code should just use the
    //     `Device::GetDevicePixelRatio()` instance method.
    //   - Reachable to consumers of the `GraphicsDeviceContext` target
    //     (plugins, the Integrations facade). NOT reachable to plain
    //     `GraphicsDevice` consumers.
    float GetDevicePixelRatio(WindowT window);
}
