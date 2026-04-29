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
        // IMPORTANT: zero-initialize. nvgTextBounds / nvgTextMetrics do not always fill these
        // slots (e.g. when called before the text atlas has been populated), and uninitialized
        // stack values turn into garbage widths the size of millions of pixels, which Babylon
        // GUI then propagates into a wildly negative fillText x coordinate, putting text off-
        // screen.
        float bounds[4] = {0, 0, 0, 0};
        nvgTextBounds(context->GetNVGContext(), 0, 0, text.c_str(), nullptr, bounds);
        float textMetrics[3] = {0, 0, 0};
        nvgTextMetrics(context->GetNVGContext(), &textMetrics[0], &textMetrics[1], &textMetrics[2]);

        auto obj{Napi::Object::New(env)};
        obj.Set("width", Napi::Value::From(env, bounds[2] - bounds[0]));
        obj.Set("height", Napi::Value::From(env, bounds[3] - bounds[1]));
        obj.Set("actualBoundingBoxLeft", Napi::Value::From(env, bounds[0]));
        obj.Set("actualBoundingBoxRight", Napi::Value::From(env, bounds[2]));
        obj.Set("fontBoundingBoxAscent", Napi::Value::From(env, textMetrics[0]));
        obj.Set("fontBoundingBoxDescent", Napi::Value::From(env, -textMetrics[1]));

        return obj.As<Napi::Value>();
    }
}
