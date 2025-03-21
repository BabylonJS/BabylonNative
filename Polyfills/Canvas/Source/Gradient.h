#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <map>
#include "nanovg/nanovg.h"

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    struct LVGColorTransform;
    class CanvasGradient final : public Napi::ObjectWrap<CanvasGradient>
    {
    public:
        static void Initialize(Napi::Env);
        static Napi::Object CreateLinear(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float x1, float y1);
        static Napi::Object CreateRadial(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float r0, float x1, float y1, float r1);

        explicit CanvasGradient(const Napi::CallbackInfo& info);
        virtual ~CanvasGradient();

        void UpdateCache();
        int CachedImage() const { return cachedImage; }
        void Dispose();

    protected:
        float x0, y0, x1, y1;
        float r0, r1;
        std::map<float, NVGcolor> colors;
        int cachedImage{-1};
        std::weak_ptr< NVGcontext*> context;
        bool dirty{};
        enum class GradientType
        {
            Linear,
            Radial
        };
        GradientType gradientType;
        void AddColorStop(const Napi::CallbackInfo& info);
        int LinearGradientStops(LVGColorTransform* x);
        int RadialGradientStops(LVGColorTransform* cxform);
    };
}