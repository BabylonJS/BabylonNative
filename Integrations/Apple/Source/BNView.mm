// BNView.mm — Obj-C++ implementation bridging BNView to
// Babylon::Integrations::View.

#import "BNRuntimeInternal.h"
#import <Babylon/Integrations/Apple/BNView.h>

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

        // View::Attach is intentionally lightweight: it just stashes
        // the layer pointer. No size query, no Device construction —
        // and therefore nothing host-recoverable to throw. The MTKView
        // delegate's `mtkView:drawableSizeWillChange:` (forwarded by
        // BNViewDelegate to `-resizeWithWidth:height:`) is what
        // actually drives Device construction + first-frame opening,
        // on the first call. MTKView fires this callback before its
        // first draw in normal Cocoa usage, so this bootstrap is
        // automatic. Hosts that drive MTKView in unusual ways (e.g.
        // hidden preload) can call `-resizeWithWidth:height:` directly
        // with the surface's current pixel size.
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
                       static_cast<uint32_t>(height),
                       Babylon::Integrations::CoordinateUnits::Physical);
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
                              static_cast<float>(y),
                              Babylon::Integrations::CoordinateUnits::Logical);
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
                              static_cast<float>(y),
                              Babylon::Integrations::CoordinateUnits::Logical);
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
                            static_cast<float>(y),
                            Babylon::Integrations::CoordinateUnits::Logical);
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
