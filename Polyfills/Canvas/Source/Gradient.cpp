#include <bgfx/bgfx.h>
#include "Canvas.h"
#include "Context.h"
#include "Gradient.h"
#include "Colors.h"
#include "NativeInstanceRegistry.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <vector>

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
    namespace
    {
        constexpr int LINEAR_GRADIENT_SAMPLE_COUNT = 256;
        // Radial ramps need more samples because their baked area can be much larger.
        constexpr int RADIAL_GRADIENT_SAMPLE_COUNT = 512;
        constexpr float MIN_STOP_DISTANCE = 0.00001f;
        constexpr size_t LINEAR_GRADIENT_ARGUMENT_COUNT = 4;

        uint32_t PackColor(NVGcolor color)
        {
            const auto channel = [](float value) {
                return static_cast<uint32_t>(value * CanvasConstants::COLOR_CHANNEL_MAX);
            };
            return (channel(color.a) << CanvasConstants::ALPHA_CHANNEL_SHIFT) |
                (channel(color.b) << CanvasConstants::BLUE_CHANNEL_SHIFT) |
                (channel(color.g) << CanvasConstants::GREEN_CHANNEL_SHIFT) |
                channel(color.r);
        }
    }

    struct ColorStop
    {
        float offset;
        NVGcolor color;
    };

    float clampf(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }

    void gradientSpan(uint32_t* dst, NVGcolor color0, NVGcolor color1, float offset0, float offset1)
    {
        float s0o = clampf(offset0, 0.0f, 1.0f);
        float s1o = clampf(offset1, 0.0f, 1.0f);
        unsigned s = static_cast<unsigned>(s0o * static_cast<float>(LINEAR_GRADIENT_SAMPLE_COUNT));
        unsigned e = static_cast<unsigned>(s1o * static_cast<float>(LINEAR_GRADIENT_SAMPLE_COUNT));
        // Coincident stops produce an empty span.
        if (e <= s)
        {
            return;
        }
        // Canvas interpolates straight RGBA; NanoVG premultiplies during rendering.
        float r = color0.r;
        float g = color0.g;
        float b = color0.b;
        float a = color0.a;
        float dr = (color1.r - r) / (e - s);
        float dg = (color1.g - g) / (e - s);
        float db = (color1.b - b) / (e - s);
        float da = (color1.a - a) / (e - s);
        for (unsigned i = s; i < e; i++)
        {
            dst[i] = PackColor({r, g, b, a});
            r += dr; g += dg; b += db; a += da;
        }
    }

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

    bool CanvasGradient::IsInstance(Napi::Env env, const Napi::Value& value)
    {
        return TryUnwrap(env, value) != nullptr;
    }

    CanvasGradient* CanvasGradient::TryUnwrap(Napi::Env env, const Napi::Value& value)
    {
        return NativeInstanceRegistry<CanvasGradient>::TryUnwrap(env, value);
    }

    Napi::Object CanvasGradient::CreateLinear(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float x1, float y1)
    {
        // A non-escapable HandleScope would release the returned QuickJS handle.
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CANVAS_GRADIENT_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto gradientValue = func.New({ Napi::Value::From(env, x0), Napi::Value::From(env, y0), Napi::Value::From(env, x1), Napi::Value::From(env, y1) });
        CanvasGradient::Unwrap(gradientValue)->context = context;
        return gradientValue;
    }

    Napi::Object CanvasGradient::CreateRadial(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float r0, float x1, float y1, float r1)
    {
        // See CreateLinear: the returned handle must escape this function.
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CANVAS_GRADIENT_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto gradientValue = func.New({ Napi::Value::From(env, x0), Napi::Value::From(env, y0), Napi::Value::From(env, x1), Napi::Value::From(env, y1), Napi::Value::From(env, r0), Napi::Value::From(env, r1) });
        CanvasGradient::Unwrap(gradientValue)->context = context;
        return gradientValue;
    }

    CanvasGradient::CanvasGradient(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<CanvasGradient>{ info }
        , x0{ info[0].As<Napi::Number>().FloatValue() }
        , y0{ info[1].As<Napi::Number>().FloatValue() }
        , x1{ info[2].As<Napi::Number>().FloatValue() }
        , y1{ info[3].As<Napi::Number>().FloatValue() }
    {
        gradientType = info.Length() == LINEAR_GRADIENT_ARGUMENT_COUNT ? GradientType::Linear : GradientType::Radial;
        if (gradientType == GradientType::Radial)
        {
            r0 = info[4].As<Napi::Number>().FloatValue();
            r1 = info[5].As<Napi::Number>().FloatValue();
        }

        // Register after successful construction only.
        NativeInstanceRegistry<CanvasGradient>::Add(info, this);
    }

    CanvasGradient::~CanvasGradient()
    {
        NativeInstanceRegistry<CanvasGradient>::Remove(this);
        Dispose();
    }

    void CanvasGradient::Dispose()
    {
        if (cachedImage >= CanvasConstants::MIN_NANOVG_RESOURCE_INDEX)
        {
            // Keep one lock result so the context cannot expire between checks.
            if (auto nvg = context.lock(); nvg && *nvg != nullptr)
            {
                nvgDeleteImage(*nvg, cachedImage);
            }
            cachedImage = CanvasConstants::INVALID_NANOVG_IMAGE;
        }
    }

    void CanvasGradient::AddColorStop(const Napi::CallbackInfo& info)
    {
        const auto offset = info[0].As<Napi::Number>().FloatValue();

        std::string colorString{ info[1].As<Napi::String>() };
        const auto color = StringToColor(info.Env(), colorString);
        colors.insert(std::make_pair(offset, color));
        dirty = true;
    }

    int CanvasGradient::LinearGradientStops(NVGcontext& nvg)
    {
        size_t nstops = colors.size();
        if (!nstops)
        {
            return 0;
        }
        // Stops may not cover the entire ramp.
        uint32_t data[LINEAR_GRADIENT_SAMPLE_COUNT]{};
        int stopIndex{};
        std::vector<ColorStop> colorStops(nstops);
        for (auto& stop : colors)
        {
            colorStops[stopIndex++] = { stop.first, stop.second };
        }
        if (colorStops[0].offset > 0.0f)
        {
            const NVGcolor s0 = colorStops[0].color;
            gradientSpan(data, s0, s0, 0.0f, colorStops[0].offset);
        }
        for (unsigned i = 0; i < (nstops - 1); i++)
        {
            gradientSpan(data, colorStops[i].color,
                colorStops[i + 1].color,
                colorStops[i].offset,
                colorStops[i + 1].offset);
        }
        if (colorStops[nstops - 1].offset < 1.0f)
        {
            const NVGcolor s0 = colorStops[nstops - 1].color;
            gradientSpan(data, s0, s0, colorStops[nstops - 1].offset, 1.0f);
        }
        return nvgCreateImageRGBA(&nvg, LINEAR_GRADIENT_SAMPLE_COUNT, 1, 0, (unsigned char*)data);
    }

    NVGcolor lerpColor(NVGcolor color0, NVGcolor color1, float offset0, float offset1, float g)
    {
        NVGcolor dst;
        float den = std::max(MIN_STOP_DISTANCE, offset1 - offset0);
        for (size_t i = 0; i < CanvasConstants::RGBA_CHANNEL_COUNT; i++)
            dst.rgba[i] = color0.rgba[i] + (color1.rgba[i] - color0.rgba[i]) * (g - offset0) / den;
        return dst;
    }

    void calcStops(const std::vector<ColorStop>& gradient, NVGcolor* color0, NVGcolor* color1, float* stop0, float* stop1, float g)
    {
        const float* s0{};
        const float* s1{};
        for (size_t i = 0; i < gradient.size() && !s1; i++)
        {
            const float* curr = &gradient[i].offset;
            if (g >= curr[0])
            {
                s0 = curr;
                *color0 = gradient[i].color;
            }
            else if (s0 && g <= curr[0])
            {
                s1 = curr;
                *color1 = gradient[i].color;
            }
        }
        if (!s0)
        {
            s0 = &gradient[0].offset;
            *color0 = gradient[0].color;
        }
        if (!s1)
        {
            s1 = &gradient[gradient.size() - 1].offset;
            *color1 = gradient[gradient.size() - 1].color;
        }
        *stop0 = s0[0];
        *stop1 = s1[0];
    }

    // Finds the largest valid offset on the interpolated circle; unmatched points are unpainted.
    static bool solveRadialOffset(float px, float py, float x0, float y0, float r0, float x1, float y1, float r1, float& offset)
    {
        const float cdx = x1 - x0;
        const float cdy = y1 - y0;
        const float dr = r1 - r0;
        const float pdx = px - x0;
        const float pdy = py - y0;

        const float a = cdx * cdx + cdy * cdy - dr * dr;
        const float b = pdx * cdx + pdy * cdy + r0 * dr;
        const float c = pdx * pdx + pdy * pdy - r0 * r0;

        // Use a relative epsilon because a is a difference of squared lengths.
        const float scale = cdx * cdx + cdy * cdy + dr * dr;
        if (std::fabs(a) <= GradientConstants::QUADRATIC_RELATIVE_EPSILON * std::max(scale, 1.0f))
        {
            // Tangent circles collapse the quadratic to a linear equation.
            if (b == 0.0f)
            {
                return false;
            }
            offset = c / (2.0f * b);
            return (r0 + offset * dr) >= 0.0f;
        }

        const float discriminant = b * b - a * c;
        if (discriminant < 0.0f)
        {
            return false;
        }
        const float root = std::sqrt(discriminant);
        const float larger = std::max((b + root) / a, (b - root) / a);
        const float smaller = std::min((b + root) / a, (b - root) / a);
        // The visible solution is the largest one with a non-negative radius.
        if ((r0 + larger * dr) >= 0.0f)
        {
            offset = larger;
            return true;
        }
        if ((r0 + smaller * dr) >= 0.0f)
        {
            offset = smaller;
            return true;
        }
        return false;
    }

    int CanvasGradient::RadialGradientStops(NVGcontext& nvg)
    {
        const size_t nstops = colors.size();
        if (!nstops)
        {
            return 0;
        }

        std::vector<ColorStop> colorStops(nstops);
        int stopIndex{};
        for (auto& stop : colors)
        {
            colorStops[stopIndex++] = { stop.first, stop.second };
        }

        // Bake the two-circle field over its bounding box; edge clamping supplies spec padding.
        imageX = std::min(x0 - r0, x1 - r1);
        imageY = std::min(y0 - r0, y1 - r1);
        imageW = std::max(std::max(x0 + r0, x1 + r1) - imageX, GradientConstants::MIN_IMAGE_EXTENT);
        imageH = std::max(std::max(y0 + r0, y1 + r1) - imageY, GradientConstants::MIN_IMAGE_EXTENT);

        const int width = RADIAL_GRADIENT_SAMPLE_COUNT;
        const int height = RADIAL_GRADIENT_SAMPLE_COUNT;
        std::vector<uint32_t> image(static_cast<size_t>(width) * static_cast<size_t>(height));

        for (int y = 0; y < height; y++)
        {
            const float py = imageY + (static_cast<float>(y) + 0.5f) * imageH / static_cast<float>(height);
            for (int x = 0; x < width; x++)
            {
                const float px = imageX + (static_cast<float>(x) + 0.5f) * imageW / static_cast<float>(width);

                NVGcolor finalcolor;
                float g{};
                if (!solveRadialOffset(px, py, x0, y0, r0, x1, y1, r1, g))
                {
                    finalcolor = nvgRGBAf(0.f, 0.f, 0.f, 0.f);
                }
                else if (g <= 0.0f)
                {
                    finalcolor = colorStops[0].color;
                }
                else if (g >= 1.0f)
                {
                    finalcolor = colorStops[nstops - 1].color;
                }
                else
                {
                    float stop0, stop1;
                    NVGcolor color0, color1;
                    calcStops(colorStops, &color0, &color1, &stop0, &stop1, g);
                    finalcolor = lerpColor(color0, color1, stop0, stop1, g);
                }

                image[(static_cast<size_t>(y) * static_cast<size_t>(width)) + static_cast<size_t>(x)] = PackColor(finalcolor);
            }
        }

        return nvgCreateImageRGBA(&nvg, width, height, 0, (unsigned char*)image.data());
    }

    NVGpaint CanvasGradient::Paint()
    {
        UpdateCache();

        auto nvg = context.lock();
        if (!nvg || *nvg == nullptr || cachedImage < CanvasConstants::MIN_NANOVG_RESOURCE_INDEX)
        {
            return NVGpaint{};
        }

        if (gradientType == GradientType::Linear)
        {
            // Orient the 1D ramp along the gradient line; edge clamping provides padding.
            const float dx = x1 - x0;
            const float dy = y1 - y0;
            float length = std::sqrt(dx * dx + dy * dy);
            if (length < GradientConstants::MIN_IMAGE_EXTENT)
            {
                // Avoid shader division by zero for a zero-length gradient.
                length = GradientConstants::MIN_IMAGE_EXTENT;
            }
            return nvgImagePattern(*nvg, x0, y0, length, length, std::atan2(dy, dx), cachedImage, 1.f);
        }

        // Map the radial ramp back onto its user-space bounding box.
        return nvgImagePattern(*nvg, imageX, imageY,
            std::max(imageW, GradientConstants::MIN_IMAGE_EXTENT),
            std::max(imageH, GradientConstants::MIN_IMAGE_EXTENT),
            0.f, cachedImage, 1.f);
    }

    void CanvasGradient::UpdateCache()
    {
        if (!dirty)
        {
            return;
        }

        // Hold one context lock while replacing the cached NanoVG image.
        auto nvg = context.lock();
        if (!nvg || *nvg == nullptr)
        {
            return;
        }

        if (cachedImage >= CanvasConstants::MIN_NANOVG_RESOURCE_INDEX)
        {
            nvgDeleteImage(*nvg, cachedImage);
            cachedImage = CanvasConstants::INVALID_NANOVG_IMAGE;
        }
        cachedImage = gradientType == GradientType::Linear ? LinearGradientStops(**nvg) : RadialGradientStops(**nvg);
        dirty = false;
    }
}
