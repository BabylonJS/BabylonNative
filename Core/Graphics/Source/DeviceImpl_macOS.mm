#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

#include <Cocoa/Cocoa.h>
#include <QuartzCore/CAMetalLayer.h>
#include <Metal/Metal.h>

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
        NSObject* nvh = (NSObject*)window;
        
        if ([nvh isKindOfClass:[CAMetalLayer class]])
        {
            CAMetalLayer* metalLayer = (CAMetalLayer*)nvh;
            return metalLayer.contentsScale;
        }
        else if([nvh isKindOfClass:[NSView class]])
        {
            NSView* nsView = (NSView*)nvh;
            return nsView.window.screen.backingScaleFactor;
        }
        
        return 1.0f;
    }
}
