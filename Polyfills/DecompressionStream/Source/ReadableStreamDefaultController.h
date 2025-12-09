#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <gsl/span>

namespace Babylon::Polyfills::Internal
{
    class ReadableStreamDefaultController final : public Napi::ObjectWrap<ReadableStreamDefaultController>
    {
    public:
        virtual ~ReadableStreamDefaultController();
        static void Initialize(Napi::Env env);

        explicit ReadableStreamDefaultController(const Napi::CallbackInfo& info);

        Napi::Value CreateInstance(Napi::Env env, Napi::Value readableStream);
    private:
        void Enqueue(const Napi::CallbackInfo& info);
        void Close(const Napi::CallbackInfo& info);
    };

    static constexpr auto JS_READABLESTREAMDFAULTCONTROLLER_CONSTRUCTOR_NAME = "ReadableStream";

    void ReadableStreamDefaultController::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_READABLESTREAMDFAULTCONTROLLER_CONSTRUCTOR_NAME,
            {
                InstanceMethod("enqueue", &ReadableStreamDefaultController::Enqueue),
                InstanceMethod("close", &ReadableStreamDefaultController::Close),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_READABLESTREAMDFAULTCONTROLLER_CONSTRUCTOR_NAME, func);
    }

    ReadableStreamDefaultController::ReadableStreamDefaultController(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ReadableStreamDefaultController>{info}
    {
        /* if (!info[0].IsObject())
        {
            return;
        }

        auto object = info[0].ToObject();
        if (object.Has("start")) {
            Napi::Value startFunction = object.Get("start");
            if (startFunction.IsFunction())
            {
                Napi::Function handler{startFunction.As<Napi::Function>()};
                m_startHandlerRef = Napi::Persistent(handler);
            }
        }*/
    }

    Napi::Value ReadableStreamDefaultController::CreateInstance(Napi::Env env, Napi::Value readableStream)
    {
                auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_READABLESTREAMDFAULTCONTROLLER_CONSTRUCTOR_NAME).As<Napi::Function>();
        return func.New({readableStream});
    }

    ReadableStreamDefaultController::~ReadableStreamDefaultController()
    {
    }

    void ReadableStreamDefaultController::Enqueue(const Napi::CallbackInfo& info)
    {
        const Napi::Env env{info.Env()};
        auto value = info[0];
        Napi::TypedArray typed = value.As<Napi::TypedArray>();

        if (typed.TypedArrayType() == napi_uint8_array)
        {
            Napi::Uint8Array array = typed.As<Napi::Uint8Array>();

            gsl::span<uint8_t> buffer = {array.Data(), array.ByteLength()};
            if (buffer.empty())
            {
                throw Napi::Error::New(env, "GZip data buffer is empty.");
            }

            //m_data = DecompressGzip(env, buffer);
        }
    }

        void ReadableStreamDefaultController::Close(const Napi::CallbackInfo& /*info*/)
    {
    }

}
