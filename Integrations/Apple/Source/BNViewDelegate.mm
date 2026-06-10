// BNViewDelegate.mm — default `MTKViewDelegate` that drives a BNView's
// per-frame rendering. Subclassable by hosts that want to insert
// per-frame work; just override `drawInMTKView:` and call `super` to
// keep the default render behavior.
//
// This delegate does NOT drive resize. BNView sets
// `autoResizeDrawable = NO` so Babylon Native owns the drawable size,
// which means MTKView no longer reports size changes via
// `mtkView:drawableSizeWillChange:`. Hosts drive resize explicitly from
// a layout hook (`-viewDidLayoutSubviews` on UIKit, `-viewDidLayout` on
// AppKit), the same way every other platform does — see
// `-[BNView resizeWithWidth:height:]`.

#import <Babylon/Integrations/Apple/BNView.h>

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

- (void)mtkView:(MTKView* __unused)v drawableSizeWillChange:(CGSize __unused)size
{
    // Intentionally empty. BNView sets autoResizeDrawable = NO, so this
    // callback no longer reports host-driven size changes; resize is
    // driven explicitly by the host from a layout hook. Kept only to
    // satisfy the (required) MTKViewDelegate protocol method.
}

- (void)drawInMTKView:(MTKView* __unused)v
{
    // [_view renderFrame] handles the XR overlay visibility toggle, so
    // subclasses that override this and call super get it for free.
    [_view renderFrame];
}

@end
