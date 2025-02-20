#pragma once

#include <Babylon/Polyfills/Canvas.h>

namespace Babylon::Polyfills::Internal
{
    class Path2D final : public Napi::ObjectWrap<Path2D>
    {
    public:
        static Napi::Value CreateInstance(Napi::Env env, Napi::String path);
        // static Napi::Value CreateInstance(Napi::Env env, Napi::String path); // TODO: Path2D arg

        explicit Path2D(const Napi::CallbackInfo& info);

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
