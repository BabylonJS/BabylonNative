#include "StereoPannerNode.h"
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

    void NativeStereoPannerNode::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceMethod("connect", &NativeStereoPannerNode::Connect),
                InstanceMethod("disconnect", &NativeStereoPannerNode::Disconnect),
                InstanceAccessor("pan", &NativeStereoPannerNode::GetPan, nullptr),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Napi::Object NativeStereoPannerNode::CreateInstance(Napi::Env env, const std::shared_ptr<AudioContextState>& context)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto object = func.New({});
        NativeStereoPannerNode::Unwrap(object)->InitializeState(context);
        return object;
    }

    NativeStereoPannerNode::NativeStereoPannerNode(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeStereoPannerNode>{info}
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

    void NativeStereoPannerNode::InitializeState(const std::shared_ptr<AudioContextState>& context)
    {
        m_state = std::make_shared<StereoPannerNodeState>();
        m_state->context = context;
        BindNodeState(Value(), m_state);
        SetAudioNodeProperties(Value(), 1, 1);
        auto getCurrentTime = MakeCurrentTimeGetter(context);
        m_pan = Napi::Persistent(NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->pan), 0.f, getCurrentTime));
    }

    Napi::Value NativeStereoPannerNode::Connect(const Napi::CallbackInfo& info)
    {
        auto destination = UnwrapNodeState(info[0]);
        if (m_state && destination)
        {
            LockAnd(m_state->context.lock(), [&]() { m_state->Connect(destination); });
        }
        return info[0];
    }

    Napi::Value NativeStereoPannerNode::Disconnect(const Napi::CallbackInfo& info)
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

    Napi::Value NativeStereoPannerNode::GetPan(const Napi::CallbackInfo&)
    {
        return m_pan.Value();
    }
}
