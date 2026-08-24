#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <cstdint>
#include <vector>

namespace Babylon::Plugins::Internal
{
    class ImageDataDawn final : public Napi::ObjectWrap<ImageDataDawn>
    {
    public:
        static Napi::Value CreateInstance(Napi::Env env, ContextDawn* context, int32_t sx, int32_t sy, uint32_t width, uint32_t height);

        explicit ImageDataDawn(const Napi::CallbackInfo& info);

    private:
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        Napi::Value GetData(const Napi::CallbackInfo&);

        uint32_t m_width{};
        uint32_t m_height{};

        // The spec requires `data` to be one live buffer that the caller can
        // mutate in place; handing back a fresh copy per access silently drops
        // every write, which breaks the standard getImageData/putImageData
        // round trip. Hold the array itself so each read returns the same one.
        Napi::Reference<Napi::Uint8Array> m_data;
    };
}