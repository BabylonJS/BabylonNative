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
    static const int GRADIENT_SAMPLES_R = 256;

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

    void gradientSpan(uint32_t* dst, NVGcolor color0, NVGcolor color1, float offset0, float offset1)
    {
        float s0o = clampf(offset0, 0.0f, 1.0f);
        float s1o = clampf(offset1, 0.0f, 1.0f);
        unsigned s = static_cast<unsigned>(s0o * static_cast<float>(GRADIENT_SAMPLES_L));
        unsigned e = static_cast<unsigned>(s1o * static_cast<float>(GRADIENT_SAMPLES_L));
        float r = color0.rgba[0];
        float g = color0.rgba[1];
        float b = color0.rgba[2];
        float a = color0.rgba[3];
        float dr = (color1.rgba[0] - r) / (e - s);
        float dg = (color1.rgba[1] - g) / (e - s);
        float db = (color1.rgba[2] - b) / (e - s);
        float da = (color1.rgba[3] - a) / (e - s);
        for (unsigned i = s; i < e; i++)
        {
            unsigned ur = (unsigned)(r * 255); unsigned ug = (unsigned)(g * 255); unsigned ub = (unsigned)(b * 255); unsigned ua = (unsigned)(a * 255);
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

    Napi::Object CanvasGradient::CreateLinear(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float x1, float y1)
    {
        Napi::HandleScope scope{ env };

        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CANVAS_GRADIENT_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto gradientValue = func.New({ Napi::Value::From(env, x0), Napi::Value::From(env, y0), Napi::Value::From(env, x1), Napi::Value::From(env, y1) });
        CanvasGradient::Unwrap(gradientValue)->context = context;
        return gradientValue;
    }

    Napi::Object CanvasGradient::CreateRadial(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float r0, float x1, float y1, float r1)
    {
        Napi::HandleScope scope{ env };

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
            if (context.lock())
            {
                nvgDeleteImage(*context.lock(), cachedImage);
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

    int CanvasGradient::LinearGradientStops(LVGColorTransform* x)
    {
        size_t nstops = colors.size();
        if (!nstops)
        {
            return 0;
        }
        uint32_t data[GRADIENT_SAMPLES_L];
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
        return nvgCreateImageRGBA(*context.lock(), GRADIENT_SAMPLES_L, 1, 0, (unsigned char*)data);
    }

    NVGcolor lerpColor(NVGcolor color0, NVGcolor color1, float offset0, float offset1, float g)
    {
        NVGcolor dst;
        float den = std::max(0.00001f, offset1 - offset0);
        for (int i = 0; i < 4; i++)
            dst.rgba[i] = color0.rgba[i] + (color1.rgba[i] - color0.rgba[i]) * (g - offset0) / den;
        dst = nvgRGBAf(std::max(0.0f, std::min(dst.r, 1.0f)), std::max(0.0f, std::min(dst.g, 1.0f)), std::max(0.0f, std::min(dst.b, 1.0f)), std::max(0.0f, std::min(dst.a, 1.0f)));
        return dst;
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

    int CanvasGradient::RadialGradientStops(LVGColorTransform* cxform)
    {
        const int width = GRADIENT_SAMPLES_R, height = GRADIENT_SAMPLES_R;
        uint32_t* image = (unsigned int*)malloc(width * height * sizeof(uint32_t));
        static const int SPREAD_PAD = 0;
        static const int SPREAD_REPEAT = 1;
        static const int SPREAD_REFLECT = 2;

        size_t nstops = colors.size();
        int stopIndex{};
        std::vector<ColorStop> colorStops(nstops);
        for (auto& stop : colors)
        {
            colorStops[stopIndex++] = { stop.first, stop.second };
        }
        int spreadMode = 0;

        float fxn = width / 2;
        float fyn = height / 2;
        float fxp = 0;
        float fyp = 0;
        float rn = width / 2 - 1.0001f;
        float denominator = (rn * rn) - (fxp * fxp + fyp * fyp);

        for (int x = 0; x < width; x++)
        {
            float dx = x - fxn;
            for (int y = 0; y < height; y++)
            {
                float dy = y - fyn;

                float numerator = (dx * fxp + dy * fyp);
                float df = dx * fyp - dy * fxp;
                numerator += std::sqrt((rn * rn) * (dx * dx + dy * dy) - (df * df));
                float g = numerator / denominator;

                // color = c0 + (c1 - c0)(g - x0)/(x1 - x0)
                // where c0 = stop color 0, c1 = stop color 1
                // where x0 = stop offset 0, x1 = stop offset 1
                NVGcolor finalcolor;
                float stop0, stop1;
                NVGcolor color0, color1;

                if (spreadMode == SPREAD_PAD)
                {
                    if (g < 0.0f)
                    {
                        finalcolor = transformColor(colorStops[0].color, cxform);
                    }
                    else if (g > 1.0f)
                    {
                        finalcolor = transformColor(colorStops[nstops - 1].color, cxform);
                    }
                    else
                    {
                        calcStops(colorStops, cxform, &color0, &color1, &stop0, &stop1, g);
                        finalcolor = lerpColor(color0, color1, stop0, stop1, g);
                    }
                }
                else
                {
                    int w = (int)std::fabs(g);
                    if (spreadMode == SPREAD_REPEAT)
                    {
                        if (g < 0)
                        {
                            g = 1 - (std::fabs(g) - w);
                        }
                        else
                        {
                            g = g - w;
                        }
                    }
                    else if (spreadMode == SPREAD_REFLECT)
                    {
                        if (g < 0)
                        {
                            if (w % 2 == 0)
                            {   // even
                                g = (std::fabs(g) - w);
                            }
                            else
                            {   // odd
                                g = (1 - (std::fabs(g) - w));
                            }
                        }
                        else
                        {
                            if (w % 2 == 0)
                            {   // even
                                g = g - w;
                            }
                            else
                            {   // odd
                                g = 1 - (g - w);
                            }
                        }
                    }
                    // clamp
                    if (g > 1)
                        g = 1;
                    if (g < 0)
                        g = 0;
                    calcStops(colorStops, cxform, &color0, &color1, &stop0, &stop1, g);
                    finalcolor = lerpColor(color0, color1, stop0, stop1, g);
                }
                uint32_t color = ((uint32_t)(finalcolor.a * 255) << 24) | ((uint32_t)(finalcolor.b * 255) << 16) |
                    ((uint32_t)(finalcolor.g * 255) << 8) | (uint32_t)(finalcolor.r * 255);
                image[(y * width) + x] = color;
            }
        }
        int img = nvgCreateImageRGBA(*context.lock(), width, height, 0, (unsigned char*)image);
        free(image);
        return img;
    }

    void CanvasGradient::UpdateCache()
    {
        if (!dirty)
        {
            return;
        }
        if (cachedImage >= 0)
        {
            nvgDeleteImage(*context.lock(), cachedImage);
        }
        cachedImage = gradientType == GradientType::Linear ? LinearGradientStops(nullptr) : RadialGradientStops(nullptr);
        dirty = false;
    }
}
