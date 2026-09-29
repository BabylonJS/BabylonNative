#include <napi/env.h>
#include "js_native_api_chakra.h"
#include <jsrt.h>
#include <array>
#include <exception>
#include <memory>
#include <stdexcept>
#include <strsafe.h>

namespace
{
    std::array<napi_ref*, 6> CachedReferences(napi_env env)
    {
        auto& intrinsics{env->property_name_intrinsics};
        return {&intrinsics.object_constructor, &intrinsics.own_names,
                &intrinsics.own_descriptor, &intrinsics.prototype,
                &env->has_own_property_reference, &env->wrap_symbol_reference};
    }

    void ThrowIfFailed(JsErrorCode errorCode)
    {
        if (errorCode != JsErrorCode::JsNoError)
        {
            throw std::exception();
        }
    }

    napi_status ReleaseCachedReferences(napi_env env)
    {
        napi_status firstError{napi_ok};
        for (napi_ref* ref : CachedReferences(env))
        {
            if (*ref != nullptr)
            {
                const napi_status status{napi_delete_reference(env, *ref)};
                if (status == napi_ok)
                {
                    *ref = nullptr;
                }
                else if (firstError == napi_ok)
                {
                    firstError = status;
                }
            }
        }
        return firstError;
    }
}

namespace Napi
{
    Env Attach()
    {
        auto env_ptr{std::make_unique<napi_env__>()};
        try
        {
            JsValueRef global;
            ThrowIfFailed(JsGetGlobalObject(&global));
            JsPropertyIdRef propertyId;
            ThrowIfFailed(JsGetPropertyIdFromName(L"Object", &propertyId));
            JsValueRef object;
            ThrowIfFailed(JsGetProperty(global, propertyId, &object));
            JsValueRef prototype;
            ThrowIfFailed(JsGetPrototype(object, &prototype));
            ThrowIfFailed(JsGetPropertyIdFromName(L"hasOwnProperty", &propertyId));
            ThrowIfFailed(JsGetProperty(prototype, propertyId, &env_ptr->has_own_property_function));
            if (napi_create_reference(env_ptr.get(), reinterpret_cast<napi_value>(env_ptr->has_own_property_function), 1, &env_ptr->has_own_property_reference) != napi_ok)
            {
                throw std::runtime_error{"Napi::Attach: failed to retain hasOwnProperty"};
            }
            if (napi_shared::CapturePropertyNameIntrinsics(env_ptr.get(), env_ptr->property_name_intrinsics) != napi_ok)
            {
                throw std::runtime_error{"Napi::Attach: failed to capture property-name intrinsics"};
            }

            JsValueRef wrapSymbolDescription;
            ThrowIfFailed(JsPointerToString(L"BabylonNative_External", 22, &wrapSymbolDescription));
            JsValueRef wrapSymbol;
            ThrowIfFailed(JsCreateSymbol(wrapSymbolDescription, &wrapSymbol));
            if (napi_create_reference(env_ptr.get(), reinterpret_cast<napi_value>(wrapSymbol), 1, &env_ptr->wrap_symbol_reference) != napi_ok)
            {
                throw std::runtime_error{"Napi::Attach: failed to retain wrap symbol"};
            }
            ThrowIfFailed(JsGetPropertyIdFromSymbol(wrapSymbol, &env_ptr->wrap_property_id));

            return {env_ptr.release()};
        }
        catch (...)
        {
            if (ReleaseCachedReferences(env_ptr.get()) != napi_ok)
            {
                std::throw_with_nested(std::runtime_error{"Napi::Attach: failed to release cached references"});
            }
            throw;
        }
    }

    void Detach(Env env)
    {
        napi_env env_ptr{env};
        for (napi_ref* ref : CachedReferences(env_ptr))
        {
            if (*ref != nullptr)
            {
                napi_chakra_internal::DiscardReferenceAfterRuntimeDisposal(*ref);
                *ref = nullptr;
            }
        }
        delete env_ptr;
    }
}
