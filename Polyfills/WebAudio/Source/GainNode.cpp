#include "GainNode.h"
#include "AudioContext.h"
#include "AudioEngine.h"
#include "AudioParam.h"

#include <Babylon/JsRuntime.h>

namespace Babylon::Polyfills::Internal
{
    void NativeGainNode::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceMethod("connect", &NativeGainNode::Connect),
                InstanceMethod("disconnect", &NativeGainNode::Disconnect),
                InstanceAccessor("gain", &NativeGainNode::GetGain, nullptr),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Napi::Object NativeGainNode::CreateInstance(Napi::Env env, const std::shared_ptr<AudioContextState>& context)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONSTRUCTOR_NAME).As<Napi::Function>();
        // Pass a placeholder; InitializeState is called from constructor when context object is provided.
        // For factory creation we construct then bind.
        auto object = func.New({});
        auto* self = NativeGainNode::Unwrap(object);
        self->InitializeState(context);
        return object;
    }

    NativeGainNode::NativeGainNode(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeGainNode>{info}
    {
        if (info.Length() > 0 && info[0].IsObject())
        {
            auto* context = NativeAudioContext::Unwrap(info[0].As<Napi::Object>());
            if (context)
            {
                info.This().As<Napi::Object>().Set("context", info[0]);
                InitializeState(context->State());
            }
        }
    }

    void NativeGainNode::InitializeState(const std::shared_ptr<AudioContextState>& context)
    {
        m_state = std::make_shared<GainNodeState>();
        m_state->context = context;
        BindNodeState(Value(), m_state);
        SetAudioNodeProperties(Value(), 1, 1);

        auto getCurrentTime = MakeCurrentTimeGetter(context);
        auto gain = NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->gain), 1.f, getCurrentTime);
        m_gain = Napi::Persistent(gain);
    }

    Napi::Value NativeGainNode::Connect(const Napi::CallbackInfo& info)
    {
        auto destination = UnwrapNodeState(info[0]);
        if (m_state && destination)
        {
            auto context = m_state->context.lock();
            if (context && context->engine)
            {
                std::lock_guard<std::mutex> lock{context->engine->GraphMutex()};
                m_state->Connect(destination);
            }
            else
            {
                m_state->Connect(destination);
            }
        }
        return info[0];
    }

    Napi::Value NativeGainNode::Disconnect(const Napi::CallbackInfo& info)
    {
        if (!m_state)
        {
            return info.Env().Undefined();
        }

        auto context = m_state->context.lock();
        auto lockGraph = [&](auto&& fn) {
            if (context && context->engine)
            {
                std::lock_guard<std::mutex> lock{context->engine->GraphMutex()};
                fn();
            }
            else
            {
                fn();
            }
        };

        if (info.Length() > 0)
        {
            auto destination = UnwrapNodeState(info[0]);
            lockGraph([&]() { m_state->Disconnect(destination); });
        }
        else
        {
            lockGraph([&]() { m_state->Disconnect(); });
        }

        return info.Env().Undefined();
    }

    Napi::Value NativeGainNode::GetGain(const Napi::CallbackInfo&)
    {
        return m_gain.Value();
    }
}
