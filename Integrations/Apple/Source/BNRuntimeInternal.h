// Internal Obj-C category exposing the underlying
// `Babylon::Integrations::Runtime*` to BNView.mm. Not part of the
// public Apple interop layer surface (Swift consumers don't see this).

#pragma once

#import <BabylonNativeIntegrations/BNRuntime.h>

#include <Babylon/Integrations/Runtime.h>

NS_ASSUME_NONNULL_BEGIN

@interface BNRuntime ()
- (Babylon::Integrations::Runtime*)nativeRuntime;
@end

NS_ASSUME_NONNULL_END
