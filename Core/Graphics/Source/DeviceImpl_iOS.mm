#include <cmath>
#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window)
    {
        pd.nwh = window;
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::PlatformData& /*pd*/, bgfx::RendererType::Enum& /*renderType*/)
    {
    }

    float DeviceImpl::GetDevicePixelRatio(WindowT window)
    {
        // contentScaleFactor can return infinity if the view is not yet parented
        // to a window hierarchy (and thus has no associated screen).
        // Fallback to the scale from the main screen.
        float scale = window.contentScaleFactor;
        if (std::isinf(scale) || scale <= 0)
        {
            scale = UIScreen.mainScreen.scale;
        }
        return scale;
    }
}
