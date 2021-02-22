#include "NativeCamera.h"

#include <Babylon/JsRuntime.h>
#include <GraphicsImpl.h>

#include <vector>

namespace Babylon::Plugins::Internal
{
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
            mediaDevices.Set("getUserMedia", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                auto env = info.Env();
                auto deferred{ Napi::Promise::Deferred::New(env) };
                auto promise{ deferred.Promise() };
               
                auto& jsRuntime{ JsRuntime::GetFromJavaScript(env) };
                    jsRuntime.Dispatch([deferred{ std::move(deferred) }](Napi::Env env) {
                        deferred.Resolve(env.Null());
                    });
                
                return std::move(promise);
                }));
            navigator.Set("mediaDevices", mediaDevices);
        }

        NativeCamera(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeCamera>{ info }
        {
        }

        ~NativeCamera()
        {
        }

    private:
        Napi::Value CreateVideo(const Napi::CallbackInfo& info)
        {
            const auto env = info.Env();
            Napi::Object video = Napi::Object::New(env);

            video.Set("videoWidth", Napi::Number::New(env, 256).As<Napi::Value>());
            video.Set("videoHeight", Napi::Number::New(env, 256).As<Napi::Value>());
            video.Set("readyState", Napi::Number::New(env, 10).As<Napi::Value>());
            video.Set("HAVE_CURRENT_DATA", Napi::Number::New(env, 1).As<Napi::Value>());
            video.Set("isNative", Napi::Boolean::New(env, true).As<Napi::Value>());

            video.Set("setAttribute", Napi::Function::New(env,[](const Napi::CallbackInfo& /*info*/){
                }));

            video.Set("addEventListener", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                auto callback = info[1].As<Napi::Function>();
                info.This().ToObject().Set("_cb", callback.As<Napi::Object>().As<Napi::Value>());
                }));

            video.Set("removeEventListener", Napi::Function::New(env, [](const Napi::CallbackInfo& ) { }));

            video.Set("play", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                auto _this = info.This().ToObject();
                auto cb = _this.Get("_cb");
                if (!cb.IsFunction())
                {
                    throw Napi::Error::New(info.Env(), "Invalid function for video playback.");
                }
                cb.As<Napi::Function>().Call({ });
                }));

            video.Set("pause", Napi::Function::New(env, [](const Napi::CallbackInfo& ) { }));

            return video;
        }

        void UpdateVideoTexture(const Napi::CallbackInfo& info)
        {
            const auto nativeTextureObject = info[0].As<Napi::Object>();
            const auto videoObject = info[0].As<Napi::Object>();

            // Lazy video object initialization

            //const auto nativeTextureHandle = nativeTextureObject.Get("Handle").As<Napi::Number>().Uint32Value();

        }
    };
}

namespace Babylon::Plugins::NativeCamera
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::NativeCamera::Initialize(env);
    }
}
