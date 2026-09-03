#pragma once

#include "XRCamera.h"

namespace Babylon
{
    class XRWebGLBinding : public Napi::ObjectWrap<XRWebGLBinding>
    {
        static constexpr auto JS_CLASS_NAME = "XRWebGLBinding";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("getCameraImage", &XRWebGLBinding::GetCameraImage),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
        }

        XRWebGLBinding(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRWebGLBinding>{info}
        {
        }

    private:
        // WebXR raw camera access: returns the camera image for the given
        // XRCamera as a Napi::Pointer<Graphics::Texture> — the object type NativeEngine
        // uses as a hardware texture's underlyingResource, so Babylon.js can sample it.
        Napi::Value GetCameraImage(const Napi::CallbackInfo& info)
        {
            if (info.Length() < 1 || !info[0].IsObject())
            {
                return info.Env().Undefined();
            }

            XRCamera* camera = XRCamera::Unwrap(info[0].As<Napi::Object>());
            if (camera == nullptr)
            {
                return info.Env().Undefined();
            }

            return camera->GetCameraImageValue(info.Env());
        }
    };
} // Babylon
