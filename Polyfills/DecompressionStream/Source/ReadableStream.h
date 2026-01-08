#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include "ReadableStreamDefaultController.h"

namespace Babylon::Polyfills::Internal
{
    class ReadableStream final : public Napi::ObjectWrap<ReadableStream>
    {
    public:
        virtual ~ReadableStream();
        static void Initialize(Napi::Env env);

        explicit ReadableStream(const Napi::CallbackInfo& info);
    private:
        Napi::Value PipeThrough(const Napi::CallbackInfo& info);

        Napi::FunctionReference m_startHandlerRef;
        Napi::ObjectReference m_pipeThroughRef;
    };

    static constexpr auto JS_READABLESTREAM_CONSTRUCTOR_NAME = "ReadableStream";

    void ReadableStream::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_READABLESTREAM_CONSTRUCTOR_NAME,
            {
                InstanceMethod("pipeThrough", &ReadableStream::PipeThrough),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_READABLESTREAM_CONSTRUCTOR_NAME, func);
    }

    ReadableStream::ReadableStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ReadableStream>{info}
    {
        //mReadableStreamDefaultController = ReadableStreamDefaultController::CreateInstance(info.Env(), info.This());

        if (!info[0].IsObject())
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
        }
    }

    ReadableStream::~ReadableStream()
    {
        m_pipeThroughRef.Reset();
        m_startHandlerRef.Reset();
    }

    Napi::Value ReadableStream::PipeThrough(const Napi::CallbackInfo& info)
    {
        Napi::Object stream = info[0].As<Napi::Object>();
        m_pipeThroughRef = Napi::Persistent(stream);
        m_startHandlerRef.Call({stream});
        return stream;
    }
}
