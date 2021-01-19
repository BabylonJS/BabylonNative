#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Context.h"
#include "MeasureText.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "nanovg/nanovg.h"
#pragma GCC diagnostic pop

namespace Babylon::Polyfills::Internal
{
    Napi::Value MeasureText::CreateInstance(Napi::Env env, Context* context, const std::string& text)
    {
        Napi::HandleScope scope{ env };
        Napi::Function func = ParentT::DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("width", &MeasureText::GetWidth, &MeasureText::SetWidth),
                InstanceAccessor("height", &MeasureText::GetHeight, &MeasureText::SetHeight),
            });
        return func.New({Napi::External<Context>::New(env, context), Napi::String::New(env, text)});
    }

    MeasureText::MeasureText(const Napi::CallbackInfo& info)
        : ParentT{ info }
    {
        auto context{info[0].As<Napi::External<Context>>().Data()};
        auto text{info[1].As<Napi::String>().Utf8Value()};
        float bounds[4];
        nvgTextBounds(context->GetNVGContext(), 0, 0, text.c_str(), nullptr, bounds);
        m_width = bounds[2] - bounds[0];
        m_height = bounds[3] - bounds[1];
    }

    Napi::Value MeasureText::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void MeasureText::SetWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_width = value.As<Napi::Number>().Uint32Value();
    }

    Napi::Value MeasureText::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void MeasureText::SetHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_height = value.As<Napi::Number>().Uint32Value();
    }
}

