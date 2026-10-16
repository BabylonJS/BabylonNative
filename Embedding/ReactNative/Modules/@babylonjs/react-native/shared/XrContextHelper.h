#pragma once

#if __has_include("jsi/jsi.h")
#include "jsi/jsi.h"
namespace Babylon::Plugins::NativeXr
{
    bool TryGetXrContext(facebook::jsi::Runtime& jsiRuntime, const std::string nativeXrContextType, uintptr_t& nativeXrContext)
    {
        nativeXrContext = reinterpret_cast<uintptr_t>(nullptr);
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
            nativeXr.getProperty(jsiRuntime, "nativeXrContextType").asString(jsiRuntime).utf8(jsiRuntime) != nativeXrContextType)
        {
            return false;
        }

        nativeXrContext = static_cast<uintptr_t>(nativeXr.getProperty(jsiRuntime, "nativeXrContext").asNumber());
        return true;
    }
}
#endif

#if __has_include("napi/env.h")
#include "napi/env.h"
namespace Babylon::Plugins::NativeXr
{
    bool TryGetXrContext(Napi::Env env, const std::string nativeXrContextType, uintptr_t& nativeXrContext)
    {
        nativeXrContext = reinterpret_cast<uintptr_t>(nullptr);
        if (!env.Global().Has("navigator"))
        {
            return false;
        }

        auto navigator{ env.Global().Get("navigator").As<Napi::Object>() };
        if (!navigator.Has("xr"))
        {
            return false;
        }

        auto nativeXr{ navigator.Get("xr").As<Napi::Object>() };
        if (!nativeXr.Has("nativeXrContext") ||
            !nativeXr.Has("nativeXrContextType") ||
            nativeXr.Get("nativeXrContextType").As<Napi::String>().Utf8Value() != nativeXrContextType)
        {
            return false;
        }

        nativeXrContext = static_cast<uintptr_t>(nativeXr.Get("nativeXrContext").As<Napi::Number>().DoubleValue());
        return true;
    }
}
#endif

#if __has_include("IXrContextOpenXR.h")
#include "IXrContextOpenXR.h"
#if __has_include("jsi/jsi.h")
namespace Babylon::Plugins::NativeXr
{
    bool TryGetXrContext(facebook::jsi::Runtime& jsiRuntime, IXrContextOpenXR*& xrContext)
    {
        xrContext = nullptr;
        uintptr_t nativePtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetXrContext(jsiRuntime, "OpenXR", nativePtr))
        {
            xrContext = reinterpret_cast<IXrContextOpenXR*>(nativePtr);
            return true;
        }

        return false;
    }
}
#endif
#if __has_include("napi/env.h")
namespace Babylon::Plugins::NativeXr
{
    bool TryGetXrContext(Napi::Env env, IXrContextOpenXR*& xrContext)
    {
        xrContext = nullptr;
        uintptr_t nativePtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetXrContext(env, "OpenXR", nativePtr))
        {
            xrContext = reinterpret_cast<IXrContextOpenXR*>(nativePtr);
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
    bool TryGetXrContext(facebook::jsi::Runtime& jsiRuntime, IXrContextARCore*& xrContext)
    {
        xrContext = nullptr;
        uintptr_t nativePtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetXrContext(jsiRuntime, "ARCore", nativePtr))
        {
            xrContext = reinterpret_cast<IXrContextARCore*>(nativePtr);
            return true;
        }
        
        return false;
    }
}
#endif
#if __has_include("napi/env.h")
namespace Babylon::Plugins::NativeXr
{
    bool TryGetXrContext(Napi::Env env, IXrContextARCore*& xrContext)
    {
        xrContext = nullptr;
        uintptr_t nativePtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetXrContext(env, "ARCore", nativePtr))
        {
            xrContext = reinterpret_cast<IXrContextARCore*>(nativePtr);
            return true;
        }
        
        return false;
    }
}
#endif
#endif

#if __has_include("IXrContextARKit.h")
#include "IXrContextARKit.h"
#if __has_include("jsi/jsi.h")
namespace Babylon::Plugins::NativeXr
{
    bool TryGetXrContext(facebook::jsi::Runtime& jsiRuntime, IXrContextARKit*& xrContext)
    {
        xrContext = nullptr;
        uintptr_t nativePtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetXrContext(jsiRuntime, "ARKit", nativePtr))
        {
            xrContext = reinterpret_cast<IXrContextARKit*>(nativePtr);
            return true;
        }
        
        return false;
    }
}
#endif
#if __has_include("napi/env.h")
namespace Babylon::Plugins::NativeXr
{
    bool TryGetXrContext(Napi::Env env, IXrContextARKit*& xrContext)
    {
        xrContext = nullptr;
        uintptr_t nativePtr{reinterpret_cast<uintptr_t>(nullptr)};
        if (TryGetXrContext(env, "ARKit", nativePtr))
        {
            xrContext = reinterpret_cast<IXrContextARKit*>(nativePtr);
            return true;
        }
        
        return false;
    }
}
#endif
#endif
