#include <Babylon/GraphicsPlatform.h>
#include "GraphicsImpl.h"
#include <Windows.h>

namespace Babylon
{
    const bool GraphicsImpl::s_bgfxFlipAfterRender = false;

    void GraphicsImpl::ConfigureBgfxPlatformData(const WindowConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.nwh = config.Window;
    }

    float GraphicsImpl::GetDevicePixelRatio(const WindowConfiguration& config)
    {
        UINT dpi{GetDpiForWindow(config.Window)};

        // In windows, 100% DPI scaling is 96dpi.
        // See https://docs.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels
        return static_cast<float>(dpi) / 96.0f;
    }
}
