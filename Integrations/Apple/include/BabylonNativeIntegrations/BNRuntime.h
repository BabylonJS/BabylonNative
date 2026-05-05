// BNRuntime.h — public Obj-C interface for the Babylon::Integrations
// runtime on Apple platforms (iOS, macOS, visionOS).
//
// Swift consumers see this through the auto-generated Swift bridge
// (BNRuntime is exposed to Swift as `BNRuntime`).
//
// See SimplifiedAPI.md §4.2 / §5 for the design and usage examples.

#pragma once

#import <Foundation/Foundation.h>

@class MTKView;

NS_ASSUME_NONNULL_BEGIN

@interface BNRuntime : NSObject

/// Constructs the runtime: starts the JS engine + thread, sets up
/// non-GPU polyfills and plugins. Cheap and synchronous; no GPU
/// device is created yet (that happens on the first `BNView` attach).
/// Default options: JS debugger off, log routes to `NSLog`.
- (instancetype)init;

/// Same as `init` but lets the host opt into the JS debugger.
- (instancetype)initWithEnableDebugger:(BOOL)enableDebugger NS_DESIGNATED_INITIALIZER;

/// Load a script from a URL onto the JS thread. Calls made before
/// the first `BNView` is created are queued internally and dispatched
/// after engine initialization completes during that first attach.
/// Calls after the first attach are dispatched immediately.
- (void)loadScript:(NSString*)url;

/// Evaluate JavaScript source on the JS thread. Same queueing
/// semantics as `loadScript`.
- (void)eval:(NSString*)source sourceURL:(NSString*)sourceURL;

/// Reference-counted suspend. While suspended, JS timers pause and
/// any attached `BNView` becomes a no-op for `renderFrame` (the host
/// can keep calling it from its draw callback unconditionally;
/// nothing happens until `resume`).
- (void)suspend;

/// Decrement the suspend count; resume the JS thread when the count
/// reaches zero.
- (void)resume;

/// Whether the runtime is currently suspended.
@property (nonatomic, readonly, getter=isSuspended) BOOL suspended;

/// Set the platform view that XR will render into (typically a
/// separate transparent `MTKView` overlay, distinct from the main
/// view's Metal layer). Pass `nil` to clear the XR surface. Safe to
/// call before the first `BNView` attach; the value is applied when
/// NativeXr finishes initializing during that first attach.
///
/// Compiled-in only when `BABYLON_NATIVE_PLUGIN_NATIVEXR` is enabled
/// at native build time; otherwise this is a no-op.
- (void)setXrView:(nullable MTKView*)xrView;

/// `YES` while an XR session is active. Updated from the JS thread
/// by NativeXr's internal session-state callback; safe to poll from
/// any thread.
@property (nonatomic, readonly, getter=isXRActive) BOOL xrActive;

@end

NS_ASSUME_NONNULL_END

