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
        static void Initialize(Napi::Env& env);
        static inline Napi::FunctionReference constructor{};

        MediaStream(const Napi::CallbackInfo& info);
        
        // MediaStream polyfill: https://developer.mozilla.org/en-US/docs/Web/API/MediaStream
        Napi::Value GetVideoTracks(const Napi::CallbackInfo& info);
        Napi::Value GetAudioTracks(const Napi::CallbackInfo& info);
        
        // MediaStreamTrack polyfill: https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack
        Napi::Value ApplyConstraints(const Napi::CallbackInfo& info);
        
        void UpdateTexture(bgfx::TextureHandle textureHandle);
        
        int Width{0};
        int Height{0};
        
    private:
        std::shared_ptr<Plugins::Camera::Impl> m_cameraImpl;
        JsRuntimeScheduler m_runtimeScheduler;
        
    };

//    class MediaStreamTrack : public Napi::ObjectWrap<MediaStreamTrack>
//    {
//    };
}
