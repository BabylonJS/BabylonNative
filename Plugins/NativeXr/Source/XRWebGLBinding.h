#pragma once

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
                {});

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
    };
} // Babylon
