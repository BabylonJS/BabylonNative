#include <Babylon/Polyfills/DOM.h>

namespace Babylon::Polyfills::DOM
{
    class DOMPointReadOnly final : public Napi::ObjectWrap<DOMPointReadOnly>
    {
        static constexpr auto JS_CLASS_NAME = "DOMPointReadOnly";

    public:
        static void Initialize(Napi::Env env) {
            Napi::HandleScope scope{ env };

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceAccessor("x", &DOMPointReadOnly::GetX, nullptr),
                    InstanceAccessor("y", &DOMPointReadOnly::GetY, nullptr),
                    InstanceAccessor("z", &DOMPointReadOnly::GetZ, nullptr),
                    InstanceAccessor("w", &DOMPointReadOnly::GetW, nullptr)
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        DOMPointReadOnly(const Napi::CallbackInfo& info) : Napi::ObjectWrap<DOMPointReadOnly>{info}
        {
            _x = info[0].As<Napi::Number>();
            _y = info[1].As<Napi::Number>();

            if (info.Length() > 2)
            {
                _z = info[2].As<Napi::Number>();
            }

            if (info.Length() > 3)
            {
                _w = info[3].As<Napi::Number>();
            }
        }

    private:
        float _x{ 0 };
        float _y{ 0 };
        float _z{ 0 };
        float _w{ 1 };

        Napi::Value GetX(const Napi::CallbackInfo& info) {
            return Napi::Number::From(info.Env(), _x);
        }

        Napi::Value GetY(const Napi::CallbackInfo& info) {
            return Napi::Number::From(info.Env(), _y);
        }

        Napi::Value GetZ(const Napi::CallbackInfo& info) {
            return Napi::Number::From(info.Env(), _z);
        }

        Napi::Value GetW(const Napi::CallbackInfo& info) {
            return Napi::Number::From(info.Env(), _w);
        }
    };

    void Initialize(Napi::Env env)
    {
        DOMPointReadOnly::Initialize(env);
    }
}
