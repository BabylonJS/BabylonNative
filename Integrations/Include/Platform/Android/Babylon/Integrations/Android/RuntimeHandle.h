#pragma once

#include <Babylon/Integrations/Runtime.h>

#include <jni.h>

namespace Babylon::Integrations::Android
{
    // Convert a jlong handle from `runtimeCreate` back to a Runtime*.
    // The JNI layer wraps each Runtime in an internal struct that also
    // holds Activity-lifecycle tickets, so `reinterpret_cast<Runtime*>`
    // is wrong — hosts shipping their own JNI helpers must go through
    // this. Returns nullptr if handle is 0.
    Runtime* RuntimeFromHandle(jlong handle);
}