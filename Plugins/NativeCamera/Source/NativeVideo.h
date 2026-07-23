#pragma once
#include <napi/napi.h>
#include "CameraDevice.h"
#include "MediaStream.h"
#include "VideoPlayer.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace Babylon::Plugins
{
    // NativeVideo provides a polyfill for the HTMLVideoElement. It supports two media
    // sources, matching the web API:
    //   - srcObject: a MediaStream (camera frames via CameraDevice)
    //   - src: a video file/URL, decoded by a per-instance VideoPlayer (AVPlayer on Apple)
    // In both cases frames are pulled into the Babylon scene through UpdateTexture.
    class NativeVideo : public Napi::ObjectWrap<NativeVideo>
    {
    public:
        static void Initialize(Napi::Env& env);
        static Napi::Object New(const Napi::CallbackInfo& info);
        NativeVideo(const Napi::CallbackInfo& info);
        ~NativeVideo() = default;

        void UpdateTexture(bgfx::TextureHandle textureHandle);

    private:
        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);
        void RaiseEvent(const char* eventType);
        Napi::Value Play(const Napi::CallbackInfo& info);
        void Pause(const Napi::CallbackInfo& info);
        Napi::Value GetVideoWidth(const Napi::CallbackInfo& info);
        Napi::Value GetVideoHeight(const Napi::CallbackInfo& info);
        void SetAttribute(const Napi::CallbackInfo&);
        void RemoveAttribute(const Napi::CallbackInfo&);
        Napi::Value GetReadyState(const Napi::CallbackInfo& info);
        Napi::Value GetHaveCurrentData(const Napi::CallbackInfo& info);
        Napi::Value GetSrcObject(const Napi::CallbackInfo& info);
        void SetSrcObject(const Napi::CallbackInfo& info, const Napi::Value& value);

        // Video-file (src) support.
        Napi::Value GetSrc(const Napi::CallbackInfo& info);
        void SetSrc(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetIsNative(const Napi::CallbackInfo& info);
        Napi::Value GetCurrentTime(const Napi::CallbackInfo& info);
        void SetCurrentTime(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDuration(const Napi::CallbackInfo& info);
        Napi::Value GetLoop(const Napi::CallbackInfo& info);
        void SetLoop(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMuted(const Napi::CallbackInfo& info);
        void SetMuted(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPaused(const Napi::CallbackInfo& info);
        Napi::Value GetEnded(const Napi::CallbackInfo& info);

        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs{};

        bool m_isReady{false};

        bool m_IsPlaying{};

        Napi::ObjectReference m_streamObject{};

        // Video-file (src) support.
        JsRuntime* m_runtime{};
        std::unique_ptr<VideoPlayer> m_videoPlayer{};
        std::string m_src{};
        bool m_loop{};
        bool m_muted{};
        bool m_ended{};
        uint32_t m_lastVideoWidth{};
        uint32_t m_lastVideoHeight{};
        // Keeps the element alive while it owns a media player (media elements with an
        // active source aren't collected mid-playback) and makes the cross-thread event
        // dispatches safe.
        Napi::ObjectReference m_selfRef{};
    };
}
