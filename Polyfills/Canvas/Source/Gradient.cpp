#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Context.h"
#include "Gradient.h"
#include "Colors.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "nanovg/nanovg.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CANVAS_GRADIENT_CONSTRUCTOR_NAME = "CanvasGradient";


    void CanvasGradient::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{ env };

        Napi::Function func = DefineClass(
            env,
            JS_CANVAS_GRADIENT_CONSTRUCTOR_NAME,
            {
                InstanceMethod("addColorStop", &CanvasGradient::AddColorStop),
                
            });
        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CANVAS_GRADIENT_CONSTRUCTOR_NAME, func);
    }

    Napi::Value CanvasGradient::CreateInstance(Napi::Env env, float x0, float y0, float x1, float y1)
    {
        Napi::HandleScope scope{ env };

        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CANVAS_GRADIENT_CONSTRUCTOR_NAME).As<Napi::Function>();
        return func.New({Napi::Value::From(env, x0), Napi::Value::From(env, y0), Napi::Value::From(env, x1), Napi::Value::From(env, y1) });
    }

    CanvasGradient::CanvasGradient(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<CanvasGradient>{ info }
        , x0{ info[0].As<Napi::Number>().FloatValue() }
        , y0{ info[1].As<Napi::Number>().FloatValue() }
        , x1{ info[2].As<Napi::Number>().FloatValue() }
        , y1{ info[3].As<Napi::Number>().FloatValue() }
    {
    }

    CanvasGradient::~CanvasGradient()
    {
    }

    void CanvasGradient::AddColorStop(const Napi::CallbackInfo& info)
    {
        const auto offset = info[0].As<Napi::Number>().FloatValue();

        std::string colorString{ info[1].As<Napi::String>() };
        const auto color = StringToColor(info.Env(), colorString);
        colors.insert(std::make_pair(offset, color));
    }

    void CanvasGradient::UpdateCache()
    {
        // compute the gradient image if the gradient is dirty
    }
}
