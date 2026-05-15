#include "RuntimeImpl.h"

#include <cmath>

#import <QuartzCore/CAMetalLayer.h>

namespace
{
    bool IsFinitePositive(CGFloat value)
    {
        return std::isfinite(static_cast<double>(value)) && value > 0;
    }
}

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
        if (!IsFinitePositive(size.width) || !IsFinitePositive(size.height))
        {
            return {0, 0, CoordinateUnits::Physical};
        }
        return {static_cast<uint32_t>(size.width),
                static_cast<uint32_t>(size.height),
                CoordinateUnits::Physical};
    }
}
