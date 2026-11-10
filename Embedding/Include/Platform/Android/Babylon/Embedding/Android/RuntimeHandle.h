#pragma once

#include <Babylon/Embedding/Runtime.h>

#include <jni.h>

#include <functional>
#include <string_view>

namespace Babylon::Embedding::Android
{
    // Convert a jlong handle from `runtimeCreate` back to a Runtime*.
    // The JNI layer wraps each Runtime in an internal struct that also
    // holds Activity-lifecycle tickets, so `reinterpret_cast<Runtime*>`
    // is wrong — hosts shipping their own JNI helpers must go through
    // this. Returns nullptr if handle is 0.
    Runtime* RuntimeFromHandle(jlong handle);

    // Replace the default logcat sink for Runtimes created by later
    // `runtimeCreate` calls. Hosts shipping their own JNI helpers use this
    // to observe console output (e.g. to forward it elsewhere); the callback
    // is responsible for any logcat output. Pass an empty function to restore
    // the default logcat sink.
    void SetLogCallback(std::function<void(LogLevel, std::string_view)> callback);
}