#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <mutex>
#include <unordered_set>

namespace Babylon::Polyfills::Internal
{
    // Answers "is this JS object wrapping a T", and hands back the instance.
    //
    // `instanceof` cannot answer it: a prototype is assignable, so
    // Object.setPrototypeOf(gradient, Path2D.prototype) makes a gradient pass a Path2D test,
    // and Object.create(Path2D.prototype) passes with no native object behind it at all.
    //
    // Neither can ObjectWrap::Unwrap. Both JsRuntimeHost Node-API ports break the contract
    // that it fails for an object that was never wrapped: the V8 port dereferences internal
    // field 0 unconditionally, which access-violates, and the QuickJS port falls back to
    // walking the prototype chain, which returns some other object's pointer.
    //
    // So each instance brands its own JS object with an External holding its address, and a
    // candidate is accepted only if that address is still registered here. An External is
    // opaque to script, and the address is compared, never dereferenced, before it is
    // accepted, so both spoofs above are rejected instead of being misread as a T.
    //
    // Script can still copy a brand off a real instance onto another object. That is not a
    // memory-safety hole: the result is the live instance the brand came from, which script
    // had to already hold. A brand left over from a collected instance is rejected, since the
    // destructor unregisters the address.
    template<typename T>
    class NativeInstanceRegistry
    {
    public:
        // Call at the very end of the constructor: one that throws never reaches the
        // destructor, which would leave a dangling address registered.
        static void Add(const Napi::CallbackInfo& info, T* instance)
        {
            info.This().As<Napi::Object>().Set(BRAND_NAME, Napi::External<T>::New(info.Env(), instance));

            const std::scoped_lock lock{Mutex()};
            Instances().insert(instance);
        }

        static void Remove(const T* instance)
        {
            const std::scoped_lock lock{Mutex()};
            Instances().erase(instance);
        }

        // Returns the wrapped instance, or nullptr when `value` is not one.
        static T* TryUnwrap(Napi::Env env, const Napi::Value& value)
        {
            if (!value.IsObject())
            {
                return nullptr;
            }

            // `value` is arbitrary, so the read can run a script accessor that throws.
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
        // Shared by every T: a brand read as the wrong type is still rejected, because each T
        // registers into its own set.
        static constexpr const char* BRAND_NAME{"__nativeInstance"};

        // Never destroyed, so that an instance finalized during static destruction cannot
        // erase itself from an already-destroyed set.
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
