#include "NativeVideo.h"
#include "CameraDevice.h"

namespace Babylon::Plugins
{
    static constexpr auto JS_CLASS_NAME = "HTMLVideoElement";

    void NativeVideo::Initialize(Napi::Env& env)
    {
        Napi::Function func = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                InstanceMethod("addEventListener", &NativeVideo::AddEventListener),
                InstanceMethod("removeEventListener", &NativeVideo::RemoveEventListener),
                InstanceMethod("play", &NativeVideo::Play),
                InstanceMethod("pause", &NativeVideo::Pause),
                InstanceMethod("setAttribute", &NativeVideo::SetAttribute),
                InstanceMethod("removeAttribute", &NativeVideo::RemoveAttribute),
                InstanceAccessor("videoWidth", &NativeVideo::GetVideoWidth, nullptr),
                InstanceAccessor("videoHeight", &NativeVideo::GetVideoHeight, nullptr),
                InstanceAccessor("readyState", &NativeVideo::GetReadyState, nullptr),
                InstanceAccessor("HAVE_CURRENT_DATA", &NativeVideo::GetHaveCurrentData, nullptr),
                InstanceAccessor("srcObject", &NativeVideo::GetSrcObject, &NativeVideo::SetSrcObject)
            });

        env.Global().Set(JS_CLASS_NAME, func);
    }

    Napi::Object NativeVideo::New(const Napi::CallbackInfo& info)
    {
        return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
    }

    NativeVideo::NativeVideo(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeVideo>{info}
    {
    }

    Napi::Value NativeVideo::GetVideoWidth(const Napi::CallbackInfo& /*info*/)
    {
        if (!m_streamObject.Value().IsNull() && !m_streamObject.Value().IsUndefined())
        {
            return Napi::Value::From(Env(), MediaStream::Unwrap(m_streamObject.Value())->Width);
        }

        return Napi::Value::From(Env(), 0);
    }

    Napi::Value NativeVideo::GetVideoHeight(const Napi::CallbackInfo& /*info*/)
    {
        if (!m_streamObject.Value().IsNull() && !m_streamObject.Value().IsUndefined())
        {
            return Napi::Value::From(Env(), MediaStream::Unwrap(m_streamObject.Value())->Height);
        }

        return Napi::Value::From(Env(), 0);
    }

    void NativeVideo::SetAttribute(const Napi::CallbackInfo&)
    {
        // This function is defined to fulfill the web api, but not used natively
    }

    void NativeVideo::RemoveAttribute(const Napi::CallbackInfo&)
    {
        // This function is defined to fulfill the web api, but not used natively
    }

    Napi::Value NativeVideo::GetReadyState(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), m_isReady ? 10u : 0u);
    }

    Napi::Value NativeVideo::GetHaveCurrentData(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), 1u);
    }

    void NativeVideo::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        // Only update the texture if we're playing and the srcObject is an instance of a MediaStream
        if (m_IsPlaying && !m_streamObject.Value().IsNull() && !m_streamObject.Value().IsUndefined())
        {
            if (MediaStream::Unwrap(m_streamObject.Value())->UpdateTexture(textureHandle))
            {
                // The video dimensions have changed, raise the resize event to observers
                RaiseEvent("resize");
            }
        }
    }

    void NativeVideo::AddEventListener(const Napi::CallbackInfo& info)
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

    void NativeVideo::RemoveEventListener(const Napi::CallbackInfo& info)
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

    void NativeVideo::RaiseEvent(const char* eventType)
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

    Napi::Value NativeVideo::Play(const Napi::CallbackInfo& info)
    {
        auto env{info.Env()};
        auto deferred{Napi::Promise::Deferred::New(env)};

        if (!m_IsPlaying && !m_streamObject.Value().IsNull() && !m_streamObject.Value().IsUndefined())
        {
            m_IsPlaying = true;
            RaiseEvent("playing");
        }

        deferred.Resolve(env.Undefined());

        return deferred.Promise();
    }

    void NativeVideo::Pause(const Napi::CallbackInfo& /*info*/)
    {
        m_IsPlaying = false;
    }

    void NativeVideo::SetSrcObject(const Napi::CallbackInfo& info, const Napi::Value& value) {
        auto env{info.Env()};

        if (value.IsNull() || value.IsUndefined() || !value.As<Napi::Object>().InstanceOf(MediaStream::GetConstructor(env)))
        {
            m_streamObject = Napi::ObjectReference();
            this->m_isReady = false;
            this->m_IsPlaying = false;
            return;
        }

        // We've received a MediaStream object
        m_streamObject = Napi::Persistent(value.As<Napi::Object>());

        this->m_isReady = true;

        // Now that we have a src object the resize event should be fired to notify observers of the new video dimensions
        RaiseEvent("resize");
    }

    Napi::Value NativeVideo::GetSrcObject(const Napi::CallbackInfo& info) {
        // If the streamObject has not yet been defined return Null instead to match the web API
        return m_streamObject.IsEmpty() ? info.Env().Null() : m_streamObject.Value();
    }
}
