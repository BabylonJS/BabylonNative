#pragma once

namespace Babylon
{
    class PointerEvent : public Napi::ObjectWrap<PointerEvent>
    {
        static constexpr auto JS_CLASS_NAME = "PointerEvent";

    public:
        static void Initialize(Napi::Env& env)
        {
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

        PointerEvent(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<PointerEvent>{info}
        {
            auto thisObject = info.This().As<Napi::Object>();
            Napi::Object params = info[1].As<Napi::Object>();
            thisObject.Set("pointerId", params.Get("pointerId"));
            thisObject.Set("pointerType", params.Get("pointerType"));
        }
    };
} // Babylon
