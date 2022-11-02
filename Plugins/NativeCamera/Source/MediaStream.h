#pragma once
#include <napi/napi.h>
#include "NativeCameraImpl.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>

namespace Babylon::Plugins
{
    class MediaStream : public Napi::ObjectWrap<MediaStream>
    {
    public:
        static constexpr auto JS_CLASS_NAME = "_MediaStream";
        
        static arcana::task<Napi::Object, std::exception_ptr> New(Napi::Env env, Napi::Object constraints);
        
        MediaStream(const Napi::CallbackInfo& info);
        ~MediaStream()=default;
        
        // MediaStream polyfill: https://developer.mozilla.org/en-US/docs/Web/API/MediaStream
        Napi::Value GetVideoTracks(const Napi::CallbackInfo& info);
        Napi::Value GetAudioTracks(const Napi::CallbackInfo& info);
        
        // MediaStreamTrack polyfill: https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack
        Napi::Value ApplyConstraints(const Napi::CallbackInfo& info);
        Napi::Value GetCapabilities(const Napi::CallbackInfo& info);
        Napi::Value GetSettings(const Napi::CallbackInfo& info);
        Napi::Value GetConstraints(const Napi::CallbackInfo& info);
        
        void UpdateTexture(bgfx::TextureHandle textureHandle);
        
        int Width{0};
        int Height{0};
        
    private:
        arcana::task<void, std::exception_ptr> ApplyInitialConstraints(Napi::Env env, Napi::Object constraints);
        std::pair<std::shared_ptr<CameraDevice>, CameraTrack*> FindBestCameraStream(Napi::Object constraints);
        bool UpdateConstraints(Napi::Env env, Napi::Object constraints);
        
        std::unique_ptr<Plugins::Camera::Impl> m_cameraImpl;
        JsRuntimeScheduler m_runtimeScheduler;
        
        std::shared_ptr<CameraDevice> m_cameraDevice{nullptr};
        CameraTrack* m_cameraResolution{nullptr};
        
        Napi::ObjectReference m_currentConstraints{};
    };

//    class MediaStreamTrack : public Napi::ObjectWrap<MediaStreamTrack>
//    {
//    };
}
