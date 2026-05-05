#include "RuntimeImpl.h"

#import <QuartzCore/CAMetalLayer.h>
#import <UIKit/UIKit.h>

#include <algorithm>

namespace Babylon::Integrations
{
    std::pair<uint32_t, uint32_t> ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        if (window == nullptr)
        {
            return {0, 0};
        }

        CAMetalLayer* layer = (__bridge CAMetalLayer*)window;

        // CAMetalLayer's hosting UIView is the layer's delegate. If
        // the host added it with Auto Layout and called us before the
        // layout pass ran, force layout now so the layer has valid
        // bounds.
        UIView* hostView = nil;
        if ([layer.delegate isKindOfClass:[UIView class]])
        {
            hostView = (UIView*)layer.delegate;
            if (hostView.bounds.size.width <= 0 || hostView.bounds.size.height <= 0)
            {
                [hostView layoutIfNeeded];
            }
        }

        CGSize size = layer.drawableSize;

        // MTKView's autoResizeDrawable updates drawableSize lazily —
        // typically not until the first display tick. Fall back to
        // the layer's own backing-pixel size, then seed drawableSize
        // so bgfx and CAMetalLayer agree from now on.
        if (size.width <= 0 || size.height <= 0)
        {
            size.width = layer.bounds.size.width * layer.contentsScale;
            size.height = layer.bounds.size.height * layer.contentsScale;
            if (size.width > 0 && size.height > 0)
            {
                layer.drawableSize = size;
            }
        }

        const auto width = static_cast<uint32_t>(std::max<CGFloat>(size.width, 1));
        const auto height = static_cast<uint32_t>(std::max<CGFloat>(size.height, 1));
        return {width, height};
    }

    std::pair<float, float> ViewImpl::ToLogicalCoords(float x, float y) const
    {
        // visionOS pointer interactions arrive through SwiftUI/RealityKit
        // gesture recognizers in logical points — passthrough.
        return {x, y};
    }
}
