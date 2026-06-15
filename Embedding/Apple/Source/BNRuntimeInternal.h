// Internal Obj-C category exposing the underlying
// `Babylon::Embedding::Runtime*` to BNView.mm. Not part of the
// public Apple interop layer surface (Swift consumers don't see this).

#pragma once

#import <Babylon/Embedding/Apple/BNRuntime.h>

#include <Babylon/Embedding/Runtime.h>

NS_ASSUME_NONNULL_BEGIN

@interface BNRuntime ()
- (Babylon::Embedding::Runtime*)nativeRuntime;

/// Toggle the XR overlay view's visibility based on the runtime's
/// current XR-active state. Called by BNView once per frame so the
/// host doesn't have to manage XR overlay visibility itself.
- (void)updateXrViewIfNeeded;
@end

NS_ASSUME_NONNULL_END
