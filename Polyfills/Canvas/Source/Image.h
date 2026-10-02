#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <UrlLib/UrlLib.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <bx/allocator.h>
#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
#include <bimg/bimg.h>
#endif

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    class NativeCanvasImage final : public Napi::ObjectWrap<NativeCanvasImage>
    {
    public:
        static void Initialize(Napi::Env env);

        static NativeCanvasImage* TryUnwrap(Napi::Env env, const Napi::Value& value);

        explicit NativeCanvasImage(const Napi::CallbackInfo& info);
        virtual ~NativeCanvasImage();

        int CreateNVGImageForContext(NVGcontext* nvgContext) const;

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }
        // Contexts cache a NanoVG image by this pointer. This changes whenever SetBuffer
        // replaces the decoded pixels, including a same-size URL or data reload.
        uint32_t GetContentGeneration() const { return m_contentGeneration; }

    private:
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        Napi::Value GetNaturalWidth(const Napi::CallbackInfo&);
        Napi::Value GetNaturalHeight(const Napi::CallbackInfo&);
        Napi::Value GetSrc(const Napi::CallbackInfo&);
        Napi::Value GetImageContainer(const Napi::CallbackInfo&);
        void SetSrc(const Napi::CallbackInfo&, const Napi::Value&);
        void SetOnload(const Napi::CallbackInfo&, const Napi::Value&);
        void SetOnerror(const Napi::CallbackInfo&, const Napi::Value&);
        void HandleLoadImageError(const Napi::Error& error);
        bool SetBuffer(gsl::span<const std::byte> buffer);
        void ReleaseImage();
        void Dispose();

        uint32_t m_width{1};
        uint32_t m_height{1};
        uint32_t m_contentGeneration{0};

        std::string m_src{};

        JsRuntimeScheduler m_runtimeScheduler;
        Napi::FunctionReference m_onloadHandlerRef;
        Napi::FunctionReference m_onerrorHandlerRef;
        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};
#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
        bimg::ImageContainer* m_imageContainer{};
#endif
    };
}
