#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

namespace Babylon::Graphics
{
    const bool DeviceImpl::s_bgfxFlipAfterRender = false;

    void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window)
    {
        pd.nwh = window;
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::PlatformData& pd, bgfx::RendererType::Enum& renderType)
    {
        // on Android, having no window or context set the renderer API to no op.
        if (!pd.nwh && !pd.context)
        {
            renderType = bgfx::RendererType::Noop;
        }
    }

    float DeviceImpl::GetDevicePixelRatio(WindowT)
    {
        // In Android, the baseline DPI is 160dpi.
        // See https://developer.android.com/training/multiscreen/screendensities#dips-pels
        auto dpi = android::global::GetAppContext().getResources().getConfiguration().getDensityDpi();
        return static_cast<float>(dpi) / 160.0f;
    }
}
