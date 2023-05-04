#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bool DeviceImpl::s_bgfxFlipAfterRender = true;

    void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window)
    {
        pd.nwh = window;
    }

    float DeviceImpl::GetDevicePixelRatio(WindowT window)
    {
        return window.window.screen.backingScaleFactor;
    }
}
