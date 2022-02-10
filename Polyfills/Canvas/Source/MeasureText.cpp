#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Context.h"
#include "MeasureText.h"
// disable anonymous struct warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "nanovg/nanovg.h"
#pragma GCC diagnostic pop

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CONSTRUCTOR_NAME = "TextDimension";

    Napi::Value MeasureText::CreateInstance(Napi::Env env, Context* context, const std::string& text)
    {
        Napi::HandleScope scope{ env };
        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("width", &MeasureText::GetWidth, nullptr),
                InstanceAccessor("height", &MeasureText::GetHeight, nullptr),
                InstanceAccessor("actualBoundingBoxLeft", &MeasureText::ActualBoundingBoxLeft, nullptr),
                InstanceAccessor("actualBoundingBoxRight", &MeasureText::ActualBoundingBoxRight, nullptr)
            });
        return func.New({Napi::External<Context>::New(env, context), Napi::String::New(env, text)});
    }

    MeasureText::MeasureText(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<MeasureText>{info}
    {
        auto context{info[0].As<Napi::External<Context>>().Data()};
        auto text{info[1].As<Napi::String>().Utf8Value()};
        
        nvgTextBounds(context->GetNVGContext(), 0, 0, text.c_str(), nullptr, m_bounds);
    }

    Napi::Value MeasureText::ActualBoundingBoxLeft(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_bounds[0]);
    }

    Napi::Value MeasureText::ActualBoundingBoxRight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_bounds[2]);
    }

    Napi::Value MeasureText::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_bounds[2] - m_bounds[0]);
    }

    Napi::Value MeasureText::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_bounds[3] - m_bounds[1]);
    }
}

