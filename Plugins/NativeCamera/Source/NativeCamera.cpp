#include <Babylon/Plugins/NativeCamera.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/napi_pointer.h>
#include "NativeVideo.h"
#include "MediaDevices.h"

namespace Babylon::Plugins::Internal
{
    // NativeVideoAccessor maintains backwards compatibility with the Babylon.JS javascript contract
    // moving forward the NativeVideo object should be called directly instead of using this accessor class
    class NativeCamera : public Napi::ObjectWrap<NativeCamera>
    {
    public:
        static void Initialize(Napi::Env& env)
        {
            Napi::Function func = DefineClass(
                env,
                "_NativeCamera",
                {
                    InstanceMethod("createVideo", &NativeCamera::CreateVideo),
                    InstanceMethod("updateVideoTexture", &NativeCamera::UpdateVideoTexture),
                });

            JsRuntime::NativeObject::GetFromJavaScript(env).Set("Camera", func);
        }

        NativeCamera(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeCamera>{info}
        {
        }

    private:
        Napi::Value CreateVideo(const Napi::CallbackInfo& info)
        {
            return NativeVideo::New(info);
        }

        void UpdateVideoTexture(const Napi::CallbackInfo& info)
        {
            const auto& texture = *info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
            auto videoObject = NativeVideo::Unwrap(info[1].As<Napi::Object>());

            videoObject->UpdateTexture(texture.Handle());
        }
    };
}

namespace Babylon::Plugins::NativeCamera
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::NativeVideo::Initialize(env);
        Babylon::Plugins::MediaDevices::Initialize(env);
        Babylon::Plugins::Internal::NativeCamera::Initialize(env);
    }
}
