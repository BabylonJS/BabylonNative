#include "AudioBufferSourceNode.h"
#include "AudioBuffer.h"
#include "AudioContext.h"
#include "AudioEngine.h"
#include "AudioParam.h"

#include <Babylon/JsRuntime.h>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        void LockAnd(const std::shared_ptr<AudioContextState>& context, auto&& fn)
        {
            if (context && context->engine)
            {
                std::lock_guard<std::mutex> lock{context->engine->GraphMutex()};
                fn();
            }
            else
            {
                fn();
            }
        }
    }

    void NativeAudioBufferSourceNode::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceMethod("connect", &NativeAudioBufferSourceNode::Connect),
                InstanceMethod("disconnect", &NativeAudioBufferSourceNode::Disconnect),
                InstanceMethod("start", &NativeAudioBufferSourceNode::Start),
                InstanceMethod("stop", &NativeAudioBufferSourceNode::Stop),
                InstanceMethod("addEventListener", &NativeAudioBufferSourceNode::AddEventListener),
                InstanceMethod("removeEventListener", &NativeAudioBufferSourceNode::RemoveEventListener),
                InstanceAccessor("buffer", &NativeAudioBufferSourceNode::GetBuffer, &NativeAudioBufferSourceNode::SetBuffer),
                InstanceAccessor("loop", &NativeAudioBufferSourceNode::GetLoop, &NativeAudioBufferSourceNode::SetLoop),
                InstanceAccessor("loopStart", &NativeAudioBufferSourceNode::GetLoopStart, &NativeAudioBufferSourceNode::SetLoopStart),
                InstanceAccessor("loopEnd", &NativeAudioBufferSourceNode::GetLoopEnd, &NativeAudioBufferSourceNode::SetLoopEnd),
                InstanceAccessor("playbackRate", &NativeAudioBufferSourceNode::GetPlaybackRate, nullptr),
                InstanceAccessor("detune", &NativeAudioBufferSourceNode::GetDetune, nullptr),
                InstanceAccessor("onended", &NativeAudioBufferSourceNode::GetOnEnded, &NativeAudioBufferSourceNode::SetOnEnded),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Napi::Object NativeAudioBufferSourceNode::CreateInstance(Napi::Env env, const std::shared_ptr<AudioContextState>& context)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto object = func.New({});
        NativeAudioBufferSourceNode::Unwrap(object)->InitializeState(context, env.Undefined());
        return object;
    }

    struct NativeAudioBufferSourceNode::EndedDispatcher : std::enable_shared_from_this<EndedDispatcher>
    {
        explicit EndedDispatcher(Napi::Object jsThis)
            : jsThis{Napi::Persistent(jsThis)}
        {
        }

        void NotifyEnded(const std::shared_ptr<AudioEngine>& engine)
        {
            if (!engine)
            {
                return;
            }

            engine->ScheduleOnJsThread([self = shared_from_this()]() {
                if (self->jsThis.IsEmpty())
                {
                    return;
                }

                auto* node = NativeAudioBufferSourceNode::Unwrap(self->jsThis.Value());
                if (node)
                {
                    node->FireEnded();
                }
            });
        }

        Napi::ObjectReference jsThis{};
    };

    NativeAudioBufferSourceNode::NativeAudioBufferSourceNode(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeAudioBufferSourceNode>{info}
    {
        m_endedDispatcher = std::make_shared<EndedDispatcher>(info.This().As<Napi::Object>());

        if (info.Length() > 0 && info[0].IsObject())
        {
            auto* context = NativeAudioContext::Unwrap(info[0].As<Napi::Object>());
            if (context)
            {
                info.This().As<Napi::Object>().Set("context", info[0]);
                InitializeState(context->State(), info.Length() > 1 ? info[1] : info.Env().Undefined());
            }
        }
    }

    NativeAudioBufferSourceNode::~NativeAudioBufferSourceNode()
    {
        if (m_state)
        {
            auto context = m_state->context.lock();
            if (context && context->engine)
            {
                context->engine->UnregisterSource(m_state.get());
            }
            m_state->onEnded = nullptr;
        }

        if (m_endedDispatcher)
        {
            m_endedDispatcher->jsThis.Reset();
        }
    }

    void NativeAudioBufferSourceNode::InitializeState(const std::shared_ptr<AudioContextState>& context, const Napi::Value& options)
    {
        m_state = std::make_shared<BufferSourceState>();
        m_state->context = context;
        BindNodeState(Value(), m_state);
        SetAudioNodeProperties(Value(), 0, 1);

        auto getCurrentTime = MakeCurrentTimeGetter(context);
        m_playbackRate = Napi::Persistent(NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->playbackRate), 1.f, getCurrentTime));
        m_detune = Napi::Persistent(NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->detune), 0.f, getCurrentTime));

        if (options.IsObject())
        {
            auto optionsObject = options.As<Napi::Object>();
            if (optionsObject.Has("buffer") && optionsObject.Get("buffer").IsObject())
            {
                auto bufferValue = optionsObject.Get("buffer");
                auto* buffer = NativeAudioBuffer::Unwrap(bufferValue.As<Napi::Object>());
                m_buffer = Napi::Persistent(bufferValue.As<Napi::Object>());
                m_state->buffer = buffer ? buffer->Data() : nullptr;
            }
            if (optionsObject.Has("loop"))
            {
                m_state->loop = optionsObject.Get("loop").ToBoolean();
            }
            if (optionsObject.Has("loopStart"))
            {
                m_state->loopStart = optionsObject.Get("loopStart").ToNumber().DoubleValue();
            }
            if (optionsObject.Has("loopEnd"))
            {
                m_state->loopEnd = optionsObject.Get("loopEnd").ToNumber().DoubleValue();
            }
        }

        if (context && context->engine)
        {
            context->engine->RegisterSource(m_state);
        }

        if (!m_endedDispatcher)
        {
            m_endedDispatcher = std::make_shared<EndedDispatcher>(Value());
        }
    }

    Napi::Value NativeAudioBufferSourceNode::Connect(const Napi::CallbackInfo& info)
    {
        auto destination = UnwrapNodeState(info[0]);
        if (m_state && destination)
        {
            LockAnd(m_state->context.lock(), [&]() { m_state->Connect(destination); });
        }
        return info[0];
    }

    Napi::Value NativeAudioBufferSourceNode::Disconnect(const Napi::CallbackInfo& info)
    {
        if (!m_state)
        {
            return info.Env().Undefined();
        }

        if (info.Length() > 0)
        {
            auto destination = UnwrapNodeState(info[0]);
            LockAnd(m_state->context.lock(), [&]() { m_state->Disconnect(destination); });
        }
        else
        {
            LockAnd(m_state->context.lock(), [&]() { m_state->Disconnect(); });
        }
        return info.Env().Undefined();
    }

    Napi::Value NativeAudioBufferSourceNode::GetBuffer(const Napi::CallbackInfo& info)
    {
        return m_buffer.IsEmpty() ? info.Env().Null() : m_buffer.Value();
    }

    void NativeAudioBufferSourceNode::SetBuffer(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (!m_state)
        {
            return;
        }

        if (value.IsNull() || value.IsUndefined())
        {
            m_buffer.Reset();
            m_state->buffer.reset();
            return;
        }

        auto* buffer = NativeAudioBuffer::Unwrap(value.As<Napi::Object>());
        m_buffer = Napi::Persistent(value.As<Napi::Object>());
        m_state->buffer = buffer ? buffer->Data() : nullptr;
    }

    Napi::Value NativeAudioBufferSourceNode::GetLoop(const Napi::CallbackInfo& info)
    {
        return Napi::Boolean::New(info.Env(), m_state && m_state->loop);
    }

    void NativeAudioBufferSourceNode::SetLoop(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->loop = value.ToBoolean();
        }
    }

    Napi::Value NativeAudioBufferSourceNode::GetLoopStart(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->loopStart : 0.0);
    }

    void NativeAudioBufferSourceNode::SetLoopStart(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->loopStart = value.ToNumber().DoubleValue();
        }
    }

    Napi::Value NativeAudioBufferSourceNode::GetLoopEnd(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->loopEnd : 0.0);
    }

    void NativeAudioBufferSourceNode::SetLoopEnd(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->loopEnd = value.ToNumber().DoubleValue();
        }
    }

    Napi::Value NativeAudioBufferSourceNode::GetPlaybackRate(const Napi::CallbackInfo&)
    {
        return m_playbackRate.Value();
    }

    Napi::Value NativeAudioBufferSourceNode::GetDetune(const Napi::CallbackInfo&)
    {
        return m_detune.Value();
    }

    Napi::Value NativeAudioBufferSourceNode::GetOnEnded(const Napi::CallbackInfo& info)
    {
        return m_onended.IsEmpty() ? info.Env().Null() : m_onended.Value();
    }

    void NativeAudioBufferSourceNode::SetOnEnded(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (value.IsFunction())
        {
            m_onended = Napi::Persistent(value.As<Napi::Function>());
        }
        else
        {
            m_onended.Reset();
        }
    }

    void NativeAudioBufferSourceNode::FireEnded()
    {
        m_eventTarget.DispatchEvent(Env(), "ended");
        if (!m_onended.IsEmpty())
        {
            m_onended.Call({});
        }
    }

    void NativeAudioBufferSourceNode::Start(const Napi::CallbackInfo& info)
    {
        if (!m_state)
        {
            return;
        }

        const double when = info.Length() > 0 ? info[0].ToNumber().DoubleValue() : 0.0;
        const double offset = info.Length() > 1 ? info[1].ToNumber().DoubleValue() : 0.0;
        const double duration = info.Length() > 2 ? info[2].ToNumber().DoubleValue() : -1.0;

        auto context = m_state->context.lock();
        auto engine = context ? context->engine : nullptr;
        auto dispatcher = m_endedDispatcher;

        m_state->onEnded = [engine, dispatcher]() {
            if (dispatcher)
            {
                dispatcher->NotifyEnded(engine);
            }
        };

        LockAnd(context, [&]() { m_state->Start(when, offset, duration); });
    }

    void NativeAudioBufferSourceNode::Stop(const Napi::CallbackInfo&)
    {
        if (!m_state)
        {
            return;
        }

        LockAnd(m_state->context.lock(), [&]() { m_state->Stop(); });
    }

    void NativeAudioBufferSourceNode::AddEventListener(const Napi::CallbackInfo& info)
    {
        m_eventTarget.AddEventListener(info);
    }

    void NativeAudioBufferSourceNode::RemoveEventListener(const Napi::CallbackInfo& info)
    {
        m_eventTarget.RemoveEventListener(info);
    }
}
