#include <Babylon/GraphicsPlatform.h>
#include "GraphicsImpl.h"

#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

namespace Babylon
{
    const bool GraphicsImpl::s_bgfxFlipAfterRender = false;

    void GraphicsImpl::ConfigureBgfxPlatformData(const WindowConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.nwh = config.Window;
    }

    float GraphicsImpl::GetDevicePixelRatio(const WindowConfiguration&)
    {
        // In Android, the baseline DPI is 160dpi.
        // See https://developer.android.com/training/multiscreen/screendensities#dips-pels
        auto dpi = android::global::GetAppContext().getResources().getConfiguration().getDensityDpi();
        return static_cast<float>(dpi) / 160.0f;
    }
}
