#pragma once

#include <Babylon/Polyfills/Canvas.h>

namespace Babylon::Polyfills::Internal
{
    class MeasureText final : public Napi::ObjectWrap<MeasureText>
    {
    public:
        static constexpr auto JS_CONSTRUCTOR_NAME = "TextDimension";

        using ParentT = Napi::ObjectWrap<MeasureText>;

        static Napi::Value CreateInstance(Napi::Env env, Context* context, const std::string& text);

        explicit MeasureText(const Napi::CallbackInfo& info);
    private:
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        void SetWidth(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        void SetHeight(const Napi::CallbackInfo&, const Napi::Value& value);

        int m_width{};
        int m_height{};
    };
}