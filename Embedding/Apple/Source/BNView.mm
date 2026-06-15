// BNView.mm — Obj-C++ implementation bridging BNView to
// Babylon::Embedding::View.

#import "BNRuntimeInternal.h"
#import <Babylon/Embedding/Apple/BNView.h>

#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <Babylon/Embedding/View.h>

#include <cstdint>
#include <optional>

namespace
{
    // Map the public BNView mouse enums onto the NativeInput button/axis IDs
    // the C++ View expects, keeping the NativeInput numbering out of the
    // public Obj-C/Swift surface.
    uint32_t NativeMouseButtonId(BNViewMouseButton button)
    {
        switch (button)
        {
            case BNViewMouseButtonLeft:   return Babylon::Embedding::View::LeftMouseButton();
            case BNViewMouseButtonMiddle: return Babylon::Embedding::View::MiddleMouseButton();
            case BNViewMouseButtonRight:  return Babylon::Embedding::View::RightMouseButton();
        }
        return Babylon::Embedding::View::LeftMouseButton();
    }

    uint32_t NativeMouseWheelAxisId(BNViewMouseWheelAxis axis)
    {
        switch (axis)
        {
            case BNViewMouseWheelAxisY: return Babylon::Embedding::View::MouseWheelY();
        }
        return Babylon::Embedding::View::MouseWheelY();
    }
}

@implementation BNView
{
    std::optional<Babylon::Embedding::View> _view;
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

    Babylon::Embedding::Runtime* nativeRuntime = runtime.nativeRuntime;
    if (nativeRuntime == nullptr)
    {
        return nil;
    }

    if ((self = [super init]))
    {
        // Babylon Native must control the drawable size.
        view.autoResizeDrawable = NO;

        _runtime = runtime;
        _mtkView = view;

        // MTKView's layer is always CAMetalLayer (via +layerClass).
        CAMetalLayer* layer = (CAMetalLayer*)view.layer;

        // View construction is lightweight (just stashes the layer). Device
        // construction is driven by the first `-resizeWithWidth:height:`
        // call. Because BNView sets `autoResizeDrawable = NO` above
        // (Babylon Native owns the drawable size), MTKView no longer
        // reports size via `mtkView:drawableSizeWillChange:`; instead the
        // host drives resize explicitly from a layout hook
        // (`-viewDidLayoutSubviews` / `-viewDidLayout`), the same as every
        // other platform. We still kick off an initial resize from the
        // current bounds below so JS engine init can begin immediately
        // when the view is already laid out at construction time; if
        // bounds are still zero (view not yet laid out / in a window), the
        // host's first layout pass delivers the size.
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
        // default (render-only) BNViewDelegate. Done AFTER View
        // construction so any delegate callback triggered by the
        // assignment doesn't reach us before _view is constructed.
        if (view.delegate == nil)
        {
            _managedDelegate = [[BNViewDelegate alloc] initWithView:self];
            view.delegate = _managedDelegate;
        }

        // Kick off the first resize using the MTKView's bounds in
        // logical units (points/DIPs). `View::Resize` handles the DPR
        // conversion internally; passing Logical here matches the units
        // the host's layout hook passes for subsequent resizes.
        //
        // If bounds are zero (e.g. MTKView not yet laid out / in a
        // window), skip and rely on the host's first layout-driven
        // `-resizeWithWidth:height:` to deliver the first size.
        // `View::Resize` past the first call is just an idempotent
        // `Device::UpdateSize`, so this explicit kick composes harmlessly
        // with the host's subsequent resize calls.
        const CGSize bounds = view.bounds.size;
        if (bounds.width > 0 && bounds.height > 0)
        {
            if (_view)
            {
                _view->Resize(static_cast<uint32_t>(bounds.width),
                               static_cast<uint32_t>(bounds.height),
                               Babylon::Embedding::CoordinateUnits::Logical);
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
    // Skip rendering until the backing CAMetalLayer has a non-zero drawable;
    // bgfx can't acquire a drawable at 0x0 and Metal validation fails with
    // "no output textures defined for the render pass". The host's
    // layout-driven -resizeWithWidth:height: delivers the real size.
    //
    // Read from the CAMetalLayer, not MTKView.drawableSize: BNView sets
    // autoResizeDrawable = NO, so MTKView stops tracking drawableSize (stays
    // 0x0) while Babylon Native writes the real size onto the layer. The
    // guard lives here because the live drawable size is Apple-specific and
    // -renderFrame is the single chokepoint for every render path.
    CAMetalLayer* metalLayer = (CAMetalLayer*)_mtkView.layer;
    const CGSize drawableSize = metalLayer.drawableSize;
    if (drawableSize.width <= 0 || drawableSize.height <= 0)
    {
        return;
    }

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
        // Host passes logical points (from the view's bounds); View::Resize
        // converts to physical internally using the layer's DPR.
        _view->Resize(static_cast<uint32_t>(width),
                       static_cast<uint32_t>(height),
                       Babylon::Embedding::CoordinateUnits::Logical);
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
                              Babylon::Embedding::CoordinateUnits::Logical);
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
                              Babylon::Embedding::CoordinateUnits::Logical);
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
                            Babylon::Embedding::CoordinateUnits::Logical);
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

- (void)mouseDown:(BNViewMouseButton)button atX:(CGFloat)x y:(CGFloat)y
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnMouseDown(NativeMouseButtonId(button),
                            static_cast<float>(x),
                            static_cast<float>(y),
                            Babylon::Embedding::CoordinateUnits::Logical);
    }
#else
    (void)button; (void)x; (void)y;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"mouseDown:atX:y: was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT was not enabled at native build time."
                 userInfo:nil];
#endif
}

- (void)mouseUp:(BNViewMouseButton)button atX:(CGFloat)x y:(CGFloat)y
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnMouseUp(NativeMouseButtonId(button),
                          static_cast<float>(x),
                          static_cast<float>(y),
                          Babylon::Embedding::CoordinateUnits::Logical);
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
                            Babylon::Embedding::CoordinateUnits::Logical);
    }
#else
    (void)x; (void)y;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"mouseMoveAtX:y: was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT was not enabled at native build time."
                 userInfo:nil];
#endif
}

- (void)mouseWheel:(BNViewMouseWheelAxis)axis delta:(CGFloat)delta
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    if (_view)
    {
        _view->OnMouseWheel(NativeMouseWheelAxisId(axis),
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

@end
