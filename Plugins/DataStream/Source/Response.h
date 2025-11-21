#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include "DecompressionStream.h"

namespace Babylon::Plugins::Internal
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

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_RESPONSE_CONSTRUCTOR_NAME, func);
    }

    Response::Response(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Response>{info}
    {
        m_data = DecompressionStream::Unwrap(info[0].As<Napi::Object>())->GetDecompressedData();
    }

    Napi::Value Response::ArrayBuffer(const Napi::CallbackInfo& )
    {
        const auto arrayBuffer = Napi::ArrayBuffer::New(Env(), m_data.size());
        std::memcpy(arrayBuffer.Data(), m_data.data(), m_data.size());

        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(arrayBuffer);
        return deferred.Promise();
    }
}
