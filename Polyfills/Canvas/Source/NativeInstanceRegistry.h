#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <mutex>
#include <unordered_set>

namespace Babylon::Polyfills::Internal
{
    // Answers "is this JS object wrapping a T", for types whose native pointer is handed to
    // napi_unwrap. It exists because the obvious answers are all forgeable from script:
    //
    //   - `instanceof` walks the prototype chain, and a prototype is assignable. After
    //     Object.setPrototypeOf(gradient, Path2D.prototype) a CanvasGradient passes an
    //     InstanceOf test against Path2D, and unwrapping it reinterprets the gradient's
    //     native pointer as a path. That is an access violation, not a wrong answer.
    //   - Object.create(Path2D.prototype) passes the same test while having no native wrap
    //     at all.
    //   - A brand property, even under a symbol, is reachable via
    //     Object.getOwnPropertySymbols and can be copied onto any object.
    //   - napi_type_tag_object would be the idiomatic answer, but only the V8 port
    //     implements it; the Chakra, JavaScriptCore and QuickJS ports do not.
    //
    // So the authority is kept here in C++, where script cannot reach it: every live instance
    // records its own address, and a candidate is accepted only if its unwrapped pointer is
    // one of them. The pointer is compared, never dereferenced, before it is accepted, so a
    // foreign wrapped object is rejected rather than misread.
    template<typename T>
    class NativeInstanceRegistry
    {
    public:
        static void Add(const T* instance)
        {
            const std::scoped_lock lock{Mutex()};
            Instances().insert(instance);
        }

        static void Remove(const T* instance)
        {
            const std::scoped_lock lock{Mutex()};
            Instances().erase(instance);
        }

        // Returns the wrapped instance, or nullptr when `value` is not one. napi_unwrap is
        // called directly rather than through ObjectWrap::Unwrap because the latter throws for
        // an object that was never wrapped, and this has to answer "no" for any value at all.
        static T* TryUnwrap(Napi::Env env, const Napi::Value& value)
        {
            if (!value.IsObject())
            {
                return nullptr;
            }

            void* unwrapped{};
            if (napi_unwrap(env, value, &unwrapped) != napi_ok || unwrapped == nullptr)
            {
                return nullptr;
            }

            const std::scoped_lock lock{Mutex()};
            return Instances().count(unwrapped) != 0 ? static_cast<T*>(unwrapped) : nullptr;
        }

    private:
        // Function-local statics so this header needs no out-of-line definitions. The mutex
        // covers hosts running more than one JS environment, whose instances share these.
        static std::mutex& Mutex()
        {
            static std::mutex mutex{};
            return mutex;
        }

        static std::unordered_set<const void*>& Instances()
        {
            static std::unordered_set<const void*> instances{};
            return instances;
        }
    };
}
