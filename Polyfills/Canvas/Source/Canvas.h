#pragma once

#include <Babylon/Polyfills/Canvas.h>

namespace Babylon::Polyfills::Internal
{
    class Canvas final : public Napi::ObjectWrap<Canvas>
    {
    public:
        static constexpr auto JS_CONSTRUCTOR_NAME = "OffscreenCanvas";

        using ParentT = Napi::ObjectWrap<Canvas>;

        static void CreateInstance(Napi::Env env);

        explicit Canvas(const Napi::CallbackInfo& info);

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

        static inline std::map<std::string, std::vector<uint8_t>> fontsInfos;
        bgfx::FrameBufferHandle GetFrameBufferHandle() const { return m_frameBufferHandle; }
    private:

        Napi::Value GetContext(const Napi::CallbackInfo&);
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        void SetWidth(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        void SetHeight(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetCanvasTexture(const Napi::CallbackInfo& info);
        static void LoadTTF(const Napi::CallbackInfo& info);
        static void BeginContextsFrame(const Napi::CallbackInfo&);
        static void EndContextsFrame(const Napi::CallbackInfo&);


        void UpdateRenderTarget();

        uint32_t m_width{1};
        uint32_t m_height{1};

        bgfx::FrameBufferHandle m_frameBufferHandle{ bgfx::kInvalidHandle };

        // TODO : replace with an availability list
        static inline uint32_t m_nextViewId{100};
    };
}
