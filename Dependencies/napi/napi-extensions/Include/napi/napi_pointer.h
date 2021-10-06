#pragma once

#include <napi/napi.h>

#include <array>
#include <type_traits>

namespace Napi
{
    template<typename NativePointerT>
    struct PointerTraits
    {
        using RepresentationT = uint32_t;
        using ArraySizeT = typename std::conditional<sizeof(NativePointerT) % sizeof(RepresentationT) == 0, 
            std::integral_constant<size_t, sizeof(NativePointerT) / sizeof(RepresentationT)>, 
            std::integral_constant<size_t, sizeof(NativePointerT) / sizeof(RepresentationT) + 1>>::type;
        using ByteSizeT = std::integral_constant<size_t, sizeof(RepresentationT) * ArraySizeT{}>;
        static inline constexpr size_t ArraySize{ArraySizeT{}};
        static inline constexpr size_t ByteSize{ByteSizeT{}};
    };

    template<typename PointerT>
    auto NapiPointerDeleter(PointerT pointer)
    {
        return [pointer]()
        {
            delete pointer;
        };
    }

    class FunctionPointer
    {
    public:
        template<typename ClassT, typename ReturnT, typename... ArgsT>
        static Napi::Value Create(Napi::Env env, ReturnT (ClassT::*functionPtr)(ArgsT...))
        {
            using PointerT = decltype(functionPtr);
            auto arrayBuffer = Napi::ArrayBuffer::New(env, PointerTraits<PointerT>::ByteSize);
            std::memcpy(arrayBuffer.Data(), &functionPtr, sizeof(PointerT));
            return Napi::Uint32Array::New(env, PointerTraits<PointerT>::ArraySize, arrayBuffer, 0).template As<Napi::Value>();
        }
    };

    template<typename T>
    class Pointer
    {
    public:
        static Napi::Value Create(Napi::Env env, const T* pointer)
        {
            using PointerT = T*;
            auto arrayBuffer = Napi::ArrayBuffer::New(env, PointerTraits<PointerT>::ByteSize);
            std::memcpy(arrayBuffer.Data(), &pointer, sizeof(PointerT));
            return Napi::Uint32Array::New(env, PointerTraits<PointerT>::ArraySize, arrayBuffer, 0).template As<Napi::Value>();
        }

        template<typename CallableT>
        static Napi::Value Create(Napi::Env env, const T* pointer, CallableT&& finalizationCallback)
        {
            using PointerT = T*;
            using DataT = std::array<uint32_t, PointerTraits<PointerT>::ArraySize>;
            auto finalizer = [callback = std::forward<CallableT>(finalizationCallback)](Napi::Env, void* ptr)
            {
                callback();
                delete reinterpret_cast<DataT*>(ptr);
            };
            auto arrayBuffer = Napi::ArrayBuffer::New(env, new DataT, PointerTraits<PointerT>::ByteSize, std::move(finalizer));
            std::memcpy(arrayBuffer.Data(), &pointer, sizeof(PointerT));
            return Napi::Uint32Array::New(env, PointerTraits<PointerT>::ArraySize, arrayBuffer, 0).template As<Napi::Value>();
        }

        template<typename EnvT, typename ValueT>
        Pointer(EnvT&& env, ValueT&& value)
            : m_pointer{*reinterpret_cast<T**>(Napi::Value(std::forward<EnvT>(env), std::forward<ValueT>(value)).As<Uint32Array>().Data())}
        {
        }

        T* Get() const
        {
            return m_pointer;
        }

        T& operator *() const
        {
            return *m_pointer;
        }

    private:
        T* m_pointer;
    };
}
