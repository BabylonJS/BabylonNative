// BNView.h — public Obj-C interface for the Babylon::Integrations
// view on Apple platforms.
//
// Construct against a host-provided `CAMetalLayer` (typically
// `MTKView.layer`). The first `BNView` constructed against a given
// `BNRuntime` triggers GPU device construction, plugin initialization,
// and queued-script flushing. Subsequent views attached to the same
// runtime are cheap surface rebinds.
//
// Width/height handling: this interop layer reads the layer's
// `drawableSize` (physical pixels) and `contentsScale` (DPR) directly,
// converting to the C++ logical-pixel convention internally. The
// Swift host does no unit math.
//
// See SimplifiedAPI.md §4.2 / §5 for the design and usage examples.

#pragma once

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@class CAMetalLayer;
@class BNRuntime;

NS_ASSUME_NONNULL_BEGIN

@interface BNView : NSObject

/// Attach to `runtime` rendering against `layer` (the host's
/// user-visible Metal layer). On the first attach for a given
/// runtime, this triggers GPU device construction and engine
/// initialization. Subsequent attaches just rebind the surface.
- (instancetype)initWithRuntime:(BNRuntime*)runtime layer:(CAMetalLayer*)layer;

/// Render exactly one frame. Call from the host's draw callback
/// (e.g. `MTKViewDelegate.draw(in:)`). No-op if the runtime is
/// suspended.
- (void)renderFrame;

/// Re-read drawableSize / contentsScale from the given layer and
/// apply as a resize. Call from
/// `MTKViewDelegate.mtkView(_:drawableSizeWillChange:)` or any other
/// resize hook.
- (void)resizeForLayer:(CAMetalLayer*)layer;

/// Forward a pointer-down event. `x`, `y` are in logical pixels.
/// Only present when `BABYLON_NATIVE_PLUGIN_NATIVEINPUT` is enabled
/// at native build time.
- (void)pointerDown:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y;

- (void)pointerMove:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y;

- (void)pointerUp:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y;

@end

NS_ASSUME_NONNULL_END
