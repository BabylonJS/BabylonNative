#pragma once

#include <Babylon/JsRuntimeScheduler.h>
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
        Napi::Value GetBody(const Napi::CallbackInfo& info);
        Napi::Value GetOk(const Napi::CallbackInfo& info);
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
                InstanceAccessor("body", &Response::GetBody, nullptr),
                InstanceAccessor("ok", &Response::GetOk, nullptr),
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

    Napi::Value Response::GetBody(const Napi::CallbackInfo& info)
    {
        // Create a ReadableStream from the response data
        auto nativeObj = JsRuntime::NativeObject::GetFromJavaScript(info.Env());
        auto readableStreamCtor = nativeObj.Get(JS_READABLESTREAM_CONSTRUCTOR_NAME).As<Napi::Function>();

        // Create underlying source
        Napi::Object source = Napi::Object::New(info.Env());
        auto dataRef = std::make_shared<std::vector<std::byte>>(m_data);

        source.Set("start", Napi::Function::New(info.Env(), [dataRef](const Napi::CallbackInfo& cbInfo) {
            auto controller = cbInfo[0].As<Napi::Object>();
            auto enqueue = controller.Get("enqueue").As<Napi::Function>();

            auto arrayBuffer = Napi::ArrayBuffer::New(cbInfo.Env(), dataRef->size());
            std::memcpy(arrayBuffer.Data(), dataRef->data(), dataRef->size());
            auto uint8Array = Napi::Uint8Array::New(cbInfo.Env(), dataRef->size(), arrayBuffer, 0);

            enqueue.Call(controller, {uint8Array});

            auto close = controller.Get("close").As<Napi::Function>();
            close.Call(controller, {});
        }));

        return readableStreamCtor.New({source});
    }

    Napi::Value Response::GetOk(const Napi::CallbackInfo& info)
    {
        return Napi::Boolean::New(info.Env(), true);
    }
}
