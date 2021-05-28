#pragma once

#if __has_include("jsi/jsi.h")
#include "jsi/jsi.h"
namespace Babylon::Plugins::NativeXr
{
    bool TryGetNativeAnchor(facebook::jsi::Runtime& jsiRuntime, facebook::jsi::Value& jsAnchor, uintptr_t& nativeAnchorPtr)
    {
        nativeAnchorPtr = reinterpret_cast<uintptr_t>(nullptr);
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
        nativeAnchorPtr = static_cast<uintptr_t>(getNativeAnchor.call(jsiRuntime, { jsAnchor.asObject(jsiRuntime) }).asNumber());
        return true;
    }
}
#endif

#if __has_include("napi/env.h")
#include "napi/env.h"
namespace Babylon::Plugins::NativeXr
{
    bool TryGetNativeAnchor(Napi::Env env, Napi::Value anchor, uintptr_t& nativeAnchorPtr)
    {
        nativeAnchorPtr = reinterpret_cast<uintptr_t>(nullptr);
        if (!anchor.IsObject())
        {
            return false;
        }

        if (!env.Global().Has("navigator"))
        {
            return false;
        }

        auto navigator{ env.Global().Get("navigator").ToObject() };
        if (!navigator.Has("xr"))
        {
            return false;
        }

        auto nativeXr{ navigator.Get("xr").ToObject() };
        if (!nativeXr.Has("getNativeAnchor"))
        {
            return false;
        }

        auto getNativeAnchor{nativeXr.Get("getNativeAnchor").As<Napi::Function>()};
        nativeAnchorPtr = static_cast<uintptr_t>(getNativeAnchor.Call({ anchor }).As<Napi::Number>().DoubleValue());
        return true;
    }
}
#endif

#if __has_include("IXrContextOpenXR.h")
#include "IXrContextOpenXR.h"
#if __has_include("jsi/jsi.h")
namespace Babylon::Plugins::NativeXr
{
    bool TryGetNativeAnchor(facebook::jsi::Runtime& jsiRuntime, facebook::jsi::Value& jsAnchor, XrSpatialAnchorMSFT& nativeAnchor)
    {
        nativeAnchor = nullptr;
        uintptr_t nativeAnchorPtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetNativeAnchor(jsiRuntime, jsAnchor, nativeAnchorPtr))
        {
            nativeAnchor = reinterpret_cast<XrSpatialAnchorMSFT>(nativeAnchorPtr);
            return true;
        }

        return false;
    }
}
#endif
#if __has_include("napi/env.h")
namespace Babylon::Plugins::NativeXr
{
    bool TryGetNativeAnchor(Napi::Env env, Napi::Value anchor, XrSpatialAnchorMSFT& nativeAnchor)
    {
        nativeAnchor = nullptr;
        uintptr_t nativeAnchorPtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetNativeAnchor(env, anchor, nativeAnchorPtr))
        {
            nativeAnchor = reinterpret_cast<XrSpatialAnchorMSFT>(nativeAnchorPtr);
            return true;
        }

        return false;
    }
}
#endif
#endif

#if __has_include("IXrContextARCore.h")
#include "IXrContextARCore.h"
#if __has_include("jsi/jsi.h")
namespace Babylon::Plugins::NativeXr
{
    bool TryGetNativeAnchor(facebook::jsi::Runtime& jsiRuntime, facebook::jsi::Value& jsAnchor, ArAnchor*& nativeAnchor)
    {
        nativeAnchor = nullptr;
        uintptr_t nativeAnchorPtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetNativeAnchor(jsiRuntime, jsAnchor, nativeAnchorPtr))
        {
            nativeAnchor = reinterpret_cast<ArAnchor*>(nativeAnchorPtr);
            return true;
        }

        return false;
    }
}
#endif
#if __has_include("napi/env.h")
namespace Babylon::Plugins::NativeXr
{
    bool TryGetNativeAnchor(Napi::Env env, Napi::Value anchor, ArAnchor*& nativeAnchor)
    {
        nativeAnchor = nullptr;
        uintptr_t nativeAnchorPtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetNativeAnchor(env, anchor, nativeAnchorPtr))
        {
            nativeAnchor = reinterpret_cast<ArAnchor*>(nativeAnchorPtr);
            return true;
        }

        return false;
    }
}
#endif
#endif
