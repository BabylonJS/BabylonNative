#include <napi/napi.h>
#include "NativeCamera.h"

#include <Babylon/JsRuntime.h>
#include <GraphicsImpl.h>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace Babylon::Plugins::Internal
{
    class NativeVideo : public Napi::ObjectWrap<NativeVideo>
    {
        static constexpr auto JS_CLASS_NAME = "NativeVideo";

    public:
        static void Initialize(Napi::Env& env);
        static Napi::Object New(const Napi::CallbackInfo& info, uint32_t width, uint32_t height, bool frontCamera);
        NativeVideo(const Napi::CallbackInfo& info);
        ~NativeVideo() = default;

        void UpdateTexture(bgfx::TextureHandle textureHandle);

    private:
        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);
        void RaiseEvent(const char* eventType);
        void Play(const Napi::CallbackInfo& info);
        void Pause(const Napi::CallbackInfo& info);
        void SetVideoWidth(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetVideoWidth(const Napi::CallbackInfo& info);
        void SetVideoHeight(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetVideoHeight(const Napi::CallbackInfo& info);
        void SetFrontCamera(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetAttribute(const Napi::CallbackInfo&);
        Napi::Value IsNative(const Napi::CallbackInfo&);
        Napi::Value GetReadyState(const Napi::CallbackInfo& info);
        Napi::Value GetHaveCurrentData(const Napi::CallbackInfo& info);

        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs{};
        uint32_t m_width{};
        uint32_t m_height{};
        bool m_frontCamera{};

        bool m_IsPlaying{};

        std::unique_ptr<Babylon::Plugins::Internal::CameraInterface> m_cameraInterface{};
    };
}