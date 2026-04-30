// BNRuntime.h — public Obj-C interface for the Babylon::Integrations
// runtime on Apple platforms (iOS, macOS, visionOS).
//
// Swift consumers see this through the auto-generated Swift bridge
// (BNRuntime is exposed to Swift as `BNRuntime`).
//
// See SimplifiedAPI.md §4.2 / §5 for the design and usage examples.

#pragma once

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface BNRuntime : NSObject

/// Constructs the runtime: starts the JS engine + thread, sets up
/// non-GPU polyfills and plugins. Cheap and synchronous; no GPU
/// device is created yet (that happens on the first `BNView` attach).
- (instancetype)init;

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

@end

NS_ASSUME_NONNULL_END
