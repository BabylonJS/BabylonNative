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
            Uint8,
            Uint32,
            Int32,
            Float32,
            Uint32Array,
            Int32Array,
            Float32Array,
            NativeHandle,
            Boolean,
        };

        template<ValidationType T, typename ReaderT>
        static void Validate(ReaderT& reader)
        {
            if constexpr (VALIDATION_ENABLED)
            {
                uint32_t value{reader.Read<uint32_t>()};
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
                assert(m_position <= static_cast<size_t>(m_bytes.size()));
                return m_position < static_cast<size_t>(m_bytes.size());
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

            template<typename PointerT>
            PointerT ReadPointer()
            {
                Validate<ValidationType::NativeHandle>(*this);
                const auto data = ReadUint32Array();
                return *reinterpret_cast<PointerT*>(data.data());
            }

        private:
            gsl::span<uint8_t> m_bytes{};
            size_t m_position{0};
            const gsl::final_action<std::function<void()>> m_scopeGuard;

            friend class NativeDataStream;

            template<typename CallableT>
            Reader(gsl::span<uint8_t> bytes, CallableT&& callable)
                : m_bytes{bytes}
                , m_scopeGuard{std::forward<CallableT>(callable)}
            {
            }

            template<typename T>
            T Read()
            {
                T t{*reinterpret_cast<T*>(m_bytes.data() + m_position)};
                m_position += sizeof(T);
                return t;
            }

            template<typename T>
            gsl::span<T> ReadArray()
            {
                // Arrays are byte aligned
                ByteAlign();
                // The first 32 bit number is a size
                uint32_t size = Read<uint32_t>();

                auto span = gsl::make_span<T>(reinterpret_cast<T*>(m_bytes.data() + m_position), size);
                m_position += size * sizeof(T);
                return span;
            }

            void ByteAlign()
            {
                // TODO: Put back byte alignment when we're doing things more efficiently in TS.
                size_t remainder = m_position % 4 > 0;
                if (remainder > 0)
                {
                    m_position += (4 - remainder);
                }
            }
        };

        static std::initializer_list<Napi::ClassPropertyDescriptor<NativeDataStream>> GetProperties(Napi::Env env)
        {
            if constexpr (VALIDATION_ENABLED)
            {
                return
                {
                    InstanceMethod("writeBytes", &NativeDataStream::WriteBytes),

                    StaticValue("VALIDATION_UINT_8", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Uint8))),
                    StaticValue("VALIDATION_UINT_32", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Uint32))),
                    StaticValue("VALIDATION_INT_32", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Int32))),
                    StaticValue("VALIDATION_FLOAT_32", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Float32))),
                    StaticValue("VALIDATION_UINT_32_ARRAY", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Uint32Array))),
                    StaticValue("VALIDATION_INT_32_ARRAY", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Int32Array))),
                    StaticValue("VALIDATION_FLOAT_32_ARRAY", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Float32Array))),
                    StaticValue("VALIDATION_NATIVE_HANDLE", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::NativeHandle))),
                    StaticValue("VALIDATION_BOOLEAN", Napi::Number::From(env, static_cast<uint32_t>(ValidationType::Boolean))),
                };
            }
            else
            {
                return
                {
                    InstanceMethod("writeBytes", &NativeDataStream::WriteBytes)
                };
            }
        }

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            if constexpr (VALIDATION_ENABLED)
            {
                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("writeBytes", &NativeDataStream::WriteBytes),

                        StaticValue("VALIDATION_UINT_8", Napi::Number::From(env, static_cast<int>(ValidationType::Uint8))),
                        StaticValue("VALIDATION_UINT_32", Napi::Number::From(env, static_cast<int>(ValidationType::Uint32))),
                        StaticValue("VALIDATION_INT_32", Napi::Number::From(env, static_cast<int>(ValidationType::Int32))),
                        StaticValue("VALIDATION_FLOAT_32", Napi::Number::From(env, static_cast<int>(ValidationType::Float32))),
                        StaticValue("VALIDATION_UINT_32_ARRAY", Napi::Number::From(env, static_cast<int>(ValidationType::Uint32Array))),
                        StaticValue("VALIDATION_INT_32_ARRAY", Napi::Number::From(env, static_cast<int>(ValidationType::Int32Array))),
                        StaticValue("VALIDATION_FLOAT_32_ARRAY", Napi::Number::From(env, static_cast<int>(ValidationType::Float32Array))),
                        StaticValue("VALIDATION_NATIVE_HANDLE", Napi::Number::From(env, static_cast<int>(ValidationType::NativeHandle))),
                        StaticValue("VALIDATION_BOOLEAN", Napi::Number::From(env, static_cast<int>(ValidationType::Boolean))),
                    });
                JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_ENGINE_CONSTRUCTOR_NAME, func);
            }
            else
            {
                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("writeBytes", &NativeDataStream::WriteBytes)
                    });
                JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_ENGINE_CONSTRUCTOR_NAME, func);
            }
        }

        NativeDataStream::NativeDataStream(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeDataStream>(info)
            , m_requestFlushCallback{Napi::Persistent(info[0].As<Napi::Function>())}
        {
        }

        void WriteBytes(const Napi::CallbackInfo& info)
        {
            assert(!m_locked); // Cannot write bytes while the stream is locked for reading.

            const auto& buffer = info[0].As<Napi::ArrayBuffer>();
            const auto& byteLength = info[1].ToNumber().Uint32Value();

            auto newBytes = gsl::make_span(reinterpret_cast<uint8_t*>(buffer.Data()), static_cast<ptrdiff_t>(byteLength));
            m_bytes.insert(m_bytes.end(), newBytes.begin(), newBytes.end());
        }

        Reader GetReader()
        {
            assert(!m_locked);
            m_requestFlushCallback.Call({});
            m_locked = true;
            return {m_bytes, [this]() {
                m_bytes.clear();
                m_locked = false; 
            }};
        }

    private:
        std::vector<uint8_t> m_bytes{};
        Napi::FunctionReference m_requestFlushCallback{};
        bool m_locked{false};
    };
}
