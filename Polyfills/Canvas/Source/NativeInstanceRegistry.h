#pragma once

#include "Canvas.h"
#include <napi/napi.h>
#include <mutex>
#include <unordered_map>
#include <utility>

namespace Babylon::Polyfills::Internal
{
    // Pointer brands are lookup hints; private weak identities authorize unwrapping.
    template<typename T>
    class NativeInstanceRegistry
    {
    public:
        // Register at end of ctor only — a throwing ctor never reaches the dtor.
        static void Add(const Napi::CallbackInfo& info, T* instance)
        {
            // Use Object.defineProperty for immutable, non-enumerable brands on all JsRuntimeHost ports.
            Napi::Object self = info.This().As<Napi::Object>();
            Napi::Object descriptor = Napi::Object::New(info.Env());
            descriptor.Set("value", Napi::External<T>::New(info.Env(), instance));
            Napi::Object object = info.Env().Global().Get("Object").As<Napi::Object>();
            object.Get("defineProperty").As<Napi::Function>().Call(
                object,
                {self, Napi::String::New(info.Env(), BRAND_NAME), descriptor});

            Identity identity{self};
            const std::scoped_lock lock{Mutex()};
            Instances().try_emplace(instance, std::move(identity));
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
            const auto entry = Instances().find(candidate);
            return entry != Instances().end() && entry->second.Matches(env, value) ? candidate : nullptr;
        }

    private:
        static constexpr const char* BRAND_NAME{"__nativeInstance"};

        struct Identity
        {
            napi_env Env;
            Canvas::Impl::WeakIdentity Receiver;

            explicit Identity(const Napi::Object& self)
                : Env{self.Env()}
                , Receiver{Canvas::Impl::GetFromJavaScript(self.Env()).CreateWeakIdentity(self)}
            {
            }

            bool Matches(Napi::Env env, const Napi::Value& value) const
            {
                return env == Env && Receiver.Has.Call(Receiver.Receivers.Value(), {value}).template As<Napi::Boolean>().Value();
            }
        };

        // Keep storage available to late finalizers during static teardown.
        static std::mutex& Mutex()
        {
            static auto* mutex{new std::mutex{}};
            return *mutex;
        }

        static std::unordered_map<const void*, Identity>& Instances()
        {
            static auto* instances{new std::unordered_map<const void*, Identity>{}};
            return *instances;
        }
    };
}
