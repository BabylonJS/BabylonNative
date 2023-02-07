#pragma once
#include <napi/napi.h>
#include "CameraDevice.h"
#include "MediaStream.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace Babylon::Plugins
{
    // NativeVideo provides a polyfill for the HTMLVideoElement which is used in conjunction with a MediaStream object to
    // pull frames out of a camera and render them into the Babylon scene.
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

        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs{};

        bool m_isReady{false};

        bool m_IsPlaying{};
        
        Napi::ObjectReference m_streamObject{};
    };
}
