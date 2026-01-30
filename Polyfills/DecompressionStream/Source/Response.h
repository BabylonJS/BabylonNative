#pragma once
#include "DecompressionStream.h"

namespace Babylon::Polyfills::Internal
{
    class Response final : public Napi::ObjectWrap<Response>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit Response(const Napi::CallbackInfo& info);

    private:
        Napi::Value ArrayBuffer(const Napi::CallbackInfo& info);
        std::vector<std::byte> m_data;
    };

    static constexpr auto JS_RESPONSE_CONSTRUCTOR_NAME = "Response";

    void Response::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_RESPONSE_CONSTRUCTOR_NAME,
            {
                InstanceMethod("arrayBuffer", &Response::ArrayBuffer),
            });

        env.Global().Set(JS_RESPONSE_CONSTRUCTOR_NAME, func);
    }

    Response::Response(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Response>{info}
    {
        if (info[0].IsObject())
        {
            // Check if it's a DecompressionStream
            auto obj = info[0].As<Napi::Object>();
            DecompressionStream* decomp = DecompressionStream::Unwrap(obj);
            if (decomp)
            {
                m_data = decomp->GetDecompressedData();
            }
        }
        else if (info[0].IsArrayBuffer())
        {
            auto arrayBuffer = info[0].As<Napi::ArrayBuffer>();
            m_data.resize(arrayBuffer.ByteLength());
            std::memcpy(m_data.data(), arrayBuffer.Data(), arrayBuffer.ByteLength());
        }
        else if (info[0].IsTypedArray())
        {
            auto typedArray = info[0].As<Napi::TypedArray>();
            auto arrayBuffer = typedArray.ArrayBuffer();
            auto byteOffset = typedArray.ByteOffset();
            auto byteLength = typedArray.ByteLength();
            m_data.resize(byteLength);
            std::memcpy(m_data.data(), static_cast<uint8_t*>(arrayBuffer.Data()) + byteOffset, byteLength);
        }
    }

    Napi::Value Response::ArrayBuffer(const Napi::CallbackInfo& /*info*/)
    {
        const auto arrayBuffer = Napi::ArrayBuffer::New(Env(), m_data.size());
        std::memcpy(arrayBuffer.Data(), m_data.data(), m_data.size());

        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(arrayBuffer);
        return deferred.Promise();
    }
}
