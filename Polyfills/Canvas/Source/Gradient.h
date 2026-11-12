#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include "CanvasConstants.h"
#include <map>
#include "nanovg/nanovg.h"

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    namespace GradientConstants
    {
        constexpr float MIN_IMAGE_EXTENT = 1e-4f;
        constexpr float QUADRATIC_RELATIVE_EPSILON = 1e-6f;
    }

    class CanvasGradient final : public Napi::ObjectWrap<CanvasGradient>
    {
    public:
        static void Initialize(Napi::Env);
        static Napi::Object CreateLinear(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float x1, float y1);
        static Napi::Object CreateRadial(Napi::Env env, const std::shared_ptr<NVGcontext*>& context, float x0, float y0, float r0, float x1, float y1, float r1);

        // True only for objects this polyfill's own constructor produced. Never use
        // ObjectWrap::Unwrap on an unchecked object; see NativeInstanceRegistry.
        static bool IsInstance(Napi::Env env, const Napi::Value& value);

        // Returns the wrapped gradient, or nullptr when `value` is not one.
        static CanvasGradient* TryUnwrap(Napi::Env env, const Napi::Value& value);

        explicit CanvasGradient(const Napi::CallbackInfo& info);
        virtual ~CanvasGradient();

        void UpdateCache();

        // Gradients retain the user-space geometry passed at creation.
        NVGpaint Paint();
        void Dispose();

    protected:
        float x0, y0, x1, y1;
        float r0, r1;
        float imageX{}, imageY{}, imageW{GradientConstants::MIN_IMAGE_EXTENT}, imageH{GradientConstants::MIN_IMAGE_EXTENT};
        // Equal-offset stops express hard transitions and retain insertion order.
        std::multimap<float, NVGcolor> colors;
        int cachedImage{CanvasConstants::INVALID_NANOVG_IMAGE};
        std::weak_ptr< NVGcontext*> context;
        bool dirty{};
        enum class GradientType
        {
            Linear,
            Radial
        };
        GradientType gradientType;
        void AddColorStop(const Napi::CallbackInfo& info);
        int LinearGradientStops(NVGcontext& nvg);
        int RadialGradientStops(NVGcontext& nvg);
    };
}