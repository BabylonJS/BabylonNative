#include <cmath>
#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

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
        float scale = static_cast<float>(((CAMetalLayer*)window).contentsScale);
        if (std::isinf(scale) || scale <= 0)
        {
            scale = NSScreen.mainScreen.backingScaleFactor;
        }
        return scale;
    }
}
