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
                {
                    InstanceMethod("preventDefault", &PointerEvent::PreventDefault),
                    InstanceMethod("stopPropagation", &PointerEvent::StopPropagation),
                    InstanceMethod("stopImmediatePropagation", &PointerEvent::StopPropagation),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
        }

        // Follows the DOM PointerEvent(type, eventInitDict) constructor. This class is installed
        // globally, so anything that synthesizes pointer input with `new PointerEvent(...)` relies
        // on type, coordinates and buttons being carried over, not just the pointer id and type.
        PointerEvent(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<PointerEvent>{info}
        {
            auto env = info.Env();
            auto thisObject = info.This().As<Napi::Object>();

            thisObject.Set("pointerId", Napi::Number::New(env, 0));
            thisObject.Set("pointerType", Napi::String::New(env, ""));
            thisObject.Set("button", Napi::Number::New(env, 0));
            thisObject.Set("buttons", Napi::Number::New(env, 0));
            thisObject.Set("clientX", Napi::Number::New(env, 0));
            thisObject.Set("clientY", Napi::Number::New(env, 0));
            thisObject.Set("movementX", Napi::Number::New(env, 0));
            thisObject.Set("movementY", Napi::Number::New(env, 0));
            thisObject.Set("target", env.Null());
            thisObject.Set("defaultPrevented", Napi::Boolean::New(env, false));

            if (info.Length() > 1 && info[1].IsObject())
            {
                // Object.assign rather than Napi::Object::GetPropertyNames, which isn't
                // implemented consistently across the JsRuntimeHost engines.
                const auto objectCtor = env.Global().Get("Object").As<Napi::Object>();
                objectCtor.Get("assign").As<Napi::Function>().Call(objectCtor, {thisObject, info[1]});
            }

            // Set after copying the init dictionary so a "type" key in it cannot override the event name.
            thisObject.Set("type", info.Length() > 0 && info[0].IsString() ? info[0] : Napi::String::New(env, ""));
        }

    private:
        void PreventDefault(const Napi::CallbackInfo& info)
        {
            info.This().As<Napi::Object>().Set("defaultPrevented", Napi::Boolean::New(info.Env(), true));
        }

        void StopPropagation(const Napi::CallbackInfo&)
        {
        }
    };
} // Babylon
