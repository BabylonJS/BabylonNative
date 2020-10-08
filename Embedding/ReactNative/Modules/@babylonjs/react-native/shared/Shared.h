#pragma once

#include <napi/napi.h>

// See https://github.com/BabylonJS/BabylonReactNative/issues/60 for original issue.
// This is a work around to poke the React Native message queue to run setImmediate callbacks.
// React Native uses a custom promise implementation based on setImmediate. The promise
// continuations will only continue once these setImmediate callbacks are triggered by the
// flushedQueue call. This is explicitly called at the end of JsRuntime's dispatch function
// to flush the queue.
inline Napi::FunctionReference GetFlushedQueue(Napi::Env env)
{
    // HACK: The __fbBatchedBridge global is an internal implementation of React Native.
    // This hack will break if React Native internals changes, but it should blow up right here.
    auto batchedBridge{ env.Global().Get("__fbBatchedBridge").As<Napi::Object>() };
    auto flushedQueue{ batchedBridge.Get("flushedQueue").As<Napi::Function>() };
    return Napi::Persistent(flushedQueue);
}
