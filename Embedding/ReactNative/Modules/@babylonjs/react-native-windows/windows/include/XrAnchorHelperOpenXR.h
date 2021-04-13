#pragma once

#if __has_include("jsi/jsi.h")
#include "jsi/jsi.h"
#include <openxr/openxr.h>

namespace BabylonReactNative
{
    bool TryGetNativeAnchor(facebook::jsi::Runtime& jsiRuntime, facebook::jsi::Value& jsAnchor, XrSpatialAnchorMSFT& nativeAnchor)
    {
        nativeAnchor = nullptr;
        if (!jsAnchor.isObject())
        {
            return false;
        }

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
        if (!nativeXr.hasProperty(jsiRuntime, "getNativeAnchor"))
        {
            return false;
        }

        auto getNativeAnchor{nativeXr.getPropertyAsFunction(jsiRuntime, "getNativeAnchor")};
        auto nativeAnchorPtr{static_cast<uintptr_t>(getNativeAnchor.call(jsiRuntime, { jsAnchor.asObject(jsiRuntime) }).asNumber())};

        nativeAnchor = reinterpret_cast<XrSpatialAnchorMSFT>(nativeAnchorPtr);
        return true;
    }
}
#endif
