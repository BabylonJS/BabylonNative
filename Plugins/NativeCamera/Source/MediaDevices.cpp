#include <napi/napi.h>
#include "MediaStream.h"
#include "NativeVideo.h"
#include "CameraDevice.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <vector>
#include <algorithm>

namespace Babylon::Plugins::Internal
{
    // The MediaDevices class is a native implementation that polyfills the MediaDevices web API
    // https://developer.mozilla.org/en-US/docs/Web/API/MediaDevices
    class MediaDevices : public Napi::ObjectWrap<MediaDevices>
    {
    public:
        MediaDevices(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<MediaDevices>{info}
        {
        }

        static Napi::Value GetUserMedia(const Napi::CallbackInfo& info)
        {
            auto env = info.Env();

            auto deferred = Napi::Promise::Deferred::New(env);
            auto promise = deferred.Promise();

            // Extract the video constraints as we only support video for the time being
            auto videoConstraints{Napi::Object::New(env)};
            if (info.Length() > 0 && info[0].IsObject())
            {
                Napi::Object constraints{info[0].As<Napi::Object>()};
                if (constraints.Get("video").IsObject())
                {
                    videoConstraints = constraints.Get("video").As<Napi::Object>();
                }
            }

            auto runtimeScheduler{std::make_unique<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(env))};
            MediaStream::NewAsync(env, videoConstraints).then(*runtimeScheduler, arcana::cancellation::none(), [runtimeScheduler = std::move(runtimeScheduler), env, deferred](const arcana::expected<Napi::Object, std::exception_ptr>& result)
            {
                if (result.has_error())
                {
                    deferred.Reject(Napi::Error::New(env, result.error()).Value());
                    return;
                }

                deferred.Resolve(result.value());
            });

            return std::move(promise);
        }
        
        static Napi::Value EnumerateDevices(const Napi::CallbackInfo& info)
        {
            auto env = info.Env();

            auto deferred = Napi::Promise::Deferred::New(env);
            auto promise = deferred.Promise();

            std::vector<CameraDevice> cameraDevices{CameraDevice::GetCameraDevices(env)};
            
            Napi::Array devices{Napi::Array::New(env, cameraDevices.size())};
            
            for (unsigned long i=0; i<cameraDevices.size(); i++)
            {
                Napi::Object device{Napi::Object::New(env)};
                device.Set("deviceId", Napi::String::New(env, std::to_string(i)));
                device.Set("groupId", Napi::String::New(env, ""));
                device.Set("kind", Napi::String::New(env, "videoinput"));
                device.Set("label", Napi::String::New(env, ""));
                
                devices.Set(static_cast<uint32_t>(i), device);
            }
            
            deferred.Resolve(devices);
            
            return std::move(promise);
        }
    };
}

namespace Babylon::Plugins::MediaDevices
{
    static constexpr auto JS_NAVIGATOR_NAME = "navigator";

    void Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

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
        mediaDevices.Set("getUserMedia", Napi::Function::New(env, &Internal::MediaDevices::GetUserMedia, "getUserMedia"));
        mediaDevices.Set("enumerateDevices", Napi::Function::New(env, &Internal::MediaDevices::EnumerateDevices, "enumerateDevices"));
        navigator.Set("mediaDevices", mediaDevices);
    }
}
