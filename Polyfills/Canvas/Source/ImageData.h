#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <cstdint>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    class ImageData final : public Napi::ObjectWrap<ImageData>
    {
    public:
        static Napi::Value CreateInstance(Napi::Env env, Context* context, int32_t sx, int32_t sy, uint32_t width, uint32_t height);

        explicit ImageData(const Napi::CallbackInfo& info);

    private:
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        Napi::Value GetData(const Napi::CallbackInfo&);

        uint32_t m_width{};
        uint32_t m_height{};
        std::vector<uint8_t> m_pixels;
    };
}