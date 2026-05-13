#include "RuntimeImpl.h"

#import <QuartzCore/CAMetalLayer.h>

namespace Babylon::Integrations
{
    ViewImpl::QuerySizeResult ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        if (window == nullptr)
        {
            return {0, 0, CoordinateUnits::Physical};
        }
        // metal-cpp's CA::MetalLayer* can be bridge-cast to the Obj-C
        // CAMetalLayer*; drawableSize is in physical pixels.
        CAMetalLayer* layer = (__bridge CAMetalLayer*)window;
        const CGSize size = layer.drawableSize;
        return {static_cast<uint32_t>(size.width),
                static_cast<uint32_t>(size.height),
                CoordinateUnits::Physical};
    }
}
