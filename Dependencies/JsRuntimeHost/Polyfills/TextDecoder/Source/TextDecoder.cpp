#include <Babylon/Polyfills/TextDecoder.h>

#include <napi/napi.h>
#include <cstring>
#include <string>

namespace
{
    class TextDecoder final : public Napi::ObjectWrap<TextDecoder>
    {
    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            static constexpr auto JS_TEXTDECODER_CONSTRUCTOR_NAME = "TextDecoder";
            if (env.Global().Get(JS_TEXTDECODER_CONSTRUCTOR_NAME).IsUndefined())
            {
                Napi::Function func = DefineClass(
                    env,
                    JS_TEXTDECODER_CONSTRUCTOR_NAME,
                    {
                        InstanceMethod("decode", &TextDecoder::Decode),
                    });

                env.Global().Set(JS_TEXTDECODER_CONSTRUCTOR_NAME, func);
            }
        }

        explicit TextDecoder(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<TextDecoder>{info}
        {
            if (info.Length() > 0 && info[0].IsString())
            {
                auto encoding = info[0].As<Napi::String>().Utf8Value();
                if (encoding != "utf-8" && encoding != "UTF-8")
                {
                    Napi::Error::New(info.Env(), "TextDecoder: unsupported encoding '" + encoding + "', only 'utf-8' is supported")
                        .ThrowAsJavaScriptException();
                }
            }
        }

    private:
        Napi::Value Decode(const Napi::CallbackInfo& info)
        {
            if (info.Length() < 1 || info[0].IsUndefined())
            {
                return Napi::String::New(info.Env(), "");
            }

            std::string data;

            if (info[0].IsTypedArray())
            {
                auto typedArray = info[0].As<Napi::TypedArray>();
                auto arrayBuffer = typedArray.ArrayBuffer();
                auto byteOffset = typedArray.ByteOffset();
                auto byteLength = typedArray.ByteLength();
                data.resize(byteLength);
                if (byteLength > 0)
                {
                    std::memcpy(data.data(), static_cast<uint8_t*>(arrayBuffer.Data()) + byteOffset, byteLength);
                }
            }
            else if (info[0].IsArrayBuffer())
            {
                auto arrayBuffer = info[0].As<Napi::ArrayBuffer>();
                auto byteLength = arrayBuffer.ByteLength();
                data.resize(byteLength);
                if (byteLength > 0)
                {
                    std::memcpy(data.data(), arrayBuffer.Data(), byteLength);
                }
            }
            else
            {
                Napi::TypeError::New(info.Env(), "TextDecoder.decode: input must be a BufferSource (ArrayBuffer or TypedArray)")
                    .ThrowAsJavaScriptException();
                return info.Env().Undefined();
            }

            return Napi::String::New(info.Env(), data);
        }
    };
}

namespace Babylon::Polyfills::TextDecoder
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        ::TextDecoder::Initialize(env);
    }
}
