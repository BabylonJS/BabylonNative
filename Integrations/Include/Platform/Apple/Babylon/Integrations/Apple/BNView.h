// BNView.h — public Obj-C interface for the Babylon::Integrations view
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

/// Default `MTKViewDelegate` implementation that drives a BNView.
/// Forwards `drawInMTKView:` → `[bnView renderFrame]` and
/// `mtkView:drawableSizeWillChange:` → `[bnView resizeWithWidth:height:]`.
///
/// BNView installs and retains one automatically when constructed
/// against an MTKView that has no delegate. To insert per-frame work,
/// subclass this, override the delegate methods, and call `super`.
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
/// underlying `Babylon::Integrations::View` throws (another View is
/// already attached to the runtime).
///
/// **Delegate management:** If `view.delegate` is nil, BNView creates a
/// BNViewDelegate and assigns it (held strongly for the BNView's
/// lifetime, since MTKView.delegate is `weak`). If the host already
/// set a delegate, BNView leaves it alone and the host drives
/// `-renderFrame` and `-resizeWithWidth:height:`.
- (nullable instancetype)initWithRuntime:(BNRuntime*)runtime view:(MTKView*)view;

/// Render exactly one frame. Call from your MTKViewDelegate's
/// `drawInMTKView:`, or rely on the auto-installed BNViewDelegate.
- (void)renderFrame;

/// Inform the runtime that the underlying surface's pixel-buffer size
/// has changed. Sizes are in physical pixels (same convention as
/// `CAMetalLayer.drawableSize`).
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

/// Forward a mouse-button event. `button` is one of `+leftMouseButton`,
/// `+middleMouseButton`, `+rightMouseButton`. `x`, `y` are logical
/// (CSS) pixels; AppKit hosts pass `NSEvent.locationInWindow` with the
/// Y axis flipped to a top-left origin.
///
/// Raises `BabylonNativePluginNotEnabledException` when
/// `BABYLON_NATIVE_PLUGIN_NATIVEINPUT` is not enabled. Same applies to
/// `mouseUp:`, `mouseMove:`, and `mouseWheel:`.
- (void)mouseDown:(NSInteger)button atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(mouseDown(button:x:y:));

- (void)mouseUp:(NSInteger)button atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(mouseUp(button:x:y:));

- (void)mouseMoveAtX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(mouseMove(x:y:));

/// Forward a scroll-wheel event. `axis` is `+mouseWheelY`. `delta` is
/// the signed scroll amount; AppKit hosts pass `-NSEvent.deltaY` so
/// scroll-up matches Babylon's negative convention.
- (void)mouseWheel:(NSInteger)axis delta:(NSInteger)delta
    NS_SWIFT_NAME(mouseWheel(axis:delta:));

/// Button identifiers accepted by `mouseDown:` and `mouseUp:`.
@property (class, nonatomic, readonly) NSInteger leftMouseButton;
@property (class, nonatomic, readonly) NSInteger middleMouseButton;
@property (class, nonatomic, readonly) NSInteger rightMouseButton;

/// Wheel axis identifier accepted by `mouseWheel:delta:`.
@property (class, nonatomic, readonly) NSInteger mouseWheelY;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
