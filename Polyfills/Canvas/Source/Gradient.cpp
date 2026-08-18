#include <bgfx/bgfx.h>
#include "Canvas.h"
#include "Context.h"
#include "Gradient.h"
#include "Colors.h"

#include <algorithm>
#include <cmath>
#include <map>

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
    static const int GRADIENT_SAMPLES_L = 256;
    // The radial field is baked over the bounding box of both circles, which can be much larger
    // than the shape being filled, so it needs more samples than the 1D linear ramp to stay
    // smooth after nanovg's bilinear stretch.
    static const int GRADIENT_SAMPLES_R = 512;

    typedef struct LVGColorTransform
    {
        float mul[4];
        float add[4];
    } LVGColorTransform;

    struct ColorStop
    {
        float offset;
        NVGcolor color;
    };

    float clampf(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }

    NVGcolor premultiply(NVGcolor c)
    {
        return nvgRGBAf(c.r * c.a, c.g * c.a, c.b * c.a, c.a);
    }

    NVGcolor unpremultiply(NVGcolor c)
    {
        if (c.a <= 0.0f)
        {
            return nvgRGBAf(0.f, 0.f, 0.f, 0.f);
        }
        const float inv = 1.0f / c.a;
        return nvgRGBAf(clampf(c.r * inv, 0.f, 1.f), clampf(c.g * inv, 0.f, 1.f), clampf(c.b * inv, 0.f, 1.f), clampf(c.a, 0.f, 1.f));
    }

    void gradientSpan(uint32_t* dst, NVGcolor color0, NVGcolor color1, float offset0, float offset1)
    {
        float s0o = clampf(offset0, 0.0f, 1.0f);
        float s1o = clampf(offset1, 0.0f, 1.0f);
        unsigned s = static_cast<unsigned>(s0o * static_cast<float>(GRADIENT_SAMPLES_L));
        unsigned e = static_cast<unsigned>(s1o * static_cast<float>(GRADIENT_SAMPLES_L));
        // Coincident stops produce an empty span; the per-sample deltas below would divide
        // by zero and write NaNs into the ramp.
        if (e <= s)
        {
            return;
        }
        // Canvas2D interpolates gradient stops in premultiplied sRGBA space. Interpolating the
        // straight (unpremultiplied) components instead bleeds a transparent stop's RGB into the
        // ramp: "#ff0000ff" -> "#00000000" darkens through maroon to black rather than staying red
        // and only losing alpha.
        const NVGcolor p0 = premultiply(color0);
        const NVGcolor p1 = premultiply(color1);
        float r = p0.rgba[0];
        float g = p0.rgba[1];
        float b = p0.rgba[2];
        float a = p0.rgba[3];
        float dr = (p1.rgba[0] - r) / (e - s);
        float dg = (p1.rgba[1] - g) / (e - s);
        float db = (p1.rgba[2] - b) / (e - s);
        float da = (p1.rgba[3] - a) / (e - s);
        for (unsigned i = s; i < e; i++)
        {
            // The baked image is sampled as straight alpha, so undo the premultiplication here.
            const NVGcolor straight = unpremultiply(nvgRGBAf(r, g, b, a));
            unsigned ur = (unsigned)(straight.r * 255); unsigned ug = (unsigned)(straight.g * 255); unsigned ub = (unsigned)(straight.b * 255); unsigned ua = (unsigned)(straight.a * 255);
            dst[i] = (ua << 24) | (ub << 16) | (ug << 8) | ur;
            r += dr; g += dg; b += db; a += da;
        }
    }

    NVGcolor transformColor(NVGcolor color, LVGColorTransform* x)
    {
        if (!x)
            return color;
        color = nvgRGBAf(color.r * x->mul[0], color.g * x->mul[1], color.b * x->mul[2], color.a * x->mul[3]);
        color = nvgRGBAf(color.r + x->add[0], color.g + x->add[1], color.b + x->add[2], color.a + x->add[3]);
        color = nvgRGBAf(std::max(0.0f, std::min(color.r, 1.0f)), std::max(0.0f, std::min(color.g, 1.0f)), std::max(0.0f, std::min(color.b, 1.0f)), std::max(0.0f, std::min(color.a, 1.0f)));
        return color;
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
        if (!value.IsObject())
        {
            return false;
        }

        const auto constructor = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CANVAS_GRADIENT_CONSTRUCTOR_NAME);
        return constructor.IsFunction() && value.As<Napi::Object>().InstanceOf(constructor.As<Napi::Function>());
    }

    Napi::Object CanvasGradient::CreateLinear(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float x1, float y1)
    {
        // NOTE: Do not open a Napi::HandleScope here. The gradient object created below is
        // returned to the caller, and a plain (non-escapable) HandleScope would release the
        // handle on close. Under the reference-counted QuickJS Node-API port that frees the
        // object's only reference, yielding a dangling value (typeof "unknown", no prototype).
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CANVAS_GRADIENT_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto gradientValue = func.New({ Napi::Value::From(env, x0), Napi::Value::From(env, y0), Napi::Value::From(env, x1), Napi::Value::From(env, y1) });
        CanvasGradient::Unwrap(gradientValue)->context = context;
        return gradientValue;
    }

    Napi::Object CanvasGradient::CreateRadial(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float r0, float x1, float y1, float r1)
    {
        // See CreateLinear: no HandleScope here so the returned gradient handle is not freed on scope close.
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
        gradientType = (info.Length() == 4) ? GradientType::Linear : GradientType::Radial;
        if (gradientType == GradientType::Radial)
        {
            r0 = info[4].As<Napi::Number>().FloatValue();
            r1 = info[5].As<Napi::Number>().FloatValue();
        }
    }

    CanvasGradient::~CanvasGradient()
    {
        Dispose();
    }

    void CanvasGradient::Dispose()
    {
        if (cachedImage >= 0)
        {
            // Lock once and keep the result: calling context.lock() a second time could observe
            // an expired context and dereference an empty shared_ptr.
            if (auto nvg = context.lock(); nvg && *nvg != nullptr)
            {
                nvgDeleteImage(*nvg, cachedImage);
            }
            cachedImage = -1;
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

    int CanvasGradient::LinearGradientStops(NVGcontext& nvg, LVGColorTransform* x)
    {
        size_t nstops = colors.size();
        if (!nstops)
        {
            return 0;
        }
        // Zero-initialize: the spans below only cover the range the stops span, so any
        // sample left untouched would otherwise be read from uninitialized stack memory.
        uint32_t data[GRADIENT_SAMPLES_L]{};
        int stopIndex{};
        std::vector<ColorStop> colorStops(nstops);
        for (auto& stop : colors)
        {
            colorStops[stopIndex++] = { stop.first, stop.second };
        }
        if (colorStops[0].offset > 0.0f)
        {
            NVGcolor s0 = transformColor(colorStops[0].color, x);
            gradientSpan(data, s0, s0, 0.0f, colorStops[0].offset);
        }
        for (unsigned i = 0; i < (nstops - 1); i++)
        {
            gradientSpan(data, transformColor(colorStops[i].color, x),
                transformColor(colorStops[i + 1].color, x),
                colorStops[i].offset,
                colorStops[i + 1].offset);
        }
        if (colorStops[nstops - 1].offset < 1.0f)
        {
            NVGcolor s0 = transformColor(colorStops[nstops - 1].color, x);
            gradientSpan(data, s0, s0, colorStops[nstops - 1].offset, 1.0f);
        }
        return nvgCreateImageRGBA(&nvg, GRADIENT_SAMPLES_L, 1, 0, (unsigned char*)data);
    }

    NVGcolor lerpColor(NVGcolor color0, NVGcolor color1, float offset0, float offset1, float g)
    {
        // See gradientSpan: stops are interpolated premultiplied, then converted back to the
        // straight alpha the baked image is sampled with.
        const NVGcolor p0 = premultiply(color0);
        const NVGcolor p1 = premultiply(color1);
        NVGcolor dst;
        float den = std::max(0.00001f, offset1 - offset0);
        for (int i = 0; i < 4; i++)
            dst.rgba[i] = p0.rgba[i] + (p1.rgba[i] - p0.rgba[i]) * (g - offset0) / den;
        return unpremultiply(dst);
    }

    void calcStops(const std::vector<ColorStop>& gradient, LVGColorTransform* x, NVGcolor* color0, NVGcolor* color1, float* stop0, float* stop1, float g)
    {
        const float* s0{};
        const float* s1{};
        for (size_t i = 0; i < gradient.size() && !s1; i++)
        {
            const float* curr = &gradient[i].offset;
            if (g >= curr[0])
            {
                s0 = curr;
                *color0 = transformColor(gradient[i].color, x);
            }
            else if (s0 && g <= curr[0])
            {
                s1 = curr;
                *color1 = transformColor(gradient[i].color, x);
            }
        }
        if (!s0)
        {
            s0 = &gradient[0].offset;
            *color0 = transformColor(gradient[0].color, x);
        }
        if (!s1)
        {
            s1 = &gradient[gradient.size() - 1].offset;
            *color1 = transformColor(gradient[gradient.size() - 1].color, x);
        }
        *stop0 = s0[0];
        *stop1 = s1[0];
    }

    // Solves the Canvas2D radial gradient equation at a point: finds the largest offset w for
    // which the point lies on the circle interpolated between (x0,y0,r0) and (x1,y1,r1), i.e.
    // |p - (c0 + w*cd)| == r0 + w*dr with a non-negative radius. Returns false when no such
    // circle exists, in which case the spec leaves the point unpainted.
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

        // 'a' is a difference of squared lengths, so it has to be tested relative to their
        // magnitude: an absolute epsilon would miss the degenerate case entirely once the
        // coordinates are in the hundreds.
        const float scale = cdx * cdx + cdy * cdy + dr * dr;
        if (std::fabs(a) <= 1e-6f * std::max(scale, 1.0f))
        {
            // The two circles are tangent, so the quadratic collapses to a linear equation.
            // This is not an exotic edge case: createRadialGradient(100,100,150, 250,100,300)
            // lands here.
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
        // The spec paints the circles in increasing offset order, so the largest solution with
        // a non-negative radius is the one visible at this point.
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

    int CanvasGradient::RadialGradientStops(NVGcontext& nvg, LVGColorTransform* cxform)
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

        // A radial gradient is defined by two independently positioned circles, so the field
        // cannot be baked as a centered disc: bake it over the bounding box that encloses both
        // circles and evaluate the real two-circle equation per texel. Every point on that box's
        // border lies outside both circles, so the ramp has already padded out to an end stop
        // there and nanovg's clamp-to-edge sampling extends that same color across the rest of
        // whatever shape is being filled -- exactly the "pad" behavior the spec requires.
        imageX = std::min(x0 - r0, x1 - r1);
        imageY = std::min(y0 - r0, y1 - r1);
        imageW = std::max(std::max(x0 + r0, x1 + r1) - imageX, 1e-4f);
        imageH = std::max(std::max(y0 + r0, y1 + r1) - imageY, 1e-4f);

        const int width = GRADIENT_SAMPLES_R;
        const int height = GRADIENT_SAMPLES_R;
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
                    finalcolor = transformColor(colorStops[0].color, cxform);
                }
                else if (g >= 1.0f)
                {
                    finalcolor = transformColor(colorStops[nstops - 1].color, cxform);
                }
                else
                {
                    float stop0, stop1;
                    NVGcolor color0, color1;
                    calcStops(colorStops, cxform, &color0, &color1, &stop0, &stop1, g);
                    finalcolor = lerpColor(color0, color1, stop0, stop1, g);
                }

                image[(static_cast<size_t>(y) * static_cast<size_t>(width)) + static_cast<size_t>(x)] =
                    ((uint32_t)(finalcolor.a * 255) << 24) | ((uint32_t)(finalcolor.b * 255) << 16) |
                    ((uint32_t)(finalcolor.g * 255) << 8) | (uint32_t)(finalcolor.r * 255);
            }
        }

        return nvgCreateImageRGBA(&nvg, width, height, 0, (unsigned char*)image.data());
    }

    NVGpaint CanvasGradient::Paint()
    {
        UpdateCache();

        auto nvg = context.lock();
        if (!nvg || *nvg == nullptr || cachedImage < 0)
        {
            // The owning context is gone, or the ramp could not be baked; there is nothing
            // sensible to paint with.
            return NVGpaint{};
        }

        if (gradientType == GradientType::Linear)
        {
            // The linear ramp is baked into a GRADIENT_SAMPLES_L x 1 image, so orient the
            // pattern along (x0,y0)->(x1,y1) and let it span exactly that distance. The
            // vertical extent is irrelevant (the image is one texel tall and clamps), but it
            // must be non-zero, and matching it to the length keeps u and v on the same scale
            // -- nanovg divides the fragment position by the extent, so a small fixed height
            // against a long gradient would cost float precision. Sampling outside the extent
            // clamps to the edge texel, which is precisely the "pad" behavior the spec
            // requires beyond the end stops.
            const float dx = x1 - x0;
            const float dy = y1 - y0;
            float length = std::sqrt(dx * dx + dy * dy);
            if (length < 1e-4f)
            {
                // A zero-length gradient would divide by zero in the shader; keep it finite so
                // the whole shape clamps to a single stop instead of rendering garbage.
                length = 1e-4f;
            }
            return nvgImagePattern(*nvg, x0, y0, length, length, std::atan2(dy, dx), cachedImage, 1.f);
        }

        // The radial ramp is baked over the bounding box of both circles (see
        // RadialGradientStops), so map the image straight back onto that box in user space.
        return nvgImagePattern(*nvg, imageX, imageY, std::max(imageW, 1e-4f), std::max(imageH, 1e-4f), 0.f, cachedImage, 1.f);
    }

    void CanvasGradient::UpdateCache()
    {
        if (!dirty)
        {
            return;
        }

        // Lock the context once and hold it for the whole update. Baking a ramp deletes the old
        // nanovg image and creates a new one, so re-locking inside each step would let the
        // context expire between them and leave a dangling dereference behind.
        auto nvg = context.lock();
        if (!nvg || *nvg == nullptr)
        {
            // The owning context is gone. Stay dirty and leave cachedImage alone: Dispose()
            // already skips deleting images whose context has expired, and Paint() returns an
            // empty paint in this state.
            return;
        }

        if (cachedImage >= 0)
        {
            nvgDeleteImage(*nvg, cachedImage);
            cachedImage = -1;
        }
        cachedImage = gradientType == GradientType::Linear ? LinearGradientStops(**nvg, nullptr) : RadialGradientStops(**nvg, nullptr);
        dirty = false;
    }
}
