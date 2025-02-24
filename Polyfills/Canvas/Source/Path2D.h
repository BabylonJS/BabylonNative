#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/JsRuntimeScheduler.h>

namespace Babylon::Polyfills::Internal
{
    class NativeCanvasPath2D final : public Napi::ObjectWrap<NativeCanvasPath2D>
    {
    public:
        static void CreateInstance(Napi::Env env);

        explicit NativeCanvasPath2D(const Napi::CallbackInfo& info);
        // virtual ~NativeCanvasPath2D(); // TODO: destructor?

    private:
        void AddPath(const Napi::CallbackInfo&);
        void ClosePath(const Napi::CallbackInfo&);
        void MoveTo(const Napi::CallbackInfo&);
        void LineTo(const Napi::CallbackInfo&);
        void BezierCurveTo(const Napi::CallbackInfo&);
        void QuadraticCurveTo(const Napi::CallbackInfo&);
        void Arc(const Napi::CallbackInfo&);
        void ArcTo(const Napi::CallbackInfo&);
        void Ellipse(const Napi::CallbackInfo&);
        void Rect(const Napi::CallbackInfo&);
        void RoundRect(const Napi::CallbackInfo&);
    };
}
