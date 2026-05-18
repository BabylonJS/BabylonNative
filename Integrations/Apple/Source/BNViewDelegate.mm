// BNViewDelegate.mm — default `MTKViewDelegate` that drives a BNView.
// Subclassable by hosts that want to insert per-frame work; just
// override the delegate methods and call `super` to keep the default
// forwarding behavior.

#import <Babylon/Integrations/Apple/BNView.h>

#include <cmath>

namespace
{
    bool IsFinitePositive(CGFloat value)
    {
        return std::isfinite(static_cast<double>(value)) && value > 0;
    }
}

@implementation BNViewDelegate
{
    // BNView retains the auto-installed delegate strongly; host
    // subclasses are typically held by the host's controller. Weak
    // is sufficient and avoids retain cycles if a subclass captures
    // the BNView.
    __weak BNView* _view;
}

- (instancetype)initWithView:(BNView*)view
{
    if (view == nil)
    {
        return nil;
    }
    if ((self = [super init]))
    {
        _view = view;
    }
    return self;
}

#pragma mark - MTKViewDelegate

- (void)mtkView:(MTKView* __unused)v drawableSizeWillChange:(CGSize)size
{
    if (!IsFinitePositive(size.width) || !IsFinitePositive(size.height))
    {
        return;
    }

    [_view resizeWithWidth:static_cast<NSUInteger>(size.width)
                    height:static_cast<NSUInteger>(size.height)];
}

- (void)drawInMTKView:(MTKView* __unused)v
{
    // [_view renderFrame] handles the XR overlay visibility toggle, so
    // subclasses that override this and call super get it for free.
    [_view renderFrame];
}

@end
