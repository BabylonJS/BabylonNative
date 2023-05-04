#pragma once
#include <napi/napi.h>
#include "CameraDevice.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>

namespace Babylon::Plugins
{
    // The MediaStream object provides a polyfill for both the MediaStream and MediaStreamTrack web API's
    // that enable control over selecting a camera stream available on the device and modifying capabilities
    // of the camera on the fly while the stream is rendering.
    // https://developer.mozilla.org/en-US/docs/Web/API/MediaStream
    class MediaStream : public Napi::ObjectWrap<MediaStream>
    {
        static constexpr auto JS_CLASS_NAME = "_MediaStream";

    public:
        static arcana::task<Napi::Object, std::exception_ptr> NewAsync(Napi::Env env, Napi::Object constraints);
        static Napi::Function GetConstructor(Napi::Env env);

        MediaStream(const Napi::CallbackInfo& info);
        ~MediaStream();

        // MediaStream polyfill: https://developer.mozilla.org/en-US/docs/Web/API/MediaStream
        Napi::Value GetVideoTracks(const Napi::CallbackInfo& info);
        Napi::Value GetAudioTracks(const Napi::CallbackInfo& info);

        // MediaStreamTrack polyfill: https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack
        Napi::Value ApplyConstraints(const Napi::CallbackInfo& info);
        Napi::Value GetCapabilities(const Napi::CallbackInfo& info);
        Napi::Value GetSettings(const Napi::CallbackInfo& info);
        Napi::Value GetConstraints(const Napi::CallbackInfo& info);
        void Stop(const Napi::CallbackInfo& info);

        // Update the camera texture and return true if the dimensions have changed, false otherwise
        bool UpdateTexture(bgfx::TextureHandle textureHandle);

        uint32_t Width{0};
        uint32_t Height{0};

    private:
        arcana::task<void, std::exception_ptr> ApplyInitialConstraintsAsync(Napi::Object constraints);
        std::optional<std::pair<CameraDevice, const CameraTrack&>> FindBestCameraStream(Napi::Object constraints);
        bool UpdateConstraints(Napi::Object constraints);

        // Capture CameraDevice in a shared_ptr because the iOS implementation relies on the `shared_from_this` syntax for async work
        std::shared_ptr<CameraDevice> m_cameraDevice{};
        JsRuntimeScheduler m_runtimeScheduler;

        Napi::ObjectReference m_currentConstraints{};
    };
}
