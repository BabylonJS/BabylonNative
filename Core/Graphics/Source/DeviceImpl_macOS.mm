#include <cmath>
#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/DeviceQueries.h>
#include "DeviceImpl.h"

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

namespace Babylon::Graphics
{
    void DeviceImpl::ConfigureBgfxSwapChain(bgfx::SwapChain& swapChain, WindowT window)
    {
        swapChain.nwh = window;
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::Init& /*init*/)
    {
    }

    float GetDevicePixelRatio(WindowT window)
    {
        float scale = static_cast<float>(((CAMetalLayer*)window).contentsScale);
        if (std::isinf(scale) || scale <= 0)
        {
            scale = NSScreen.mainScreen.backingScaleFactor;
        }
        return scale;
    }
}
