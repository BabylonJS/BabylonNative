#include "RuntimeImpl.h"

#import <QuartzCore/CAMetalLayer.h>

namespace Babylon::Integrations
{
    std::pair<uint32_t, uint32_t> ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        if (window == nullptr)
        {
            return {0, 0};
        }
        // metal-cpp's CA::MetalLayer* can be bridge-cast to the Obj-C
        // CAMetalLayer*; drawableSize is in physical pixels.
        CAMetalLayer* layer = (__bridge CAMetalLayer*)window;
        const CGSize size = layer.drawableSize;
        return {static_cast<uint32_t>(size.width),
                static_cast<uint32_t>(size.height)};
    }

    std::pair<float, float> ViewImpl::ToLogicalCoords(float x, float y) const
    {
        // UIKit `UITouch.locationInView` is already in logical points —
        // passthrough.
        return {x, y};
    }
}
