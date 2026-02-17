#pragma once

namespace Babylon
{
    class XRWebGLLayer : public Napi::ObjectWrap<XRWebGLLayer>
    {
        static constexpr auto JS_CLASS_NAME = "XRWebGLLayer";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("getViewport", &XRWebGLLayer::GetViewport),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
        }

        XRWebGLLayer(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRWebGLLayer>{info}
        {
        }

    private:
        Napi::Value GetViewport(const Napi::CallbackInfo& info)
        {
            return info.This().As<Napi::Object>().Get("viewport");
        }
    };
} // Babylon
