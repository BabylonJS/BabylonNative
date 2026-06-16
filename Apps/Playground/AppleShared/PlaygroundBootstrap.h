// PlaygroundBootstrap.h — Obj-C helper exposed to Swift via the
// bridging header on iOS, and used directly from Obj-C++ on macOS.
// Single class method that hands a freshly-created `BNRuntime` to
// `Apps/Playground/Shared/PlaygroundScripts.cpp`, which loads the
// Babylon.js bootstrap script list shared with the other Playground
// hosts (Win32, Android, …).

#pragma once

#import <Foundation/Foundation.h>

@class BNRuntime;

NS_ASSUME_NONNULL_BEGIN

@interface PlaygroundBootstrap : NSObject

/// Performs process-wide Playground setup (PerfTrace level, …) and
/// queues the Babylon.js bootstrap scripts onto `runtime`. Idempotent;
/// safe to call multiple times.
+ (void)loadScripts:(BNRuntime*)runtime;

@end

NS_ASSUME_NONNULL_END
