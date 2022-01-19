#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bool DeviceImpl::s_bgfxFlipAfterRender = true;

    void DeviceImpl::ConfigureBgfxPlatformData(const WindowConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.nwh = config.WindowPtr;
    }

    float DeviceImpl::GetDevicePixelRatio(const WindowConfiguration& config)
    {
        return config.WindowPtr.contentScaleFactor;
    }
}
