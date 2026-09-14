#include "AudioDestinationNode.h"
#include "AudioEngine.h"

#include <Babylon/JsRuntime.h>

namespace Babylon::Polyfills::Internal
{
    void NativeAudioDestinationNode::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceMethod("connect", &NativeAudioDestinationNode::Connect),
                InstanceMethod("disconnect", &NativeAudioDestinationNode::Disconnect),
                InstanceAccessor("maxChannelCount", &NativeAudioDestinationNode::GetMaxChannelCount, nullptr),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Napi::Object NativeAudioDestinationNode::CreateInstance(Napi::Env env, const std::shared_ptr<DestinationNodeState>& state)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto object = func.New({});
        auto* self = NativeAudioDestinationNode::Unwrap(object);
        self->m_state = state;
        BindNodeState(object, state);
        SetAudioNodeProperties(object, 1, 0);
        object.Set("channelCount", 2);
        return object;
    }

    NativeAudioDestinationNode::NativeAudioDestinationNode(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeAudioDestinationNode>{info}
    {
    }

    std::shared_ptr<AudioNodeState> NativeAudioDestinationNode::State() const
    {
        return m_state;
    }

    Napi::Value NativeAudioDestinationNode::Connect(const Napi::CallbackInfo& info)
    {
        return info[0];
    }

    Napi::Value NativeAudioDestinationNode::Disconnect(const Napi::CallbackInfo& info)
    {
        return info.Env().Undefined();
    }

    Napi::Value NativeAudioDestinationNode::GetMaxChannelCount(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), 2);
    }
}
