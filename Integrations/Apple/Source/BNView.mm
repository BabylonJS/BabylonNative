// BNView.mm — Obj-C++ implementation bridging BNView to
// Babylon::Integrations::View.

#import "BNRuntimeInternal.h"
#import <BabylonNativeIntegrations/BNView.h>

#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <Babylon/Integrations/View.h>

#include <cstdint>
#include <memory>

@implementation BNView
{
    std::unique_ptr<Babylon::Integrations::View> _view;
    BNRuntime* _runtime;
    MTKView* _mtkView;

    // When BNView auto-installs a default delegate (because the host
    // didn't set one), it's held here so the strong reference outlives
    // the MTKView's `weak` delegate slot. Stays nil if the host
    // installed their own delegate before constructing BNView.
    BNViewDelegate* _managedDelegate;
}

- (instancetype)initWithRuntime:(BNRuntime*)runtime view:(MTKView*)view
{
    if (runtime == nil || view == nil)
    {
        return nil;
    }
    if ((self = [super init]))
    {
        _runtime = runtime;
        _mtkView = view;

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

        // If the host hasn't installed their own MTKViewDelegate by
        // now, install a default `BNViewDelegate` so frames start
        // flowing without any extra host wiring. Done AFTER Attach so
        // any drawableSizeWillChange: dispatched as a side-effect of
        // the assignment doesn't reach us before _view is constructed.
        if (view.delegate == nil)
        {
            _managedDelegate = [[BNViewDelegate alloc] initWithView:self];
            view.delegate = _managedDelegate;
        }
    }
    return self;
}

- (void)dealloc
{
    // Only clear the MTKView's delegate slot if it still points at the
    // delegate we installed; never disturb a host-installed delegate.
    if (_managedDelegate != nil && _mtkView.delegate == _managedDelegate)
    {
        _mtkView.delegate = nil;
    }
}

- (void)renderFrame
{
    // The runtime owns the XR overlay view (handed to it via
    // -setXrView:), so it's the natural place to keep its visibility
    // in sync with the XR session state. Doing this here means hosts
    // never have to manage the XR overlay themselves, regardless of
    // whether the runtime's frame is being driven by the auto-installed
    // BNViewDelegate, a host subclass, or a fully-custom delegate.
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

#pragma mark - Pointer forwarding

- (void)pointerDown:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnPointerDown(static_cast<int32_t>(pointerId),
                              static_cast<float>(x),
                              static_cast<float>(y));
    }
#else
    (void)pointerId; (void)x; (void)y;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"pointerDown:atX:y: was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT was not enabled at native build time."
                 userInfo:nil];
#endif
}

- (void)pointerMove:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnPointerMove(static_cast<int32_t>(pointerId),
                              static_cast<float>(x),
                              static_cast<float>(y));
    }
#else
    (void)pointerId; (void)x; (void)y;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"pointerMove:atX:y: was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT was not enabled at native build time."
                 userInfo:nil];
#endif
}

- (void)pointerUp:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnPointerUp(static_cast<int32_t>(pointerId),
                            static_cast<float>(x),
                            static_cast<float>(y));
    }
#else
    (void)pointerId; (void)x; (void)y;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"pointerUp:atX:y: was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT was not enabled at native build time."
                 userInfo:nil];
#endif
}

@end
