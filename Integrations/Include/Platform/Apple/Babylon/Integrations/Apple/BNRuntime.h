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
- (instancetype)initWithEnableDebugger:(BOOL)enableDebugger;

/// Same as `initWithEnableDebugger:` but also wires up a persistent
/// on-disk GPU shader cache. Pass a writable file path (typically
/// inside `NSCachesDirectory`, e.g.
/// `[NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES).firstObject stringByAppendingPathComponent:@"babylon.shadercache"]`).
/// Pass `nil` to disable the on-disk cache (equivalent to the
/// `initWithEnableDebugger:` overload).
///
/// The cache is loaded on first `BNView` attach and saved on `suspend`
/// and on deallocation.
///
/// If `shaderCachePath` is non-`nil` but the native library was built
/// without `BABYLON_NATIVE_PLUGIN_SHADERCACHE`, this method raises an
/// `NSException` (name
/// `BabylonNativePluginNotEnabledException`) so the misconfiguration
/// surfaces at construction time rather than silently dropping the
/// cache. Passing `nil` is always safe regardless of build config.
- (instancetype)initWithEnableDebugger:(BOOL)enableDebugger
                       shaderCachePath:(nullable NSString*)shaderCachePath
                       NS_DESIGNATED_INITIALIZER;

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
/// Raises an `NSException` (name
/// `BabylonNativePluginNotEnabledException`) if invoked when
/// `BABYLON_NATIVE_PLUGIN_NATIVEXR` was not enabled at native build
/// time.
- (void)setXrView:(nullable MTKView*)xrView;

/// `YES` while an XR session is active. Updated from the JS thread
/// by NativeXr's internal session-state callback; safe to poll from
/// any thread. Returns `NO` when `BABYLON_NATIVE_PLUGIN_NATIVEXR` was
/// not enabled at native build time (no XR session can ever be active
/// in that build).
@property (nonatomic, readonly, getter=isXRActive) BOOL xrActive;

@end

NS_ASSUME_NONNULL_END