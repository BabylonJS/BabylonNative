#pragma once

#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/RendererType.h>

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
#include <bgfx/bgfx.h>
#include <optional>
#endif

namespace Babylon::Graphics
{
    // Query the device-pixel-ratio for `window`. Free function so callers
    // can obtain the ratio before a `Device` has been constructed (e.g.
    // an interop layer converting physical → logical pixels for
    // `Configuration::Width/Height` ahead of `Device::Device(config)`).
    //
    // For an existing Device, prefer `Device::GetDevicePixelRatio()` —
    // that's this function's last-`UpdateWindow` return cached on the Device.
    //
    // Lives in InternalInclude/ to keep it off the public surface;
    // reachable to in-tree consumers via the GraphicsDeviceContext target.
    float GetDevicePixelRatio(WindowT window);

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
    namespace D3D11TextureFormats
    {
        struct BgfxTextureFormat
        {
            bgfx::TextureFormat::Enum Format;
            bool Srgb;
        };

        std::optional<BgfxTextureFormat> TryGetBgfxTextureFormat(DXGI_FORMAT format);
    }
#endif
}
