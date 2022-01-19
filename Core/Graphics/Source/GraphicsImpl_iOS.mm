#include <Babylon/GraphicsPlatform.h>
#include "GraphicsImpl.h"

namespace Babylon
{
    const bool GraphicsImpl::s_bgfxFlipAfterRender = true;

    void GraphicsImpl::ConfigureBgfxPlatformData(const WindowConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.nwh = config.Window;
    }

    float GraphicsImpl::GetDevicePixelRatio(const WindowConfiguration& config)
    {
        return config.Window.contentScaleFactor;
    }
}
