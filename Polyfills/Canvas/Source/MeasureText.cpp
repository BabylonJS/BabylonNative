#include <bgfx/bgfx.h>
#include <cmath>
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
        // Ink extents (not the full line box): nvgTextBounds replaces ymin/ymax with
        // fonsLineBounds, which would make height/actualBoundingBox* the em line box.
        float bounds[4] = {0, 0, 0, 0};
        const float advance = nvgTextBoundsInk(context->GetNVGContext(), 0, 0, text.c_str(), nullptr, bounds);
        float textMetrics[3] = {0, 0, 0};
        nvgTextMetrics(context->GetNVGContext(), &textMetrics[0], &textMetrics[1], &textMetrics[2]);

        // CSS TextMetrics distances are signed when all ink lies on the opposite side
        // of the alignment baseline.
        const float inkAscent = -bounds[1];
        const float inkDescent = bounds[3];
        // Keep the browser-compatible integral CSS line box used by Babylon GUI.
        // The raw font metrics are fractional em-scaled values; Canvas layout rounds
        // the baseline and total line height independently.
        const float fontAscent = std::floor(textMetrics[0]);
        const float fontDescent = std::ceil(textMetrics[2]) - fontAscent;

        auto obj{Napi::Object::New(env)};
        obj.Set("width", Napi::Value::From(env, advance));
        obj.Set("height", Napi::Value::From(env, bounds[3] - bounds[1]));
        // actualBoundingBoxLeft is positive left of the alignment point.
        obj.Set("actualBoundingBoxLeft", Napi::Value::From(env, -bounds[0]));
        obj.Set("actualBoundingBoxRight", Napi::Value::From(env, bounds[2]));
        obj.Set("actualBoundingBoxAscent", Napi::Value::From(env, inkAscent));
        obj.Set("actualBoundingBoxDescent", Napi::Value::From(env, inkDescent));
        obj.Set("fontBoundingBoxAscent", Napi::Value::From(env, fontAscent));
        obj.Set("fontBoundingBoxDescent", Napi::Value::From(env, fontDescent));

        return obj.As<Napi::Value>();
    }
}
