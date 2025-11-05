#pragma once

#include <string>
#include <Babylon/JsRuntimeScheduler.h>

namespace Babylon::Plugins::Internal
{
    class TextDecoder final : public Napi::ObjectWrap<TextDecoder>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit TextDecoder(const Napi::CallbackInfo& info);
    private:
        Napi::Value Decode(const Napi::CallbackInfo& info);
    };

    static constexpr auto JS_TEXTDECODER_CONSTRUCTOR_NAME = "TextDecoder";

    void TextDecoder::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_TEXTDECODER_CONSTRUCTOR_NAME,
            {
                InstanceMethod("decode", &TextDecoder::Decode),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_TEXTDECODER_CONSTRUCTOR_NAME, func);
    }

    TextDecoder::TextDecoder(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<TextDecoder>{info}
    {
    }

    Napi::Value TextDecoder::Decode(const Napi::CallbackInfo& info)
    {
        const Napi::Env env{info.Env()};
        if (info.Length() < 1 || !info[0].IsTypedArray()) {
            throw Napi::Error::New(info.Env(), "Expected Uint8Array argument");
        }

        // content
        Napi::Uint8Array input = info[0].As<Napi::Uint8Array>();
        const uint8_t* inputData = input.Data();
        const size_t inputSize = input.ByteLength();

        return Napi::Value::From(env, std::string(reinterpret_cast<const char*>(inputData), inputSize));
    }
}
