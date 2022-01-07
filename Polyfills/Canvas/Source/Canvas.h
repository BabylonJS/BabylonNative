#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <GraphicsContext.h>
#include <FrameBuffer.h>

// HACK: for TextureData
#include <Texture.h>

namespace Babylon::Polyfills
{
    class Canvas::Impl final : public std::enable_shared_from_this<Canvas::Impl>
    {
    public:
        explicit Impl(Napi::Env);

        void FlushGraphicResources();

        static Canvas::Impl& GetFromJavaScript(Napi::Env env);

        struct MonitoredResource
        {
            MonitoredResource(Canvas::Impl& impl) : m_impl(impl)
            {
                m_impl.AddMonitoredResource(this);
            }
            virtual ~MonitoredResource()
            {
                m_impl.RemoveMonitoredResource(this);
            }

            virtual void FlushGraphicResources() = 0;
        private:
            Canvas::Impl& m_impl;
        };

    private:
        Napi::Env m_env;

        void AddToJavaScript(Napi::Env env);

        std::vector<MonitoredResource*> m_monitoredResources{};

        void AddMonitoredResource(MonitoredResource* monitoredResource);
        void RemoveMonitoredResource(MonitoredResource* monitoredResource);

        friend struct MonitoredResource;
    };
} // namespace

namespace Babylon::Polyfills::Internal
{
    class NativeCanvas final : public Napi::ObjectWrap<NativeCanvas>, Polyfills::Canvas::Impl::MonitoredResource
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

        GraphicsContext& GetGraphicsContext()
        {
            return m_graphicsContext;
        }

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

        Babylon::GraphicsContext& m_graphicsContext;

        std::unique_ptr<Babylon::FrameBuffer> m_frameBuffer;
        std::unique_ptr<TextureData> m_textureData{};
        bool m_dirty{};

        void FlushGraphicResources() override;
    };
}
