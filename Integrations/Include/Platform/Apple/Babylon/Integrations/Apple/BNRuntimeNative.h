// Obj-C++ escape hatch for host-specific helpers layered on top of
// Babylon Native's Apple Integrations runtime.

#pragma once

#import <Babylon/Integrations/Apple/BNRuntime.h>

#ifdef __cplusplus

#include <Babylon/Integrations/Runtime.h>

namespace Babylon::Integrations::Apple
{
    // Convert a BNRuntime wrapper back to its underlying C++ Runtime.
    //
    // Hosts that ship small app-specific Obj-C++ helpers alongside the
    // Apple Integrations layer can use this to access C++ escape hatches
    // such as Runtime::RunOnJsThread without making those concepts part
    // of the Swift/Objective-C BNRuntime surface.
    //
    // Returns nullptr if runtime is nil.
    Runtime* RuntimeFromBNRuntime(BNRuntime* runtime);
}

#endif