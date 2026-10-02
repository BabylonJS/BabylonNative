#include <Babylon/Polyfills/TextEncoder.h>

#include <napi/napi.h>

#include <cstring>
#include <string>

namespace
{
    class TextEncoder final : public Napi::ObjectWrap<TextEncoder>
    {
    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            static constexpr auto JS_TEXTENCODER_CONSTRUCTOR_NAME = "TextEncoder";
            if (env.Global().Get(JS_TEXTENCODER_CONSTRUCTOR_NAME).IsUndefined())
            {
                Napi::Function func = DefineClass(
                    env,
                    JS_TEXTENCODER_CONSTRUCTOR_NAME,
                    {
                        InstanceAccessor("encoding", &TextEncoder::Encoding, nullptr),
                        InstanceMethod("encode", &TextEncoder::Encode),
                    });

                env.Global().Set(JS_TEXTENCODER_CONSTRUCTOR_NAME, func);
            }
        }

        explicit TextEncoder(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<TextEncoder>{info}
        {
            // The TextEncoder constructor takes no arguments. The encoding is
            // always UTF-8 per the WHATWG Encoding Standard.
        }

    private:
        Napi::Value Encoding(const Napi::CallbackInfo& info)
        {
            return Napi::String::New(info.Env(), "utf-8");
        }

        // encode(input = "") - returns a Uint8Array containing the UTF-8 bytes.
        // Per spec, undefined input defaults to the empty string (not "undefined").
        Napi::Value Encode(const Napi::CallbackInfo& info)
        {
            auto env = info.Env();
            std::string utf8;
            if (info.Length() > 0 && !info[0].IsUndefined())
            {
                utf8 = info[0].ToString().Utf8Value();
            }

            auto bytes = Napi::Uint8Array::New(env, utf8.size());
            if (!utf8.empty())
            {
                std::memcpy(bytes.Data(), utf8.data(), utf8.size());
            }
            return bytes;
        }
    };
}

namespace Babylon::Polyfills::TextEncoder
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        ::TextEncoder::Initialize(env);
    }
}
