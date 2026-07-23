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
                InstanceAccessor("srcObject", &NativeVideo::GetSrcObject, &NativeVideo::SetSrcObject),
                InstanceAccessor("src", &NativeVideo::GetSrc, &NativeVideo::SetSrc),
                InstanceAccessor("isNative", &NativeVideo::GetIsNative, nullptr),
                InstanceAccessor("currentTime", &NativeVideo::GetCurrentTime, &NativeVideo::SetCurrentTime),
                InstanceAccessor("duration", &NativeVideo::GetDuration, nullptr),
                InstanceAccessor("loop", &NativeVideo::GetLoop, &NativeVideo::SetLoop),
                InstanceAccessor("muted", &NativeVideo::GetMuted, &NativeVideo::SetMuted),
                InstanceAccessor("paused", &NativeVideo::GetPaused, nullptr),
                InstanceAccessor("ended", &NativeVideo::GetEnded, nullptr),
            });

        env.Global().Set(JS_CLASS_NAME, func);
    }

    Napi::Object NativeVideo::New(const Napi::CallbackInfo& info)
    {
        return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
    }

    NativeVideo::NativeVideo(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeVideo>{info}
        , m_runtime{&JsRuntime::GetFromJavaScript(info.Env())}
    {
    }

    Napi::Value NativeVideo::GetVideoWidth(const Napi::CallbackInfo& /*info*/)
    {
        if (m_videoPlayer != nullptr)
        {
            return Napi::Value::From(Env(), m_videoPlayer->GetWidth());
        }

        if (!m_streamObject.Value().IsNull() && !m_streamObject.Value().IsUndefined())
        {
            return Napi::Value::From(Env(), MediaStream::Unwrap(m_streamObject.Value())->Width);
        }

        return Napi::Value::From(Env(), 0);
    }

    Napi::Value NativeVideo::GetVideoHeight(const Napi::CallbackInfo& /*info*/)
    {
        if (m_videoPlayer != nullptr)
        {
            return Napi::Value::From(Env(), m_videoPlayer->GetHeight());
        }

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
        // Video-file playback (src). Not gated on m_IsPlaying so a seek while paused
        // still refreshes the current frame; the player is a cheap no-op when idle.
        else if (m_videoPlayer != nullptr && m_videoPlayer->IsReady())
        {
            m_videoPlayer->UpdateTexture(textureHandle);

            const uint32_t width = m_videoPlayer->GetWidth();
            const uint32_t height = m_videoPlayer->GetHeight();
            if (width != m_lastVideoWidth || height != m_lastVideoHeight)
            {
                m_lastVideoWidth = width;
                m_lastVideoHeight = height;
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
        else if (!m_IsPlaying && m_videoPlayer != nullptr)
        {
            m_IsPlaying = true;
            m_ended = false;
            // Safe to call before the player is ready; playback starts once buffered.
            m_videoPlayer->Play();
            RaiseEvent("playing");
        }

        deferred.Resolve(env.Undefined());

        return deferred.Promise();
    }

    void NativeVideo::Pause(const Napi::CallbackInfo& /*info*/)
    {
        const bool wasPlaying = m_IsPlaying;
        m_IsPlaying = false;
        if (m_videoPlayer != nullptr)
        {
            m_videoPlayer->Pause();
            if (wasPlaying)
            {
                RaiseEvent("pause");
                // Babylon.js's VideoTexture listens for "paused" (its own convention).
                RaiseEvent("paused");
            }
        }
    }

    void NativeVideo::SetSrcObject(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        auto env{info.Env()};

        if (value.IsNull() || value.IsUndefined() || !value.As<Napi::Object>().InstanceOf(MediaStream::GetConstructor(env)))
        {
            m_streamObject = Napi::ObjectReference();
            this->m_isReady = false;
            this->m_IsPlaying = false;
            return;
        }

        // We've received a MediaStream object. A media element has a single source:
        // assigning a stream replaces any file player.
        m_videoPlayer.reset();
        m_src.clear();
        m_streamObject = Napi::Persistent(value.As<Napi::Object>());
        this->m_isReady = true;

        // Now that we have a src object the resize event should be fired to notify observers of the new video dimensions
        RaiseEvent("resize");
    }

    Napi::Value NativeVideo::GetSrcObject(const Napi::CallbackInfo& info)
    {
        // If the streamObject has not yet been defined return Null instead to match the web API
        return m_streamObject.IsEmpty() ? info.Env().Null() : m_streamObject.Value();
    }

    Napi::Value NativeVideo::GetSrc(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_src);
    }

    void NativeVideo::SetSrc(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        m_videoPlayer.reset();
        m_isReady = false;
        m_IsPlaying = false;
        m_ended = false;
        m_lastVideoWidth = 0;
        m_lastVideoHeight = 0;
        m_src.clear();

        if (value.IsNull() || value.IsUndefined() || !value.IsString())
        {
            RaiseEvent("emptied");
            return;
        }

        m_src = value.As<Napi::String>().Utf8Value();
        if (m_src.empty())
        {
            RaiseEvent("emptied");
            return;
        }

        // A media element with an active source is kept alive (and makes the
        // cross-thread event dispatches below safe).
        m_selfRef = Napi::Persistent(info.This().As<Napi::Object>());

        // Assigning a file source replaces any stream source.
        m_streamObject = Napi::ObjectReference();

#ifdef __APPLE__
        auto& deviceContext = Graphics::DeviceContext::GetFromJavaScript(info.Env());
        m_videoPlayer = VideoPlayer::Create(deviceContext, m_src, [this](VideoPlayer::Event event) {
            // May be called on any thread — marshal to the JS thread. m_selfRef keeps
            // `this` alive for the lifetime of the dispatch.
            m_runtime->Dispatch([this, event](Napi::Env) {
                switch (event)
                {
                    case VideoPlayer::Event::MetadataLoaded:
                        m_isReady = true;
                        RaiseEvent("loadedmetadata");
                        RaiseEvent("loadeddata");
                        RaiseEvent("canplay");
                        // play() may have been called before the media was ready.
                        if (m_IsPlaying && m_videoPlayer != nullptr)
                        {
                            m_videoPlayer->Play();
                        }
                        break;
                    case VideoPlayer::Event::Ended:
                        m_IsPlaying = false;
                        m_ended = true;
                        RaiseEvent("ended");
                        break;
                    case VideoPlayer::Event::Seeked:
                        RaiseEvent("seeked");
                        break;
                }
            });
        });
        if (m_videoPlayer != nullptr)
        {
            m_videoPlayer->SetLoop(m_loop);
            m_videoPlayer->SetMuted(m_muted);
        }
#endif
    }

    Napi::Value NativeVideo::GetIsNative(const Napi::CallbackInfo& info)
    {
        // Lets Babylon.js's VideoTexture accept this element directly (its _getVideo
        // returns native elements as-is instead of calling document.createElement).
        return Napi::Value::From(info.Env(), true);
    }

    Napi::Value NativeVideo::GetCurrentTime(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_videoPlayer != nullptr ? m_videoPlayer->GetCurrentTime() : 0.0);
    }

    void NativeVideo::SetCurrentTime(const Napi::CallbackInfo& /*info*/, const Napi::Value& value)
    {
        if (m_videoPlayer != nullptr && value.IsNumber())
        {
            m_ended = false;
            m_videoPlayer->Seek(value.As<Napi::Number>().DoubleValue());
        }
    }

    Napi::Value NativeVideo::GetDuration(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_videoPlayer != nullptr ? m_videoPlayer->GetDuration() : 0.0);
    }

    Napi::Value NativeVideo::GetLoop(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_loop);
    }

    void NativeVideo::SetLoop(const Napi::CallbackInfo& /*info*/, const Napi::Value& value)
    {
        m_loop = value.ToBoolean();
        if (m_videoPlayer != nullptr)
        {
            m_videoPlayer->SetLoop(m_loop);
        }
    }

    Napi::Value NativeVideo::GetMuted(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_muted);
    }

    void NativeVideo::SetMuted(const Napi::CallbackInfo& /*info*/, const Napi::Value& value)
    {
        m_muted = value.ToBoolean();
        if (m_videoPlayer != nullptr)
        {
            m_videoPlayer->SetMuted(m_muted);
        }
    }

    Napi::Value NativeVideo::GetPaused(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), !m_IsPlaying);
    }

    Napi::Value NativeVideo::GetEnded(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_ended);
    }
}
