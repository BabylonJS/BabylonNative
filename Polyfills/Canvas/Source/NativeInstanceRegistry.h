#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <napi/napi.h>
#include <mutex>
#include <unordered_set>

namespace Babylon::Polyfills::Internal
{
    // Brands a native ObjectWrap instance so TryUnwrap can reject forgeable instanceof /
    // unchecked ObjectWrap::Unwrap (V8 AV; QuickJS wrong pointer). Temporary until
    // JsRuntimeHost type tags land on all ports; then swap to CheckTypeTag.
    template<typename T>
    class NativeInstanceRegistry
    {
    public:
        // Register at end of ctor only — a throwing ctor never reaches the dtor.
        static void Add(const Napi::CallbackInfo& info, T* instance)
        {
            // Non-enumerable brand so it does not show up in Object.keys / for..in.
                    // Use the C napi_define_properties path: Napi::PropertyDescriptor is not
                    // available on every JsRuntimeHost port (notably UWP/JSI).
                    Napi::Object self = info.This().As<Napi::Object>();
                    Napi::External<T> brand = Napi::External<T>::New(info.Env(), instance);
                    napi_property_descriptor desc{};
                    desc.utf8name = BRAND_NAME;
                    desc.value = brand;
                    desc.attributes = napi_default; // non-enumerable, non-configurable, non-writable
                    napi_status status = napi_define_properties(info.Env(), self, 1, &desc);
                    if (status != napi_ok)
                    {
                        Napi::Error::New(info.Env(), "NativeInstanceRegistry: failed to brand instance").ThrowAsJavaScriptException();
                        return;
                    }

                    const std::scoped_lock lock{Mutex()};
                    Instances().insert(instance);
                }

        static void Remove(const T* instance)
        {
            const std::scoped_lock lock{Mutex()};
            Instances().erase(instance);
        }

        static T* TryUnwrap(Napi::Env env, const Napi::Value& value)
        {
            if (!value.IsObject())
            {
                return nullptr;
            }

            Napi::Value brand{env.Undefined()};
            try
            {
                brand = value.As<Napi::Object>().Get(BRAND_NAME);
            }
            catch (...)
            {
            }

            if (env.IsExceptionPending())
            {
                (void)env.GetAndClearPendingException();
            }

            if (!brand.IsExternal())
            {
                return nullptr;
            }

            T* const candidate = brand.As<Napi::External<T>>().Data();

            const std::scoped_lock lock{Mutex()};
            return Instances().count(candidate) != 0 ? candidate : nullptr;
        }

    private:
        static constexpr const char* BRAND_NAME{"__nativeInstance"};

        // Heap-allocated so a late finalizer during static teardown cannot touch a dead set.
        static std::mutex& Mutex()
        {
            static auto* mutex{new std::mutex{}};
            return *mutex;
        }

        static std::unordered_set<const void*>& Instances()
        {
            static auto* instances{new std::unordered_set<const void*>{}};
            return *instances;
        }
    };
}