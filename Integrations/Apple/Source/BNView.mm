// BNView.mm — Obj-C++ implementation bridging BNView to
// Babylon::Integrations::View.

#import "BNRuntimeInternal.h"
#import <BabylonNativeIntegrations/BNView.h>

#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <Babylon/Integrations/View.h>

#include <cstdint>
#include <memory>

@interface BNView () <MTKViewDelegate>
@end

@implementation BNView
{
    std::unique_ptr<Babylon::Integrations::View> _view;
    BNRuntime* _runtime;
    MTKView* _mtkView;
    BOOL _autoDelegate;
}

- (instancetype)initWithRuntime:(BNRuntime*)runtime view:(MTKView*)view
{
    return [self initWithRuntime:runtime view:view autoDelegate:YES];
}

- (instancetype)initWithRuntime:(BNRuntime*)runtime
                            view:(MTKView*)view
                    autoDelegate:(BOOL)autoDelegate
{
    if (runtime == nil || view == nil)
    {
        return nil;
    }
    if ((self = [super init]))
    {
        _runtime = runtime;
        _mtkView = view;
        _autoDelegate = autoDelegate;

        // MTKView's underlying layer is always a CAMetalLayer (its
        // +layerClass override).
        CAMetalLayer* layer = (CAMetalLayer*)view.layer;

        // Force a layout pass so the view's bounds are valid before we
        // read them, and seed the layer's drawableSize from the view's
        // logical bounds × backing scale. MTKView's autoResizeDrawable
        // will keep drawableSize in sync from this point on, but at
        // attach time it can still be (0, 0) — so we set it explicitly
        // to avoid handing bgfx a zero-sized swap chain.
        [view layoutIfNeeded];
#if TARGET_OS_OSX
        const CGFloat scale = view.window.backingScaleFactor > 0
            ? view.window.backingScaleFactor
            : 1.0;
#else
        const CGFloat scale = view.contentScaleFactor;
#endif
        layer.drawableSize = CGSizeMake(view.bounds.size.width * scale,
                                         view.bounds.size.height * scale);

        // First attach on this runtime triggers GPU device construction
        // + plugin initialization + queued-script flush. The View
        // queries the layer's drawableSize itself; the host doesn't
        // need to pass dimensions.
        _view = Babylon::Integrations::View::Attach(
            *runtime.nativeRuntime,
            (__bridge CA::MetalLayer*)layer);
        if (!_view)
        {
            _mtkView = nil;
            return nil;
        }

        // Install ourselves as the delegate AFTER Attach so that any
        // drawableSizeWillChange: dispatched as a side-effect of
        // assignment doesn't reach us before _view is constructed.
        if (_autoDelegate)
        {
            view.delegate = self;
        }
    }
    return self;
}

- (void)dealloc
{
    if (_autoDelegate && _mtkView.delegate == self)
    {
        _mtkView.delegate = nil;
    }
}

- (void)renderFrame
{
    // The runtime owns the XR overlay view (handed to it via
    // -setXrView:), so it's the natural place to keep its visibility
    // in sync with the XR session state. Doing this here means hosts
    // never have to manage the XR overlay themselves.
    [_runtime updateXrViewIfNeeded];

    if (_view)
    {
        _view->RenderFrame();
    }
}

- (void)resizeWithWidth:(NSUInteger)width height:(NSUInteger)height
{
    if (_view)
    {
        _view->Resize(static_cast<uint32_t>(width),
                       static_cast<uint32_t>(height));
    }
}

#pragma mark - MTKViewDelegate (auto-delegate mode only)

- (void)mtkView:(MTKView* __unused)view drawableSizeWillChange:(CGSize)size
{
    [self resizeWithWidth:static_cast<NSUInteger>(size.width)
                   height:static_cast<NSUInteger>(size.height)];
}

- (void)drawInMTKView:(MTKView* __unused)view
{
    [self renderFrame];
}

#pragma mark - Pointer forwarding

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT

- (void)pointerDown:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
{
    if (_view)
    {
        _view->OnPointerDown(static_cast<int32_t>(pointerId),
                              static_cast<float>(x),
                              static_cast<float>(y));
    }
}

- (void)pointerMove:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
{
    if (_view)
    {
        _view->OnPointerMove(static_cast<int32_t>(pointerId),
                              static_cast<float>(x),
                              static_cast<float>(y));
    }
}

- (void)pointerUp:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
{
    if (_view)
    {
        _view->OnPointerUp(static_cast<int32_t>(pointerId),
                            static_cast<float>(x),
                            static_cast<float>(y));
    }
}

#else

// When NATIVEINPUT is disabled at native build time, the methods are
// still declared on the public BNView header for binary stability;
// they become no-ops here.
- (void)pointerDown:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y { (void)pointerId; (void)x; (void)y; }
- (void)pointerMove:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y { (void)pointerId; (void)x; (void)y; }
- (void)pointerUp:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y   { (void)pointerId; (void)x; (void)y; }

#endif

@end
