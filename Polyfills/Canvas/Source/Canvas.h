#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <GraphicsImpl.h>
#include <FrameBuffer.h>

#include <optional>

namespace Babylon
{
    struct TextureData;
}

namespace Babylon::Polyfills::Internal
{
    class NativeCanvas final : public Napi::ObjectWrap<NativeCanvas>
    {
    public:
        static void CreateInstance(Napi::Env env);

        explicit NativeCanvas(const Napi::CallbackInfo& info);
        virtual ~NativeCanvas();

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

        static inline std::map<std::string, std::vector<uint8_t>> fontsInfos;

        // returns true if frameBuffer size has changed
        bool UpdateRenderTarget();
        Babylon::FrameBuffer& GetFrameBuffer() { return *m_frameBuffer; }

    private:
        Napi::Value GetContext(const Napi::CallbackInfo&);
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        void SetWidth(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        void SetHeight(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetCanvasTexture(const Napi::CallbackInfo& info);
        static Napi::Value LoadTTFAsync(const Napi::CallbackInfo& info);
        void Dispose(const Napi::CallbackInfo& info);
        void Dispose();

        uint32_t m_width{1};
        uint32_t m_height{1};

        Babylon::GraphicsImpl& m_graphicsImpl;

        std::unique_ptr<Babylon::FrameBuffer> m_frameBuffer;
        std::unique_ptr<TextureData> m_textureData{};
        bool m_dirty{};
    };
}
