// Swift-Obj-C bridging header for the iOS Playground app.
//
// Exposes both the Babylon::Embedding Apple interop layer
// (`BNRuntime`, `BNView`) and the Playground-specific helper
// (`PlaygroundBootstrap`) to the Swift sources.

#pragma once

#import <Babylon/Embedding/Apple/BabylonNativeEmbedding.h>

#import "AppleShared/PlaygroundBootstrap.h"
