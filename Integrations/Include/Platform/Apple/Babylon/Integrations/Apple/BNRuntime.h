// BNRuntime.h — public Obj-C interface for the Babylon::Integrations
// runtime on Apple platforms (iOS, macOS, visionOS).
//
// Swift consumers see this through the auto-generated Swift bridge
// (BNRuntime is exposed to Swift as `BNRuntime`).

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

/// Constructs the runtime: starts the JS engine/thread and sets up
/// non-GPU polyfills/plugins. The GPU Device is deferred to the first
/// `BNView` attach. Default options: debugger off, DebugTrace off,
/// log routes to `os_log`.
- (instancetype)init;

/// Constructs the runtime with platform-friendly options (nil = same
/// defaults as `init`).
///
/// `options.shaderCachePath`: loaded on first BNView attach; saved on
/// `suspend` and on deallocation. Raises
/// `BabylonNativePluginNotEnabledException` when non-nil but the
/// native library was built without `BABYLON_NATIVE_PLUGIN_SHADERCACHE`.
- (instancetype)initWithOptions:(nullable BNRuntimeOptions*)options NS_DESIGNATED_INITIALIZER;

/// Load a script from a URL onto the JS thread. Calls made before the
/// first `BNView` is created are queued and dispatched after engine
/// init completes during that first attach; calls after first attach
/// are dispatched immediately.
- (void)loadScript:(NSString*)url;

/// Evaluate JavaScript on the JS thread. Same queueing as `loadScript`.
- (void)eval:(NSString*)source sourceURL:(NSString*)sourceURL;

/// Reference-counted suspend. While suspended, JS timers pause and
/// any attached `BNView` becomes a no-op for `renderFrame`.
- (void)suspend;

/// Decrement the suspend count; resume when it reaches zero.
- (void)resume;

/// Whether the runtime is currently suspended.
@property (nonatomic, readonly, getter=isSuspended) BOOL suspended;

/// Set the view that XR will render into (typically a transparent
/// MTKView overlay distinct from the main view's Metal layer). Pass
/// `nil` to clear. Safe to call before the first `BNView` attach;
/// applied when NativeXr finishes initializing.
///
/// Raises `BabylonNativePluginNotEnabledException` when
/// `BABYLON_NATIVE_PLUGIN_NATIVEXR` is not enabled at native build time.
- (void)setXrView:(nullable MTKView*)xrView;

/// `YES` while an XR session is active. Safe to poll from any thread.
/// Returns `NO` when `BABYLON_NATIVE_PLUGIN_NATIVEXR` is not enabled.
@property (nonatomic, readonly, getter=isXRActive) BOOL xrActive;

@end

NS_ASSUME_NONNULL_END