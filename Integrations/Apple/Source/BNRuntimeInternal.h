// Internal Obj-C category exposing the underlying
// `Babylon::Integrations::Runtime*` to BNView.mm. Not part of the
// public Apple interop layer surface (Swift consumers don't see this).

#pragma once

#import <Babylon/Integrations/Apple/BNRuntime.h>

#include <Babylon/Integrations/Runtime.h>

NS_ASSUME_NONNULL_BEGIN

@interface BNRuntime ()
- (Babylon::Integrations::Runtime*)nativeRuntime;

/// Toggle the XR overlay view's visibility based on the runtime's
/// current XR-active state. Called by BNView once per frame so the
/// host doesn't have to manage XR overlay visibility itself.
- (void)updateXrViewIfNeeded;
@end

NS_ASSUME_NONNULL_END
