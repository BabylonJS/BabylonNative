// Swift-Obj-C++ bridging header for the visionOS Playground app.
//
// Exposes both the Babylon::Integrations Apple interop layer
// (`BNRuntime`, `BNView`) and the Playground-specific helper
// (`PlaygroundBootstrap`) to the Swift sources.

#pragma once

#import <Babylon/Integrations/Apple/BabylonNativeIntegrations.h>

#import "AppleShared/PlaygroundBootstrap.h"
