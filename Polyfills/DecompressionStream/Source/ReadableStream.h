#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <queue>

namespace Babylon::Polyfills::Internal
{
    class ReadableStream final : public Napi::ObjectWrap<ReadableStream>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit ReadableStream(const Napi::CallbackInfo& info);
        virtual ~ReadableStream();

    private:
        Napi::Value PipeThrough(const Napi::CallbackInfo& info);
        Napi::Value GetReader(const Napi::CallbackInfo& info);
        Napi::Value Cancel(const Napi::CallbackInfo& info);
        Napi::Value GetLocked(const Napi::CallbackInfo& info);

        // Creates a plain JS controller object (not a wrapped class)
        Napi::Object CreateController(Napi::Env env);

        Napi::FunctionReference m_pullHandlerRef;
        Napi::FunctionReference m_cancelHandlerRef;
        std::queue<Napi::ObjectReference> m_queue;
        bool m_locked{false};
        bool m_closed{false};
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
                InstanceMethod("getReader", &ReadableStream::GetReader),
                InstanceMethod("cancel", &ReadableStream::Cancel),
                InstanceAccessor("locked", &ReadableStream::GetLocked, nullptr),
            });

        env.Global().Set(JS_READABLESTREAM_CONSTRUCTOR_NAME, func);
    }

    Napi::Object ReadableStream::CreateController(Napi::Env env)
    {
        // Create a plain JS object acting as the controller
        Napi::Object controller = Napi::Object::New(env);

        // enqueue(chunk) - adds chunk to the queue
        controller.Set("enqueue", Napi::Function::New(env,
                                      [this](const Napi::CallbackInfo& info) {
                                          if (!m_closed && info.Length() > 0)
                                          {
                                              m_queue.push(Napi::Persistent(info[0].As<Napi::Object>()));
                                          }
                                      }));

        // close() - signals the stream is done
        controller.Set("close", Napi::Function::New(env,
                                    [this](const Napi::CallbackInfo&) {
                                        m_closed = true;
                                    }));

        // error(e) - signals an error
        controller.Set("error", Napi::Function::New(env,
                                    [this](const Napi::CallbackInfo&) {
                                        m_closed = true;
                                        while (!m_queue.empty())
                                        {
                                            m_queue.pop();
                                        }
                                    }));

        // desiredSize property (read-only)
        controller.Set("desiredSize", Napi::Number::New(env, 1.0));

        return controller;
    }

    ReadableStream::ReadableStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ReadableStream>{info}
    {
        if (info.Length() > 0 && info[0].IsObject())
        {
            auto underlyingSource = info[0].ToObject();
            Napi::Object controller = CreateController(info.Env());

            // Per spec: invoke start(controller) immediately
            if (underlyingSource.Has("start"))
            {
                Napi::Value startFn = underlyingSource.Get("start");
                if (startFn.IsFunction())
                {
                    startFn.As<Napi::Function>().Call({controller});
                }
            }

            if (underlyingSource.Has("pull"))
            {
                m_pullHandlerRef = Napi::Persistent(underlyingSource.Get("pull").As<Napi::Function>());
            }

            if (underlyingSource.Has("cancel"))
            {
                m_cancelHandlerRef = Napi::Persistent(underlyingSource.Get("cancel").As<Napi::Function>());
            }
        }
    }

    ReadableStream::~ReadableStream()
    {
        m_pullHandlerRef.Reset();
        m_cancelHandlerRef.Reset();
        while (!m_queue.empty())
        {
            m_queue.pop();
        }
    }

    Napi::Value ReadableStream::GetLocked(const Napi::CallbackInfo& info)
    {
        return Napi::Boolean::New(info.Env(), m_locked);
    }

    Napi::Value ReadableStream::PipeThrough(const Napi::CallbackInfo& info)
    {
        Napi::Object transformStream = info[0].As<Napi::Object>();
        if (transformStream.Has("readable"))
        {
            return transformStream.Get("readable");
        }
        return info.Env().Undefined();
    }

    Napi::Value ReadableStream::GetReader(const Napi::CallbackInfo& info)
    {
        if (m_locked)
        {
            throw Napi::Error::New(info.Env(), "ReadableStream is locked");
        }
        m_locked = true;

        Napi::Object reader = Napi::Object::New(info.Env());

        reader.Set("read", Napi::Function::New(info.Env(),
                               [this](const Napi::CallbackInfo& cbInfo) -> Napi::Value {
                                   auto deferred = Napi::Promise::Deferred::New(cbInfo.Env());
                                   Napi::Object result = Napi::Object::New(cbInfo.Env());

                                   if (!m_queue.empty())
                                   {
                                       result.Set("value", m_queue.front().Value());
                                       result.Set("done", false);
                                       m_queue.pop();
                                   }
                                   else
                                   {
                                       result.Set("value", cbInfo.Env().Undefined());
                                       result.Set("done", true);
                                   }
                                   deferred.Resolve(result);
                                   return deferred.Promise();
                               }));

        reader.Set("releaseLock", Napi::Function::New(info.Env(),
                                      [this](const Napi::CallbackInfo&) { m_locked = false; }));

        return reader;
    }

    Napi::Value ReadableStream::Cancel(const Napi::CallbackInfo& info)
    {
        auto deferred = Napi::Promise::Deferred::New(info.Env());
        m_closed = true;
        deferred.Resolve(info.Env().Undefined());
        return deferred.Promise();
    }
}
