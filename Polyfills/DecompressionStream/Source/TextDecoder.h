#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <string>

namespace Babylon::Polyfills::Internal
{
    class TextDecoder final : public Napi::ObjectWrap<TextDecoder>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit TextDecoder(const Napi::CallbackInfo& info);

    private:
        Napi::Value Decode(const Napi::CallbackInfo& info);
        std::string m_encoding;
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

        env.Global().Set(JS_TEXTDECODER_CONSTRUCTOR_NAME, func);
    }

    TextDecoder::TextDecoder(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<TextDecoder>{info}
        , m_encoding{"utf-8"}
    {
        if (info.Length() > 0 && info[0].IsString())
        {
            m_encoding = info[0].As<Napi::String>().Utf8Value();
        }
    }

    Napi::Value TextDecoder::Decode(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 1)
        {
            return Napi::String::New(info.Env(), "");
        }

        std::vector<uint8_t> data;

        if (info[0].IsTypedArray())
        {
            auto typedArray = info[0].As<Napi::TypedArray>();
            auto arrayBuffer = typedArray.ArrayBuffer();
            auto byteOffset = typedArray.ByteOffset();
            auto byteLength = typedArray.ByteLength();
            data.resize(byteLength);
            std::memcpy(data.data(), static_cast<uint8_t*>(arrayBuffer.Data()) + byteOffset, byteLength);
        }
        else if (info[0].IsArrayBuffer())
        {
            auto arrayBuffer = info[0].As<Napi::ArrayBuffer>();
            data.resize(arrayBuffer.ByteLength());
            std::memcpy(data.data(), arrayBuffer.Data(), arrayBuffer.ByteLength());
        }

        // Convert bytes to string (UTF-8)
        std::string result(data.begin(), data.end());
        return Napi::String::New(info.Env(), result);
    }
}
