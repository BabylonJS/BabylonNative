#pragma once

#include <Babylon/Integrations/Runtime.h>

#include <jni.h>

namespace Babylon::Integrations::Android
{
    // Convert an opaque jlong handle (as returned by `runtimeCreate` in
    // `BabylonNativeIntegrations.cpp`) back to a Runtime pointer.
    //
    // The Android JNI layer wraps each Runtime in an internal struct
    // that also holds Activity-lifecycle event tickets, so a direct
    // `reinterpret_cast<Runtime*>(handle)` is incorrect — hosts that
    // ship their own JNI helpers alongside `libBabylonNativeIntegrations.so`
    // (e.g. for app-specific bootstrap routines) must go through this
    // function to resolve the handle correctly.
    //
    // Returns nullptr if `handle` is 0.
    Runtime* RuntimeFromHandle(jlong handle);
}
