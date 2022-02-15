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
    Napi::Value MeasureText::CreateInstance(Napi::Env env, Context* context, const std::string& text)
    {
        float bounds[4];
        nvgTextBounds(context->GetNVGContext(), 0, 0, text.c_str(), nullptr, bounds);

        auto obj{Napi::Object::New(env)};
        obj.Set("width", Napi::Value::From(env, bounds[2] - bounds[0]));
        obj.Set("height", Napi::Value::From(env, bounds[3] - bounds[1]));
        obj.Set("actualBoundingBoxLeft", Napi::Value::From(env, bounds[0]));
        obj.Set("actualBoundingBoxRight", Napi::Value::From(env, bounds[2]));

        return obj.As<Napi::Value>();
    }

    Napi::Value MeasureText::GetTextMetrics(Napi::Env env, Context* context)
    {
        float ascender{}, descender{}, lineh{};
        nvgTextMetrics(context->GetNVGContext(), &ascender, &descender, &lineh);

        auto obj{Napi::Object::New(env)};
        obj.Set("ascent", Napi::Value::From(env, ascender));
        obj.Set("height", Napi::Value::From(env, lineh));
        obj.Set("descent", Napi::Value::From(env, descender));
        return obj.As<Napi::Value>();
    }
}

