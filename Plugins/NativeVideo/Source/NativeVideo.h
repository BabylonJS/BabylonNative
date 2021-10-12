#include <napi/napi.h>
#include "NativeVideoImpl.h"
#include <Babylon/JsRuntime.h>
#include <GraphicsImpl.h>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace Babylon::Plugins
{
    class NativeVideoStream : public Napi::ObjectWrap<NativeVideoStream>
    {

    public:
        static void Initialize(Napi::Env& env, std::shared_ptr<Plugins::Video::Impl> nativeCameraImpl);
        static Napi::Object New(const Napi::CallbackInfo& info);
        NativeVideoStream(const Napi::CallbackInfo& info);
        ~NativeVideoStream() = default;

    private:
        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);
        void RaiseEvent(const char* eventType);
        Napi::Value Play(const Napi::CallbackInfo& info);
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
        Napi::Value NativeVideoStream::GetSrc(const Napi::CallbackInfo& info);
        void NativeVideoStream::SetSrc(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value NativeVideoStream::GetDst(const Napi::CallbackInfo& info);
        void NativeVideoStream::SetDst(const Napi::CallbackInfo&, const Napi::Value& value);
        void UpdateVideoTexture(const Napi::CallbackInfo& info);
        void AddFrame(const Napi::CallbackInfo& info);
        void Stop(const Napi::CallbackInfo& info);

        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs{};
        uint32_t m_width{};
        uint32_t m_height{};
        bool m_frontCamera{};

        bool m_IsPlaying{};

        std::string m_src{};
        std::string m_dst{};

        static inline std::shared_ptr<Plugins::Video::Impl> m_nativeCameraImpl{};
    };
}