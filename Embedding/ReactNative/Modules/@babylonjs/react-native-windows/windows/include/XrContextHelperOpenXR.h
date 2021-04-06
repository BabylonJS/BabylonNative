#pragma once

#if __has_include("jsi/jsi.h")
#include "jsi/jsi.h"
#include "IXrContextOpenXR.h"

namespace BabylonReactNative
{
    bool TryGetXrContext(facebook::jsi::Runtime& jsiRuntime, IXrContextOpenXR*& xrContext)
    {
        xrContext = nullptr;
        if (!jsiRuntime.global().hasProperty(jsiRuntime, "navigator"))
        {
            return false;
        }

        auto navigator{ jsiRuntime.global().getProperty(jsiRuntime, "navigator").asObject(jsiRuntime) };
        if (!navigator.hasProperty(jsiRuntime, "xr"))
        {
            return false;
        }

        auto nativeXr{ navigator.getProperty(jsiRuntime, "xr").asObject(jsiRuntime) };
        if (!nativeXr.hasProperty(jsiRuntime, "nativeXrContext") ||
            !nativeXr.hasProperty(jsiRuntime, "nativeXrContextType") ||
            nativeXr.getProperty(jsiRuntime, "nativeXrContextType").asString(jsiRuntime).utf8(jsiRuntime) != "OpenXR")
        {
            return false;
        }

        auto nativeExtensionPtr = static_cast<uintptr_t>(nativeXr.getProperty(jsiRuntime, "nativeXrContext").asNumber());
        xrContext = reinterpret_cast<IXrContextOpenXR*>(nativeExtensionPtr);
        return true;
    }
}
#endif