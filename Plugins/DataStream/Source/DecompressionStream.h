#include <Babylon/JsRuntimeScheduler.h>

namespace Babylon::Plugins::Internal
{
    class DecompressionStream final : public Napi::ObjectWrap<DecompressionStream>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit DecompressionStream(const Napi::CallbackInfo& info);
    private:
        Napi::Value Decode(const Napi::CallbackInfo& /*info*/);
    };

    static constexpr auto JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME = "DecompressionStream";

    void DecompressionStream::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{ env };

        Napi::Function func = DefineClass(
            env,
            JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME,
            {
                InstanceMethod("decode", &DecompressionStream::Decode),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME, func);
    }

    DecompressionStream::DecompressionStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<DecompressionStream>{ info }
    {
        const Napi::Env env{ info.Env() };
        if (info.Length() < 1 || !info[0].IsString()) {
            throw Napi::Error::New(info.Env(), "Expected String argument for DecompressionStream constructor.");
        }
        std::string compressionType = info[0].As<Napi::String>().Utf8Value();
        if (compressionType != "gzip") {
            throw Napi::Error::New(info.Env(), "Unexpected compression type.");
        }
    }

    Napi::Value DecompressionStream::Decode(const Napi::CallbackInfo& /*info*/)
    {

        return {};
    }
}
