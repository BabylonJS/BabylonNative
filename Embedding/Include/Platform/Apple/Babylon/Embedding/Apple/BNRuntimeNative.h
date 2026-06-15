// Obj-C++ escape hatch for host-specific helpers layered on top of
// Babylon Native's Apple Embedding runtime.

#pragma once

#import <Babylon/Embedding/Apple/BNRuntime.h>

#ifdef __cplusplus

#include <Babylon/Embedding/Runtime.h>

namespace Babylon::Embedding::Apple
{
    // Convert a BNRuntime wrapper back to its underlying C++ Runtime.
    //
    // Hosts that ship small app-specific Obj-C++ helpers alongside the
    // Apple Embedding layer can use this to access C++ escape hatches
    // such as Runtime::RunOnJsThread without making those concepts part
    // of the Swift/Objective-C BNRuntime surface.
    //
    // Returns nullptr if runtime is nil.
    Runtime* RuntimeFromBNRuntime(BNRuntime* runtime);
}

#endif