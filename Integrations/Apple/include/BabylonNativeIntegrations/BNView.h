// BNView.h — public Obj-C interface for the Babylon::Integrations
// view on Apple platforms.
//
// Construct against a host-provided `MTKView`. The first `BNView`
// constructed against a given `BNRuntime` triggers GPU device
// construction, plugin initialization, and queued-script flushing.
// Subsequent views attached to the same runtime are cheap surface
// rebinds.
//
// BNView installs itself as the MTKView's `delegate` and drives the
// per-frame render and resize callbacks internally — the host does
// not need to forward `MTKViewDelegate.draw(in:)` or
// `mtkView:drawableSizeWillChange:`.
//
// See SimplifiedAPI.md §4.2 / §5 for the design and usage examples.

#pragma once

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@class MTKView;
@class BNRuntime;

NS_ASSUME_NONNULL_BEGIN

@interface BNView : NSObject

/// Attach `runtime` to render into `view` ("super simple" mode). BNView
/// installs itself as the MTKView's `delegate` and drives the per-frame
/// render and resize callbacks internally — the host does not need to
/// forward anything.
///
/// Equivalent to `-initWithRuntime:view:autoDelegate:` with
/// `autoDelegate = YES`.
- (instancetype)initWithRuntime:(BNRuntime*)runtime view:(MTKView*)view;

/// Attach `runtime` to render into `view` with explicit control over
/// the MTKView delegate.
///
/// - When `autoDelegate` is `YES`, BNView installs itself as the
///   MTKView's delegate and drives `drawInMTKView:` and
///   `mtkView:drawableSizeWillChange:` internally. The host should
///   not assign its own delegate to the same MTKView.
/// - When `autoDelegate` is `NO`, the host keeps ownership of the
///   MTKView's delegate and is responsible for calling `-renderFrame`
///   and `-resizeWithWidth:height:` from its own MTKViewDelegate
///   methods. Use this mode when the host needs to interleave its
///   own per-frame work with the runtime's render.
- (instancetype)initWithRuntime:(BNRuntime*)runtime
                            view:(MTKView*)view
                    autoDelegate:(BOOL)autoDelegate NS_DESIGNATED_INITIALIZER;

/// Render exactly one frame. Only used in manual-delegate mode
/// (`autoDelegate = NO`) — call from your own `MTKViewDelegate
/// drawInMTKView:` method.
- (void)renderFrame;

/// Inform the runtime that the underlying surface's pixel-buffer size
/// has changed. Only used in manual-delegate mode — call from your
/// own `MTKViewDelegate mtkView:drawableSizeWillChange:` method with
/// the `size.width` / `size.height` it provides. Sizes are in physical
/// pixels — the same convention as `CAMetalLayer.drawableSize`.
- (void)resizeWithWidth:(NSUInteger)width height:(NSUInteger)height
    NS_SWIFT_NAME(resize(width:height:));

/// Forward a pointer-down event. `x`, `y` are in logical (CSS) pixels
/// — pass `UITouch.location(in:)` (UIKit) or `NSEvent.locationInWindow`
/// (AppKit) coordinates through unchanged.
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

