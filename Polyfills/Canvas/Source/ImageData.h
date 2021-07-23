#pragma once

#include <Babylon/Polyfills/Canvas.h>

namespace Babylon::Polyfills::Internal
{
    class ImageData final : public Napi::ObjectWrap<ImageData>
    {
    public:
        static Napi::Value CreateInstance(Napi::Env env, Context* context, uint32_t width, uint32_t height);

        explicit ImageData(const Napi::CallbackInfo& info);
    private:
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        Napi::Value GetData(const Napi::CallbackInfo&);

        uint32_t m_width{};
        uint32_t m_height{};
    };
}