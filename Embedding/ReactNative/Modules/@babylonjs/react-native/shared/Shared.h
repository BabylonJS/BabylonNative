#pragma once

#include <napi/napi.h>
#include <jsi/jsi.h>

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

// On iOS, directly calling flushedQueue breaks some kind of internal UI state and we start getting errors like:
// *** Assertion failure in -[RCTNativeAnimatedNodesManager disconnectAnimatedNodes:childTag:](), node_modules/react-native/Libraries/NativeAnimation/RCTNativeAnimatedNodesManager.m:138
// [native] Exception thrown while executing UI block: 'parentNode' is a required parameter
// However, calling setTimeout eventually also calls flushedQueue, but doesn't result in the above error,
// so we'll go this route until we (hopefully) get a better resolution from the React Native team at Facebook.
inline std::shared_ptr<facebook::jsi::Function> GetSetTimeout(facebook::jsi::Runtime& rt)
{
    auto code{std::make_shared<const facebook::jsi::StringBuffer>("() => setTimeout(() => {})")};
    return std::make_shared<facebook::jsi::Function>(rt.evaluateJavaScript(std::move(code), "").asObject(rt).asFunction(rt));
}
