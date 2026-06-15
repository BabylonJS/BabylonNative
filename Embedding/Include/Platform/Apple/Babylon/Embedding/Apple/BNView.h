// BNView.h — public Obj-C interface for the Babylon::Embedding view
// on Apple platforms.
//
// Construct against a host-provided `MTKView`. The first BNView against
// a given BNRuntime triggers GPU device construction, plugin init, and
// queued-script flushing. Subsequent attaches are cheap surface rebinds.
//
// If the MTKView has no delegate when BNView is constructed, BNView
// installs and retains a `BNViewDelegate` automatically. If the host
// installed a delegate first (typically a BNViewDelegate subclass for
// per-frame work), BNView leaves it alone.

#pragma once

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <MetalKit/MTKView.h>

@class BNRuntime;
@class BNView;

NS_ASSUME_NONNULL_BEGIN

/// Mouse buttons accepted by `-mouseDown:atX:y:` / `-mouseUp:atX:y:`.
/// Obj-C can't nest a type in `@interface`, so the enum is prefixed and
/// surfaced to Swift nested as `BNView.MouseButton` (`.left` / `.middle`
/// / `.right`).
typedef NS_ENUM(NSInteger, BNViewMouseButton) {
    BNViewMouseButtonLeft,
    BNViewMouseButtonMiddle,
    BNViewMouseButtonRight,
} NS_SWIFT_NAME(BNView.MouseButton);

/// Scroll-wheel axes accepted by `-mouseWheel:delta:`.
/// Surfaced to Swift nested as `BNView.MouseWheelAxis` (`.y`).
typedef NS_ENUM(NSInteger, BNViewMouseWheelAxis) {
    BNViewMouseWheelAxisY,
} NS_SWIFT_NAME(BNView.MouseWheelAxis);

/// Default `MTKViewDelegate` implementation that drives a BNView's
/// per-frame rendering: forwards `drawInMTKView:` → `[bnView renderFrame]`.
///
/// It does NOT drive resize — BNView sets `autoResizeDrawable = NO`, so
/// MTKView no longer reports size changes and the host drives resize
/// explicitly from a layout hook (see `-[BNView resizeWithWidth:height:]`).
///
/// BNView installs and retains one automatically when constructed
/// against an MTKView that has no delegate. To insert per-frame work,
/// subclass this, override `drawInMTKView:`, and call `super`.
@interface BNViewDelegate : NSObject <MTKViewDelegate>

/// Initialize a delegate that drives `view`. The reference is weak;
/// the host (or BNView, for the auto-installed instance) keeps the
/// BNView alive.
- (nullable instancetype)initWithView:(BNView*)view NS_DESIGNATED_INITIALIZER;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

@end

@interface BNView : NSObject

/// Attach `runtime` to render into `view`. On the first attach for a
/// given runtime this triggers GPU device construction and engine
/// initialization; subsequent attaches just rebind the surface.
///
/// Returns `nil` if `runtime` or `view` is `nil`, or if constructing the
/// underlying `Babylon::Embedding::View` throws (another View is
/// already attached to the runtime).
///
/// **Delegate management:** If `view.delegate` is nil, BNView creates a
/// BNViewDelegate and assigns it (held strongly for the BNView's
/// lifetime, since MTKView.delegate is `weak`). If the host already
/// set a delegate, BNView leaves it alone and the host drives
/// `-renderFrame`. Either way the host must drive `-resizeWithWidth:height:`
/// from a layout hook; the delegate never does (BNView sets
/// `autoResizeDrawable = NO`, so MTKView reports no size changes).
- (nullable instancetype)initWithRuntime:(BNRuntime*)runtime view:(MTKView*)view;

/// Render exactly one frame. Call from your MTKViewDelegate's
/// `drawInMTKView:`, or rely on the auto-installed BNViewDelegate.
- (void)renderFrame;

/// Inform the runtime that the view's size has changed. Call this from
/// your view-controller layout hook — `-viewDidLayoutSubviews` on UIKit,
/// `-viewDidLayout` on AppKit — passing the MTKView's `bounds.size`.
///
/// Sizes are in **logical** points (CSS pixels); BNView applies the
/// device-pixel-ratio internally. Babylon's render resolution then
/// follows the engine's hardware scaling level (logical by default, or
/// physical when `adaptToDevicePixelRatio` is enabled).
- (void)resizeWithWidth:(NSUInteger)width height:(NSUInteger)height
    NS_SWIFT_NAME(resize(width:height:));

/// Forward a pointer event. `x`, `y` are logical (CSS) pixels — pass
/// `UITouch.location(in:)` (UIKit) or `NSEvent.locationInWindow`
/// (AppKit) coordinates through unchanged.
///
/// Raises `BabylonNativePluginNotEnabledException` when
/// `BABYLON_NATIVE_PLUGIN_NATIVEINPUT` is not enabled. Same applies to
/// `pointerMove:` and `pointerUp:`.
- (void)pointerDown:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(pointerDown(id:x:y:));

- (void)pointerMove:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(pointerMove(id:x:y:));

- (void)pointerUp:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(pointerUp(id:x:y:));

/// Forward a mouse-button event. `button` selects which button. `x`, `y`
/// are logical (CSS) pixels; AppKit hosts pass `NSEvent.locationInWindow`
/// with the Y axis flipped to a top-left origin.
///
/// Raises `BabylonNativePluginNotEnabledException` when
/// `BABYLON_NATIVE_PLUGIN_NATIVEINPUT` is not enabled. Same applies to
/// `mouseUp:`, `mouseMove:`, and `mouseWheel:`.
- (void)mouseDown:(BNViewMouseButton)button atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(mouseDown(button:x:y:));

- (void)mouseUp:(BNViewMouseButton)button atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(mouseUp(button:x:y:));

- (void)mouseMoveAtX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(mouseMove(x:y:));

/// Forward a scroll-wheel event. `axis` selects which axis. `delta` is the
/// signed scroll amount; AppKit hosts pass `-NSEvent.deltaY` so scroll-up
/// matches Babylon's negative convention. The fractional delta is truncated
/// to an integer for the underlying input pipeline.
- (void)mouseWheel:(BNViewMouseWheelAxis)axis delta:(CGFloat)delta
    NS_SWIFT_NAME(mouseWheel(axis:delta:));

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
