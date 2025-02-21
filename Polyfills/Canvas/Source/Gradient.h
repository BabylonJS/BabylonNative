#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <map>
#include "nanovg.h"

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    class CanvasGradient final : public Napi::ObjectWrap<CanvasGradient>
    {
    public:
        static void Initialize(Napi::Env);
        static Napi::Value CreateInstance(Napi::Env env, float x0, float y0, float x1, float y1);

        explicit CanvasGradient(const Napi::CallbackInfo& info);
        virtual ~CanvasGradient();

        void UpdateCache();

    protected:
        float x0, y0, x1, y1;
        std::map<float, NVGcolor> colors;
        void AddColorStop(const Napi::CallbackInfo& info);
    };
}