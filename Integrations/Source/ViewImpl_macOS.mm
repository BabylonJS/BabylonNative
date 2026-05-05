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
        CAMetalLayer* layer = (__bridge CAMetalLayer*)window;
        const CGSize size = layer.drawableSize;
        return {static_cast<uint32_t>(size.width),
                static_cast<uint32_t>(size.height)};
    }

    std::pair<float, float> ViewImpl::ToLogicalCoords(float x, float y) const
    {
        // AppKit `NSEvent.locationInWindow` is in logical points — passthrough.
        return {x, y};
    }
}
