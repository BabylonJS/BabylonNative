#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <GraphicsImpl.h>
#include <FrameBuffer.h>
#include <UrlLib/UrlLib.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <bx/allocator.h>
#include <bimg/bimg.h>

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    class NativeCanvasImage final : public Napi::ObjectWrap<NativeCanvasImage>
    {
    public:
        static void CreateInstance(Napi::Env env);

        explicit NativeCanvasImage(const Napi::CallbackInfo& info);
        virtual ~NativeCanvasImage();

        int CreateNVGImageForContext(NVGcontext* nvgContext) const;

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

    private:
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        Napi::Value GetNaturalWidth(const Napi::CallbackInfo&);
        Napi::Value GetNaturalHeight(const Napi::CallbackInfo&);
        Napi::Value GetSrc(const Napi::CallbackInfo&);
        Napi::Value Decode(const Napi::CallbackInfo&);
        void SetSrc(const Napi::CallbackInfo&, const Napi::Value&);
        void SetOnload(const Napi::CallbackInfo&, const Napi::Value&);
        void Dispose();

        uint32_t m_width{1};
        uint32_t m_height{1};

        std::string m_src{};

        JsRuntimeScheduler m_runtimeScheduler;
        Napi::FunctionReference m_onloadHandlerRef;
        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};
        bx::DefaultAllocator m_allocator{};
        bimg::ImageContainer* m_imageContainer{};
    };
}
