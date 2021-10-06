#include "NativeVideo.h"
#include "NativeVideoImpl.h"

namespace Babylon::Plugins
{
    static constexpr auto JS_CLASS_NAME = "NativeVideoStream";

    void NativeVideoStream::Initialize(Napi::Env& env, std::shared_ptr<Plugins::Video::Impl> nativeCameraImpl)
    {
        Napi::Function func = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                InstanceMethod("addEventListener", &NativeVideoStream::AddEventListener),
                InstanceMethod("removeEventListener", &NativeVideoStream::RemoveEventListener),
                InstanceMethod("play", &NativeVideoStream::Play),
                InstanceMethod("pause", &NativeVideoStream::Pause),
                InstanceMethod("setAttribute", &NativeVideoStream::SetAttribute),
                InstanceAccessor("videoWidth", &NativeVideoStream::GetVideoWidth, &NativeVideoStream::SetVideoWidth),
                InstanceAccessor("videoHeight", &NativeVideoStream::GetVideoHeight, &NativeVideoStream::SetVideoHeight),
                InstanceAccessor("frontCamera", nullptr, &NativeVideoStream::SetFrontCamera),
                InstanceAccessor("isNative", &NativeVideoStream::IsNative, nullptr),
                InstanceAccessor("readyState", &NativeVideoStream::GetReadyState, nullptr),
                InstanceAccessor("HAVE_CURRENT_DATA", &NativeVideoStream::GetHaveCurrentData, nullptr),
            });

        env.Global().Set(JS_CLASS_NAME, func);

        NativeCameraImpl = nativeCameraImpl;
    }

    Napi::Object NativeVideoStream::New(const Napi::CallbackInfo& info, uint32_t width, uint32_t height, bool frontCamera)
    {
        return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({ Napi::Value::From(info.Env(), width), Napi::Value::From(info.Env(), height), Napi::Value::From(info.Env(), frontCamera) });
    }

    NativeVideoStream::NativeVideoStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeVideoStream>{ info }
        , m_width{ info[0].As<Napi::Number>().Uint32Value() }
        , m_height{ info[1].As<Napi::Number>().Uint32Value() }
        , m_frontCamera{ info[2].As<Napi::Boolean>().Value() }
    {
    }

    void NativeVideoStream::SetVideoWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_width = value.As<Napi::Number>().Uint32Value();
    }

    Napi::Value NativeVideoStream::GetVideoWidth(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void NativeVideoStream::SetVideoHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_height = value.As<Napi::Number>().Uint32Value();
    }

    Napi::Value NativeVideoStream::GetVideoHeight(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void NativeVideoStream::SetFrontCamera(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_frontCamera = value.As<Napi::Boolean>().Value();
    }

    Napi::Value NativeVideoStream::IsNative(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), true);
    }

    void NativeVideoStream::SetAttribute(const Napi::CallbackInfo&)
    {
    }

    Napi::Value NativeVideoStream::GetReadyState(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), 10u);
    }

    Napi::Value NativeVideoStream::GetHaveCurrentData(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), 1u);
    }

    void NativeVideoStream::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        NativeCameraImpl->UpdateTexture(textureHandle);
    }

    void NativeVideoStream::AddEventListener(const Napi::CallbackInfo& info)
    {
        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function eventHandler = info[1].As<Napi::Function>();

        const auto& eventHandlerRefs = m_eventHandlerRefs[eventType];
        for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
        {
            if (it->Value() == eventHandler)
            {
                throw Napi::Error::New(info.Env(), "Cannot add the same event handler twice");
            }
        }

        m_eventHandlerRefs[eventType].push_back(Napi::Persistent(eventHandler));
    }

    void NativeVideoStream::RemoveEventListener(const Napi::CallbackInfo& info)
    {
        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function eventHandler = info[1].As<Napi::Function>();
        auto itType = m_eventHandlerRefs.find(eventType);
        if (itType != m_eventHandlerRefs.end())
        {
            auto& eventHandlerRefs = itType->second;
            for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
            {
                if (it->Value() == eventHandler)
                {
                    eventHandlerRefs.erase(it);
                    break;
                }
            }
        }
    }

    void NativeVideoStream::RaiseEvent(const char* eventType)
    {
        auto it = m_eventHandlerRefs.find(eventType);
        if (it != m_eventHandlerRefs.end())
        {
            const auto& eventHandlerRefs = it->second;
            for (const auto& eventHandlerRef : eventHandlerRefs)
            {
                eventHandlerRef.Call({});
            }
        }
    }

    void NativeVideoStream::Play(const Napi::CallbackInfo& /*info*/)
    {
        if (!m_IsPlaying)
        {
            m_IsPlaying = true;
            NativeCameraImpl->Open(m_width, m_height, m_frontCamera);
            RaiseEvent("playing");
        }
    }

    void NativeVideoStream::Pause(const Napi::CallbackInfo& /*info*/)
    {
        m_IsPlaying = false;
        NativeCameraImpl->Close();
    }


    Video::Video(std::shared_ptr<Impl> impl)
        : m_impl{ std::move(impl) }
    {
    }

    Video Video::Initialize(Napi::Env env)
    {
        auto impl{ std::make_shared<Impl>(env) };

        Babylon::Plugins::NativeVideoStream::Initialize(env, impl);

        return { impl };
    }
}