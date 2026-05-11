// BNView.h — public Obj-C interface for the Babylon::Integrations
// view on Apple platforms.
//
// Construct against a host-provided `MTKView`. The first `BNView`
// constructed against a given `BNRuntime` triggers GPU device
// construction, plugin initialization, and queued-script flushing.
// Subsequent views attached to the same runtime are cheap surface
// rebinds.
//
// By default, if the MTKView has no delegate when `BNView` is
// constructed, BNView installs and strong-holds a `BNViewDelegate`
// for you and the host doesn't have to wire anything up. If the host
// wants to interleave per-frame work, they assign their own
// `MTKViewDelegate` (typically a `BNViewDelegate` subclass) to the
// MTKView before constructing the BNView; in that case BNView leaves
// the host's delegate alone.
//
// See SimplifiedAPI.md §4.2 / §5 for the design and usage examples.

#pragma once

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <MetalKit/MTKView.h>

@class BNRuntime;
@class BNView;

NS_ASSUME_NONNULL_BEGIN

/// Default `MTKViewDelegate` implementation that drives a `BNView`.
/// Forwards `drawInMTKView:` to `[bnView renderFrame]` and
/// `mtkView:drawableSizeWillChange:` to `[bnView resizeWithWidth:height:]`.
///
/// `BNView` automatically installs and retains an instance of this
/// class when constructed against an `MTKView` that has no delegate
/// yet, so most hosts never need to construct one explicitly.
///
/// To insert per-frame work, subclass `BNViewDelegate`, override the
/// delegate methods, and call `super` to keep the default forwarding
/// behavior.
@interface BNViewDelegate : NSObject <MTKViewDelegate>

/// Initialize a delegate that drives `view`. The reference is held
/// weakly; the host (or BNView, when this is the auto-installed
/// instance) is responsible for keeping the BNView alive.
- (nullable instancetype)initWithView:(BNView*)view NS_DESIGNATED_INITIALIZER;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

@end

@interface BNView : NSObject

/// Attach `runtime` to render into `view`. On the first attach for a
/// given runtime, this triggers GPU device construction and engine
/// initialization. Subsequent attaches just rebind the surface.
///
/// Returns `nil` if `runtime` or `view` is `nil`, or if the underlying
/// `Babylon::Integrations::View::Attach` fails.
///
/// **Delegate management:** If `view.delegate` is `nil` at the time of
/// construction, BNView creates a `BNViewDelegate` and assigns it to
/// `view.delegate`, holding it strongly for the lifetime of the
/// BNView (so the host doesn't have to retain it themselves —
/// `MTKView.delegate` is a `weak` reference). If `view.delegate` is
/// already set, BNView does *not* touch it; the host is expected to
/// drive `-renderFrame` and `-resize(width:height:)` themselves
/// (typically via their own `MTKViewDelegate` or a `BNViewDelegate`
/// subclass).
- (nullable instancetype)initWithRuntime:(BNRuntime*)runtime view:(MTKView*)view;

/// Render exactly one frame. Call from your `MTKViewDelegate
/// drawInMTKView:`, or rely on the auto-installed `BNViewDelegate`
/// to call it for you.
- (void)renderFrame;

/// Inform the runtime that the underlying surface's pixel-buffer size
/// has changed. Sizes are in physical pixels — the same convention as
/// `CAMetalLayer.drawableSize`.
- (void)resizeWithWidth:(NSUInteger)width height:(NSUInteger)height
    NS_SWIFT_NAME(resize(width:height:));

/// Forward a pointer-down event. `x`, `y` are in logical (CSS) pixels
/// — pass `UITouch.location(in:)` (UIKit) or `NSEvent.locationInWindow`
/// (AppKit) coordinates through unchanged.
///
/// Raises an `NSException` (name
/// `BabylonNativePluginNotEnabledException`) if invoked when
/// `BABYLON_NATIVE_PLUGIN_NATIVEINPUT` was not enabled at native
/// build time. The same applies to `pointerMove:atX:y:` and
/// `pointerUp:atX:y:` below.
- (void)pointerDown:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(pointerDown(id:x:y:));

- (void)pointerMove:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(pointerMove(id:x:y:));

- (void)pointerUp:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
    NS_SWIFT_NAME(pointerUp(id:x:y:));

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END

