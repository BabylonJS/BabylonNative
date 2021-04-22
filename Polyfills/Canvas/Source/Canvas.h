#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <GraphicsImpl.h>

namespace Babylon::Polyfills::Internal
{
    class NativeCanvas final : public Napi::ObjectWrap<NativeCanvas>
    {
    public:
        static constexpr auto JS_CONSTRUCTOR_NAME = "NativeCanvas";

        using ParentT = Napi::ObjectWrap<NativeCanvas>;

        static void CreateInstance(Napi::Env env);

        explicit NativeCanvas(const Napi::CallbackInfo& info);
        virtual ~NativeCanvas();

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

        static inline std::map<std::string, std::vector<uint8_t>> fontsInfos;

        void UpdateRenderTarget();
        Babylon::FrameBuffer& GetFrameBuffer() { return *m_frameBuffer; }

    private:

        Napi::Value GetContext(const Napi::CallbackInfo&);
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        void SetWidth(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        void SetHeight(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetCanvasTexture(const Napi::CallbackInfo& info);
        static void LoadTTF(const Napi::CallbackInfo& info);
        void Dispose(const Napi::CallbackInfo& info);
        

        uint32_t m_width{1};
        uint32_t m_height{1};

        Babylon::GraphicsImpl& m_graphicsImpl;

        bgfx::FrameBufferHandle m_frameBufferHandle{ bgfx::kInvalidHandle };
        Babylon::FrameBuffer* m_frameBuffer{};
        bool m_dirty{};
    };
}
