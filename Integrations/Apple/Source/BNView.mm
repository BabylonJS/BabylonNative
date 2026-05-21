// BNView.mm — Obj-C++ implementation bridging BNView to
// Babylon::Integrations::View.

#import "BNRuntimeInternal.h"
#import <Babylon/Integrations/Apple/BNView.h>

#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <Babylon/Integrations/View.h>

#include <cstdint>
#include <optional>

@implementation BNView
{
    std::optional<Babylon::Integrations::View> _view;
    BNRuntime* _runtime;
    MTKView* _mtkView;

    // When BNView auto-installs a default delegate, hold a strong ref
    // here to outlive MTKView's `weak` delegate slot. Nil when the host
    // installed their own delegate before constructing BNView.
    BNViewDelegate* _managedDelegate;
}

- (instancetype)initWithRuntime:(BNRuntime*)runtime view:(MTKView*)view
{
    if (runtime == nil || view == nil)
    {
        return nil;
    }
    Babylon::Integrations::Runtime* nativeRuntime = runtime.nativeRuntime;
    if (nativeRuntime == nullptr)
    {
        return nil;
    }
    if ((self = [super init]))
    {
        _runtime = runtime;
        _mtkView = view;

        // MTKView's layer is always CAMetalLayer (via +layerClass).
        CAMetalLayer* layer = (CAMetalLayer*)view.layer;

        // View construction is lightweight (just stashes the layer). Device
        // construction is driven by the first `-resizeWithWidth:height:`
        // call, which BNViewDelegate forwards from MTKView's
        // `mtkView:drawableSizeWillChange:`. UIKit's view lifecycle
        // reliably fires that callback during initial layout, but AppKit
        // is more lazy: on macOS the callback typically does NOT fire
        // for the initial drawable size if the delegate was installed
        // after the size was already determined — only for subsequent
        // changes. To keep both platforms working out of the box, we
        // query the current drawable size below and kick off an explicit
        // resize. If the drawable isn't sized yet (e.g. MTKView not yet
        // in a window), the early-out below skips it and the delegate
        // path will pick it up later.
        try
        {
            _view.emplace(*nativeRuntime, (__bridge CA::MetalLayer*)layer);
        }
        catch (const std::exception&)
        {
            _mtkView = nil;
            return nil;
        }

        // If the host hasn't installed an MTKViewDelegate, install a
        // default BNViewDelegate. Done AFTER View construction so any
        // drawableSizeWillChange: triggered by the assignment doesn't
        // reach us before _view is constructed.
        if (view.delegate == nil)
        {
            _managedDelegate = [[BNViewDelegate alloc] initWithView:self];
            view.delegate = _managedDelegate;
        }

        // Kick off the first resize using the MTKView's bounds in
        // logical units (points/DIPs). `View::Resize` handles the DPR
        // conversion internally; passing Logical here means we don't
        // have to query the layer's drawableSize (which can be zero
        // before MTKView's first layout pass) or recompute the same
        // bounds × scale conversion ourselves.
        //
        // If bounds are also zero (e.g. MTKView not yet in a window),
        // skip and rely on the delegate path
        // (`mtkView:drawableSizeWillChange:`) to deliver the first
        // size once layout happens. `View::Resize` past the first
        // call is just an idempotent `Device::UpdateSize`, so the
        // explicit kick here composes harmlessly with any subsequent
        // delegate callback.
        const CGSize bounds = view.bounds.size;
        if (bounds.width > 0 && bounds.height > 0)
        {
            if (_view)
            {
                _view->Resize(static_cast<uint32_t>(bounds.width),
                               static_cast<uint32_t>(bounds.height),
                               Babylon::Integrations::CoordinateUnits::Logical);
            }
        }
    }
    return self;
}

- (void)dealloc
{
    // Only clear if it's still ours; never disturb a host-installed delegate.
    if (_managedDelegate != nil && _mtkView.delegate == _managedDelegate)
    {
        _mtkView.delegate = nil;
    }
}

- (void)renderFrame
{
    // The runtime owns the XR overlay view (via -setXrView:), so it's
    // the natural place to keep its visibility in sync. Doing this here
    // means hosts never need to manage the overlay themselves regardless
    // of whether frames are driven by the auto-installed delegate, a
    // host subclass, or a fully-custom delegate.
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

#pragma mark - Mouse forwarding

- (void)mouseDown:(NSInteger)button atX:(CGFloat)x y:(CGFloat)y
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnMouseDown(static_cast<uint32_t>(button),
                            static_cast<float>(x),
                            static_cast<float>(y),
                            Babylon::Integrations::CoordinateUnits::Logical);
    }
#else
    (void)button; (void)x; (void)y;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"mouseDown:atX:y: was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT was not enabled at native build time."
                 userInfo:nil];
#endif
}

- (void)mouseUp:(NSInteger)button atX:(CGFloat)x y:(CGFloat)y
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnMouseUp(static_cast<uint32_t>(button),
                          static_cast<float>(x),
                          static_cast<float>(y),
                          Babylon::Integrations::CoordinateUnits::Logical);
    }
#else
    (void)button; (void)x; (void)y;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"mouseUp:atX:y: was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT was not enabled at native build time."
                 userInfo:nil];
#endif
}

- (void)mouseMoveAtX:(CGFloat)x y:(CGFloat)y
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnMouseMove(static_cast<float>(x),
                            static_cast<float>(y),
                            Babylon::Integrations::CoordinateUnits::Logical);
    }
#else
    (void)x; (void)y;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"mouseMoveAtX:y: was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT was not enabled at native build time."
                 userInfo:nil];
#endif
}

- (void)mouseWheel:(NSInteger)axis delta:(NSInteger)delta
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnMouseWheel(static_cast<uint32_t>(axis),
                             static_cast<int32_t>(delta));
    }
#else
    (void)axis; (void)delta;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"mouseWheel:delta: was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT was not enabled at native build time."
                 userInfo:nil];
#endif
}

+ (NSInteger)leftMouseButton
{
    return static_cast<NSInteger>(Babylon::Integrations::View::LeftMouseButton());
}

+ (NSInteger)middleMouseButton
{
    return static_cast<NSInteger>(Babylon::Integrations::View::MiddleMouseButton());
}

+ (NSInteger)rightMouseButton
{
    return static_cast<NSInteger>(Babylon::Integrations::View::RightMouseButton());
}

+ (NSInteger)mouseWheelY
{
    return static_cast<NSInteger>(Babylon::Integrations::View::MouseWheelY());
}

@end
