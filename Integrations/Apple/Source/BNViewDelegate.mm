// BNViewDelegate.mm — default `MTKViewDelegate` that drives a BNView.
// Subclassable by hosts that want to insert per-frame work; just
// override the delegate methods and call `super` to keep the default
// forwarding behavior.

#import <BabylonNativeIntegrations/BNView.h>

@implementation BNViewDelegate
{
    // BNView holds the auto-installed delegate strongly, and
    // host-installed subclass delegates are typically held strongly by
    // the host's own controller — so a weak back-reference here is
    // sufficient and avoids any chance of a retain cycle if a subclass
    // captures the BNView in a closure or property.
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
    [_view resizeWithWidth:static_cast<NSUInteger>(size.width)
                    height:static_cast<NSUInteger>(size.height)];
}

- (void)drawInMTKView:(MTKView* __unused)v
{
    // `[_view renderFrame]` already handles the XR overlay visibility
    // toggle internally via the runtime, so subclasses that override
    // `drawInMTKView:` and call `super` get the behavior for free.
    [_view renderFrame];
}

@end
