#pragma once

#include <Babylon/Polyfills/Canvas.h>

namespace Babylon::Polyfills::Internal
{
    class MeasureText final : public Napi::ObjectWrap<MeasureText>
    {
    public:
        static Napi::Value CreateInstance(Napi::Env env, Context* context, const std::string& text);

        explicit MeasureText(const Napi::CallbackInfo& info);
    private:
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        Napi::Value ActualBoundingBoxLeft(const Napi::CallbackInfo&);
        Napi::Value ActualBoundingBoxRight(const Napi::CallbackInfo&);

        float m_bounds[4];
    };
}