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

        // metal-cpp's CA::MetalLayer* can be bridge-cast to the Obj-C
        // CAMetalLayer*.
        CAMetalLayer* layer = (__bridge CAMetalLayer*)window;

        // CAMetalLayer's hosting UIView (e.g. MTKView) is registered as
        // the layer's delegate. If the host added it with Auto Layout
        // and called us before the layout pass ran, force layout now so
        // the layer has valid bounds. This keeps host code free of
        // "remember to call layoutIfNeeded before constructing BNView"
        // boilerplate.
        UIView* hostView = nil;
        if ([layer.delegate isKindOfClass:[UIView class]])
        {
            hostView = (UIView*)layer.delegate;
            if (hostView.bounds.size.width <= 0 || hostView.bounds.size.height <= 0)
            {
                [hostView layoutIfNeeded];
            }
        }

        // Prefer the explicit drawableSize when the host has set it
        // (e.g. the off-screen prewarm pattern in SimplifiedAPI §5).
        CGSize size = layer.drawableSize;

        // MTKView's autoResizeDrawable updates drawableSize lazily —
        // typically not until the first display tick — so it can still
        // be (0, 0) here even when the layer is fully laid out. Fall
        // back to the layer's own backing-pixel size, then seed
        // drawableSize so bgfx and CAMetalLayer agree from now on.
        if (size.width <= 0 || size.height <= 0)
        {
            size.width = layer.bounds.size.width * layer.contentsScale;
            size.height = layer.bounds.size.height * layer.contentsScale;
            if (size.width > 0 && size.height > 0)
            {
                layer.drawableSize = size;
            }
        }

        // Last-resort clamp: never hand bgfx (0, 0). If the host gave
        // us a layer with no geometry yet, render at 1x1 until the
        // host's resize callback (e.g. `MTKViewDelegate
        // mtkView:drawableSizeWillChange:`) provides a real size.
        const auto width = static_cast<uint32_t>(std::max<CGFloat>(size.width, 1));
        const auto height = static_cast<uint32_t>(std::max<CGFloat>(size.height, 1));
        return {width, height};
    }

    std::pair<float, float> ViewImpl::ToLogicalCoords(float x, float y) const
    {
        // UIKit `UITouch.locationInView` is already in logical points —
        // passthrough.
        return {x, y};
    }
}
