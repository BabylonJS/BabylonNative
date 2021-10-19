#pragma once

#include <Babylon/JsRuntime.h>

#include <napi/env.h>

#include <gsl/gsl>

namespace Babylon
{
    class NativeDataStream final : public Napi::ObjectWrap<NativeDataStream>
    {
        static constexpr auto JS_CLASS_NAME = "_NativeDataStream";
        static constexpr auto JS_ENGINE_CONSTRUCTOR_NAME = "NativeDataStream";

        static constexpr auto VALIDATION_ENABLED = false;

        enum class ValidationType : uint32_t
        {
            Uint32,
            Int32,
            Float32,
            Uint32Array,
            Int32Array,
            Float32Array,
            NativeData,
            Boolean,
        };

        template<ValidationType T, typename ReaderT>
        static inline void Validate(ReaderT& reader)
        {
            if constexpr (VALIDATION_ENABLED)
            {
                uint32_t value{reader.template Read<uint32_t>()};
                if (value != static_cast<uint32_t>(T))
                {
                    throw std::runtime_error{"Data stream validation error: data type mismatch."};
                }
            }
        }

    public:
        class Reader final
        {
        public:
            Reader(const Reader&) = delete;
            Reader(Reader&&) = delete;
            Reader operator=(const Reader&) = delete;
            Reader operator=(const Reader&&) = delete;

            bool CanRead() const
            {
                assert(m_position <= static_cast<size_t>(m_buffer.size()));
                return m_position < static_cast<size_t>(m_buffer.size());
            }

            uint32_t ReadUint32()
            {
                Validate<ValidationType::Uint32>(*this);
                return Read<uint32_t>();
            }

            int32_t ReadInt32()
            {
                Validate<ValidationType::Int32>(*this);
                return Read<int32_t>();
            }

            float ReadFloat32()
            {
                Validate<ValidationType::Float32>(*this);
                return Read<float>();
            }

            gsl::span<uint32_t> ReadUint32Array()
            {
                Validate<ValidationType::Uint32Array>(*this);
                return ReadArray<uint32_t>();
            }

            gsl::span<int32_t> ReadInt32Array()
            {
                Validate<ValidationType::Int32Array>(*this);
                return ReadArray<int32_t>();
            }

            gsl::span<float> ReadFloat32Array()
            {
                Validate<ValidationType::Float32Array>(*this);
                return ReadArray<float>();
            }

            template<typename T>
            T ReadNativeData()
            {
                Validate<ValidationType::NativeData>(*this);
                static_assert(sizeof(T) % 4 == 0);
                auto span = gsl::make_span(reinterpret_cast<uint32_t*>(m_buffer.data() + m_position), sizeof(T) / 4);
                m_position += sizeof(T) / 4;
                return *reinterpret_cast<T*>(span.data());
            }

            template<typename T, class = typename std::enable_if<!std::is_pointer<T>::value>::type>
            auto ReadPointer()
            {
                return ReadNativeData<typename std::conditional<std::is_member_pointer<T>::value, T, T*>::type>();
            }

        private:
            gsl::span<uint32_t> m_buffer{};
            size_t m_position{0};
            const gsl::final_action<std::function<void()>> m_scopeGuard;

            friend class NativeDataStream;

            template<typename CallableT>
            Reader(gsl::span<uint32_t> buffer, CallableT&& callable)
                : m_buffer{buffer}
                , m_scopeGuard{std::forward<CallableT>(callable)}
            {
            }

            template<typename T>
            T Read()
            {
                static_assert(sizeof(T) % 4 == 0);
                T t{*reinterpret_cast<T*>(m_buffer.data() + m_position)};
                m_position += sizeof(T) / 4;
                return t;
            }

            template<typename T>
            gsl::span<T> ReadArray()
            {
                static_assert(sizeof(T) % 4 == 0);

                // The first 32 bit number is a length
                uint32_t length = Read<uint32_t>();

                auto span = gsl::make_span<T>(reinterpret_cast<T*>(m_buffer.data() + m_position), length * (sizeof(T) / 4));
                m_position += length;
                return span;
            }
        };

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            if constexpr (VALIDATION_ENABLED)
            {
                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("writeBuffer", &NativeDataStream::WriteBuffer),

                        StaticValue("VALIDATION_ENABLED", Napi::Boolean::From(env, VALIDATION_ENABLED)),
                        StaticValue("VALIDATION_UINT_32", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Uint32))),
                        StaticValue("VALIDATION_INT_32", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Int32))),
                        StaticValue("VALIDATION_FLOAT_32", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Float32))),
                        StaticValue("VALIDATION_UINT_32_ARRAY", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Uint32Array))),
                        StaticValue("VALIDATION_INT_32_ARRAY", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Int32Array))),
                        StaticValue("VALIDATION_FLOAT_32_ARRAY", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Float32Array))),
                        StaticValue("VALIDATION_NATIVE_DATA", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::NativeData))),
                        StaticValue("VALIDATION_BOOLEAN", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Boolean))),
                    });
                JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_ENGINE_CONSTRUCTOR_NAME, func);
            }
            else
            {
                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("writeBuffer", &NativeDataStream::WriteBuffer)
                    });
                JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_ENGINE_CONSTRUCTOR_NAME, func);
            }
        }

        NativeDataStream(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeDataStream>(info)
            , m_requestFlushCallback{Napi::Persistent(info[0].As<Napi::Function>())}
        {
        }

        void WriteBuffer(const Napi::CallbackInfo& info)
        {
            assert(!m_locked); // Cannot write bytes while the stream is locked for reading.

            const auto& buffer = info[0].As<Napi::ArrayBuffer>();
            const auto& length = info[1].ToNumber().Uint32Value();

            auto span = gsl::make_span(reinterpret_cast<uint32_t*>(buffer.Data()), static_cast<ptrdiff_t>(length));
            m_buffer.insert(m_buffer.end(), span.begin(), span.end());
        }

        Reader GetReader()
        {
            assert(!m_locked);
            m_requestFlushCallback.Call({});
            m_locked = true;
            return {m_buffer, [this]() {
                m_buffer.clear();
                m_locked = false; 
            }};
        }

    private:
        std::vector<uint32_t> m_buffer{};
        Napi::FunctionReference m_requestFlushCallback{};
        bool m_locked{false};
    };
}
