#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Context.h"
#include "MeasureText.h"

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
}
