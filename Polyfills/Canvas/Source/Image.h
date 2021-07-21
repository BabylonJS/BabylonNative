#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <GraphicsImpl.h>
#include <FrameBuffer.h>

namespace Babylon::Polyfills::Internal
{
    class NativeCanvasImage final : public Napi::ObjectWrap<NativeCanvasImage>
    {
    public:
        static void CreateInstance(Napi::Env env);

        explicit NativeCanvasImage(const Napi::CallbackInfo& info);
        virtual ~NativeCanvasImage();

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

    private:
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        Napi::Value GetNaturalWidth(const Napi::CallbackInfo&);
        Napi::Value GetNaturalHeight(const Napi::CallbackInfo&);
        Napi::Value GetSrc(const Napi::CallbackInfo&);
        void SetSrc(const Napi::CallbackInfo&, const Napi::Value&);
        Napi::Value GetOnload(const Napi::CallbackInfo&);
        void SetOnload(const Napi::CallbackInfo&, const Napi::Value&);

        uint32_t m_width{1};
        uint32_t m_height{1};

        std::string m_src{};
    };
}
