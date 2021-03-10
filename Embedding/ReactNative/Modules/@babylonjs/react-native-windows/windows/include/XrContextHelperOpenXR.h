#pragma once

#if __has_include("jsi/jsi.h")
#include "jsi/jsi.h"
#include "IXrContextOpenXR.h"

namespace BabylonReactNative
{
    bool TryGetXrContext(facebook::jsi::Runtime& jsiRuntime, const facebook::jsi::Value& xrSession, IXrContextOpenXR*& xrContext)
    {
        xrContext = nullptr;
        if (!xrSession.isObject() ||
            !xrSession.asObject(jsiRuntime).hasProperty(jsiRuntime, "nativeXrContext") ||
            !xrSession.asObject(jsiRuntime).hasProperty(jsiRuntime, "nativeXrContextType") ||
            xrSession.asObject(jsiRuntime).getProperty(jsiRuntime, "nativeXrContextType").asString(jsiRuntime).utf8(jsiRuntime) != "OpenXR")
        {
            return false;
        }

        auto nativeExtensionPtr = static_cast<uintptr_t>(xrSession.asObject(jsiRuntime).getProperty(jsiRuntime, "nativeXrContext").asNumber());
        xrContext = reinterpret_cast<IXrContextOpenXR*>(nativeExtensionPtr);
        return true;
    }
}
#endif