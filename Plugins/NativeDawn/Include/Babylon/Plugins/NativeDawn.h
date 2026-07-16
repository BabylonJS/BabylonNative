#pragma once

#include <napi/napi.h>

#include <cstdint>

namespace Babylon::Plugins::NativeDawn
{
    // Initializes the NativeDawn plugin: creates a Dawn (WebGPU) instance,
    // adapter, device and a surface bound to the given native window, then
    // installs `navigator.gpu` and the WebGPU global objects into `env`.
    //
    // This is the clean-room replacement for the bgfx NativeEngine path: all
    // rendering goes through Dawn, no bgfx. Win32 only for now (`window` is an
    // HWND).
    void Initialize(Napi::Env env, void* window, uint32_t width, uint32_t height);

    // Reconfigures the Dawn surface (and cached drawing-buffer size) to the given
    // dimensions. Must be called on the JS thread (where the Dawn device lives),
    // e.g. from the app's per-frame dispatch when the native window has resized.
    // Does not touch the native window itself.
    void ResizeSurface(uint32_t width, uint32_t height);

    // Drives one host-side frame: ticks the Dawn device (callbacks) and, if the
    // JS side has rendered into the current surface texture, presents it. Call
    // once per native frame from the app loop.
    void Tick(Napi::Env env);
}
