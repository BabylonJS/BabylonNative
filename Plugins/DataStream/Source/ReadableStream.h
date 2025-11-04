#include <Babylon/JsRuntimeScheduler.h>

namespace Babylon::Plugins::Internal
{
    class ReadableStream final : public Napi::ObjectWrap<ReadableStream>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit ReadableStream(const Napi::CallbackInfo& info);
    private:
        Napi::Value PipeThrough(const Napi::CallbackInfo& info);

        Napi::FunctionReference m_startHandlerRef;
    };

    static constexpr auto JS_READABLESTREAM_CONSTRUCTOR_NAME = "ReadableStream";

    void ReadableStream::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{ env };

        Napi::Function func = DefineClass(
            env,
            JS_READABLESTREAM_CONSTRUCTOR_NAME,
            {
                InstanceMethod("pipeThrough", &ReadableStream::PipeThrough),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_READABLESTREAM_CONSTRUCTOR_NAME, func);
    }

    ReadableStream::ReadableStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ReadableStream>{ info }
    {
        if (!info[0].IsObject())
        {
            return;
        }

        auto object = info[0].ToObject();
        if (object.Has("start")) {
            Napi::Value startFunction = object.Get("start");
            if (startFunction.IsFunction())
            {
                Napi::Function handler{ startFunction.As<Napi::Function>() };
                m_startHandlerRef = Napi::Persistent(handler);
            }
        }
    }

    Napi::Value ReadableStream::PipeThrough(const Napi::CallbackInfo& info)
    {
        return m_startHandlerRef.Call({info[0]});
    }
}
