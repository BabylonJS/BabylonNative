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

@interface BNRuntimeOptions : NSObject

/// Optional MSAA sample count for the back buffer. Valid values are 0, 2, 4, 8, and 16.
/// Defaults to the shared C++ RuntimeOptions value when nil.
@property (nonatomic, strong, nullable) NSNumber* msaaSamples;

/// Enable the JavaScript debugger when supported by the configured JS engine.
/// Defaults to NO, matching the shared C++ RuntimeOptions default.
@property (nonatomic) BOOL enableDebugger;

/// Enable Babylon::DebugTrace output through the default log sink.
/// Defaults to NO, matching the shared C++ RuntimeOptions default.
@property (nonatomic) BOOL enableDebugTrace;

/// Block engine startup until a debugger attaches when supported by the configured JS engine.
/// Defaults to NO, matching the shared C++ RuntimeOptions default.
@property (nonatomic) BOOL waitForDebugger;

/// Optional writable file path for a persistent on-disk GPU shader cache. Defaults to nil.
@property (nonatomic, copy, nullable) NSString* shaderCachePath;

@end

@interface BNRuntime : NSObject

/// Constructs the runtime: starts the JS engine + thread, sets up
/// non-GPU polyfills and plugins. Cheap and synchronous; no GPU
/// device is created yet (that happens on the first `BNView` attach).
/// Default options: JS debugger off, DebugTrace off, log routes to `NSLog`.
- (instancetype)init;

/// Constructs the runtime with platform-friendly options. Pass nil to use
/// the same defaults as `init`.
///
/// If `options.shaderCachePath` is non-`nil`, the cache is loaded on first
/// `BNView` attach and saved on `suspend` and on deallocation.
///
/// If `options.shaderCachePath` is non-`nil` but the native library was built
/// without `BABYLON_NATIVE_PLUGIN_SHADERCACHE`, this initializer raises an
/// `NSException` (name
/// `BabylonNativePluginNotEnabledException`) so the misconfiguration
/// surfaces at construction time rather than silently dropping the
/// cache. Passing nil options, or options with nil `shaderCachePath`, is
/// always safe regardless of build config.
- (instancetype)initWithOptions:(nullable BNRuntimeOptions*)options NS_DESIGNATED_INITIALIZER;

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