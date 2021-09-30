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

    public:
        class Reader final
        {
        public:
            Reader(const Reader&) = delete;
            Reader(Reader&&) = delete;
            Reader operator=(const Reader&) = delete;
            Reader operator=(const Reader&&) = delete;

            ~Reader()
            {
                m_lock = false;
            }

            bool CanRead() const
            {
                assert(m_position <= static_cast<size_t>(m_bytes.size()));
                return m_position < static_cast<size_t>(m_bytes.size());
            }

            template<typename PointerT>
            PointerT ReadPointer()
            {
                const auto data = ReadUint32Array();
                return *reinterpret_cast<PointerT*>(data.data());
            }

            uint32_t ReadUint32()
            {
                return Read<uint32_t>();
            }

            int32_t ReadInt32()
            {
                return Read<int32_t>();
            }

            float ReadFloat32()
            {
                return Read<float>();
            }

            gsl::span<uint32_t> ReadUint32Array()
            {
                return ReadArray<uint32_t>();
            }

            gsl::span<int32_t> ReadInt32Array()
            {
                return ReadArray<int32_t>();
            }

            gsl::span<float> ReadFloat32Array()
            {
                return ReadArray<float>();
            }

        private:
            gsl::span<uint8_t> m_bytes{};
            size_t m_position{0};
            bool& m_lock;

            friend class NativeDataStream;
            Reader(gsl::span<uint8_t> bytes, bool& lock)
                : m_bytes{bytes}
                , m_lock{lock}
            {
                lock = true;
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

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            // clang-format off
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("writeBytes", &NativeDataStream::WriteBytes)
                });
            // clang-format on

            JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_ENGINE_CONSTRUCTOR_NAME, func);
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
            return {m_bytes, m_locked};
        }

    private:
        std::vector<uint8_t> m_bytes{};
        Napi::FunctionReference m_requestFlushCallback{};
        bool m_locked{false};
    };
}
