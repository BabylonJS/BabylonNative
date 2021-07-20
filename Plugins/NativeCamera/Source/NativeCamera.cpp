#include <napi/napi.h>
#include <NativeCamera.h>
#include "NativeVideo.h"
#include "NativeCameraImpl.h"
#include <Babylon/JsRuntime.h>
#include <GraphicsImpl.h>
#include <vector>
#include <algorithm>

namespace Babylon
{
    namespace Plugins
    {
        // Move this struct to Graphics
        struct TextureData final
        {
            ~TextureData()
            {
                if (bgfx::isValid(Handle))
                {
                    bgfx::destroy(Handle);
                }
            }

            bgfx::TextureHandle Handle{ bgfx::kInvalidHandle };
            uint32_t Width{ 0 };
            uint32_t Height{ 0 };
            uint32_t Flags{ 0 };
            uint8_t AnisotropicLevel{ 0 };
        };

        class NativeCamera : public Napi::ObjectWrap<NativeCamera>
        {
            static constexpr auto JS_NAVIGATOR_NAME = "navigator";
            static constexpr auto JS_CLASS_NAME = "_NativeCamera";
            static constexpr auto JS_NATIVECAMERA_CONSTRUCTOR_NAME = "NativeCamera";

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

                JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_NATIVECAMERA_CONSTRUCTOR_NAME, func);

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
                mediaDevices.Set("getUserMedia", Napi::Function::New(env, &NativeCamera::GetUserMedia, "getUserMedia"));
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
                const auto texture = info[0].As<Napi::External<TextureData>>().Data();
                auto videoObject = NativeVideo::Unwrap(info[1].As<Napi::Object>());

                videoObject->UpdateTexture(texture->Handle);
            }

            static Napi::Value GetUserMedia(const Napi::CallbackInfo& info) 
            {
                auto env = info.Env();
                auto deferred{Napi::Promise::Deferred::New(env)};
                auto promise{deferred.Promise()};

                auto& jsRuntime{JsRuntime::GetFromJavaScript(env)};
                jsRuntime.Dispatch([deferred{std::move(deferred)}](Napi::Env env) {
                    deferred.Resolve(env.Null());
                });

                return promise;
            }
        };

        Camera::Camera(std::shared_ptr<Impl> impl)
            : m_impl{std::move(impl)}
        {
        }

        Camera Camera::Initialize(Napi::Env env, bool overrideCameraTexture)
        {
            auto impl{std::make_shared<Impl>(env, overrideCameraTexture)};

            Babylon::Plugins::NativeVideo::Initialize(env, impl);
            Babylon::Plugins::NativeCamera::Initialize(env);

            return {impl};
        }

        void Camera::SetTextureOverride(void* texturePtr)
        {
           m_impl->SetTextureOverride(texturePtr);
        }
    }
}