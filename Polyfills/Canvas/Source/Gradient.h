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

        // Builds the nanovg paint that maps the baked color ramp onto this gradient's own
        // geometry. Callers must not derive the pattern from the shape being filled: per the
        // Canvas2D spec a gradient is positioned by the coordinates given to
        // createLinear/RadialGradient, in user space, independently of what it fills.
        NVGpaint Paint();
        void Dispose();

    protected:
        float x0, y0, x1, y1;
        float r0, r1;
        // User-space box the radial ramp image is baked over and mapped back onto.
        float imageX{}, imageY{}, imageW{1e-4f}, imageH{1e-4f};
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
        // Both take the context by reference rather than re-locking `context` themselves: the
        // caller owns the lock for the whole bake, so it cannot expire midway through.
        int LinearGradientStops(NVGcontext& nvg, LVGColorTransform* x);
        int RadialGradientStops(NVGcontext& nvg, LVGColorTransform* cxform);
    };
}