#include <cmath>
#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>

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
        // contentsScale can return 0 if it hasn't been set yet.
        // Fallback to the scale from the main screen.
        float scale = static_cast<float>(((CAMetalLayer*)window).contentsScale);
        if (std::isinf(scale) || scale <= 0)
        {
            scale = UIScreen.mainScreen.scale;
        }
        return scale;
    }
}
