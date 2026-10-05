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

    // Releases JS references and Dawn objects while the N-API environment is
    // still alive. Must run on the JS thread before AppRuntime is destroyed.
    void Deinitialize(Napi::Env env);

    // Reconfigures the Dawn surface (and cached drawing-buffer size) to the given
    // dimensions. Must be called on the JS thread (where the Dawn device lives),
    // e.g. from the app's per-frame dispatch when the native window has resized.
    // Does not touch the native window itself.
    void ResizeSurface(uint32_t width, uint32_t height);

    // Also updates the presentation canvas's client dimensions. Engine-specific
    // resize handling belongs to the host. Must be called on the JS thread.
    void ResizeSurface(Napi::Env env, uint32_t width, uint32_t height);

    // Pumps animation callbacks, deferred readback and GPU resource retirement.
    // Presents submitted surface work before starting the next frame. Call once
    // per native frame on the JS thread.
    void Tick(Napi::Env env);
}
