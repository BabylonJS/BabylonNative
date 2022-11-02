#include <napi/napi.h>
#include <napi/napi_pointer.h>
#include <NativeCamera.h>
#include "MediaStream.h"
#include "NativeVideo.h"
#include "NativeCameraImpl.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <vector>
#include <algorithm>

namespace Babylon
{
    namespace Plugins
    {
        class NativeCamera : public Napi::ObjectWrap<NativeCamera>
        {
            static constexpr auto JS_NAVIGATOR_NAME = "navigator";
            static constexpr auto JS_CLASS_NAME = "_NativeCamera";
            static constexpr auto JS_CONSTRUCTOR_NAME = "Camera";

        public:

            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{ env };

                Napi::Function func = NativeCamera::DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        NativeCamera::InstanceMethod("createVideo", &NativeCamera::CreateVideo),
                        NativeCamera::InstanceMethod("updateVideoTexture", &NativeCamera::UpdateVideoTexture),
                    });

                JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);

                // create or get global navigator object
                Napi::Object global = env.Global();
                Napi::Object navigator;
                if (global.Has(JS_NAVIGATOR_NAME))
                {
                    navigator = global.Get(JS_NAVIGATOR_NAME).As<Napi::Object>();
                }
                else
                {
                    navigator = Napi::Object::New(env);
                    global.Set(JS_NAVIGATOR_NAME, navigator);
                }

                // append media devices to navigator
                Napi::Object mediaDevices = Napi::Object::New(env);
                mediaDevices.Set("getUserMedia", Napi::Function::New(env, Camera::GetUserMedia, "getUserMedia"));
                navigator.Set("mediaDevices", mediaDevices);
            }

            NativeCamera(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<NativeCamera>{ info }
            {
            }

        private:

            Napi::Value CreateVideo(const Napi::CallbackInfo& info)
            {
                auto constraints = info[0].As<Napi::Object>();
                uint32_t maxWidth{256}, maxHeight{256};
                std::string facingMode{};

                auto maxWidthValue{constraints.Get("maxWidth")};
                auto maxHeightValue{constraints.Get("maxHeight")};
                auto facingModeValue{constraints.Get("facingMode")};
                if (maxWidthValue.IsNumber())
                {
                    maxWidth = maxWidthValue.As<Napi::Number>().Uint32Value();
                }
                if (maxHeightValue.IsNumber())
                {
                    maxHeight = maxHeightValue.As<Napi::Number>().Uint32Value();
                }
                if (facingModeValue.IsString())
                {
                    facingMode = facingModeValue.As<Napi::String>().Utf8Value();
                }

                return NativeVideo::New(info, maxWidth, maxHeight, facingMode == "user");
            }

            void UpdateVideoTexture(const Napi::CallbackInfo& info)
            {
                const auto& texture = *info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
                auto videoObject = NativeVideo::Unwrap(info[1].As<Napi::Object>());

                videoObject->UpdateTexture(texture.Handle());
            }
        };

        Camera::Camera()
        {
        }

        Camera Camera::Initialize(Napi::Env env)
        {
            Babylon::Plugins::NativeVideo::Initialize(env);
            Babylon::Plugins::NativeCamera::Initialize(env);

            return {};
        }

        Napi::Value Camera::GetUserMedia(const Napi::CallbackInfo& info)
        {
            auto env = info.Env();
            
            auto deferred{Napi::Promise::Deferred::New(env)};
            auto promise{deferred.Promise()};

            // Extract the video constraints as we only support video for the time being
            auto videoConstraints{ Napi::Object::New(env) };
            if (info.Length() > 0 && info[0].IsObject())
            {
                Napi::Object constraints{ info[0].As<Napi::Object>() };
                if (constraints.Get("video").IsObject())
                {
                    videoConstraints = constraints.Get("video").As<Napi::Object>();
                }
            }

            auto runtimeScheduler{ std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(env)) };
            MediaStream::New(env, videoConstraints).then(*runtimeScheduler, arcana::cancellation::none(), [runtimeScheduler, env, deferred](const arcana::expected<Napi::Object, std::exception_ptr>& result)
            {
                if (result.has_error())
                {
                    deferred.Reject(Napi::Error::New(env, result.error()).Value());
                    return;
                }

                deferred.Resolve(result.value());
            });
            
            return static_cast<Napi::Value>(promise);
        }
    }
}
