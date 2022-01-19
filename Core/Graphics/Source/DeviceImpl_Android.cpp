#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

namespace Babylon::Graphics
{
    const bool DeviceImpl::s_bgfxFlipAfterRender = false;

    void DeviceImpl::ConfigureBgfxPlatformData(const WindowConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.nwh = config.Window;
    }

    float DeviceImpl::GetDevicePixelRatio(const WindowConfiguration&)
    {
        // In Android, the baseline DPI is 160dpi.
        // See https://developer.android.com/training/multiscreen/screendensities#dips-pels
        auto dpi = android::global::GetAppContext().getResources().getConfiguration().getDensityDpi();
        return static_cast<float>(dpi) / 160.0f;
    }
}
