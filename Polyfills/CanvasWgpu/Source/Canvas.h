#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <map>
#include <string>
#include <vector>

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
            MonitoredResource(Canvas::Impl& impl)
                : m_impl(impl.shared_from_this())
            {
                m_impl->AddMonitoredResource(this);
            }

            virtual ~MonitoredResource()
            {
                m_impl->RemoveMonitoredResource(this);
            }

            virtual void FlushGraphicResources() = 0;

        private:
            // Canvas::Impl is app-owned while monitored resources are GC-owned.
            // Keep the impl alive until monitored resources are released.
            std::shared_ptr<Canvas::Impl> m_impl;
        };

    private:
        Napi::Env m_env;
        std::vector<MonitoredResource*> m_monitoredResources{};

        void AddToJavaScript(Napi::Env env);
        void AddMonitoredResource(MonitoredResource* monitoredResource);
        void RemoveMonitoredResource(MonitoredResource* monitoredResource);

        friend struct MonitoredResource;
    };
} // namespace Babylon::Polyfills

namespace Babylon::Polyfills::Internal
{
    class NativeCanvas final : public Napi::ObjectWrap<NativeCanvas>, Polyfills::Canvas::Impl::MonitoredResource
    {
    public:
        static void Initialize(Napi::Env env);

        explicit NativeCanvas(const Napi::CallbackInfo& info);
        ~NativeCanvas() override;

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

        static inline std::map<std::string, std::vector<uint8_t>> fontsInfos;

        // CanvasWgpu currently renders to an internal femtovg/wgpu target.
        // The return value indicates whether the next flush should clear first.
        bool UpdateRenderTarget();

    private:
        Napi::Value GetContext(const Napi::CallbackInfo&);
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        void SetWidth(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        void SetHeight(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetCanvasTexture(const Napi::CallbackInfo&);
        static void LoadTTF(const Napi::CallbackInfo& info);
        static Napi::Value LoadTTFAsync(const Napi::CallbackInfo& info);
        static Napi::Value ParseColor(const Napi::CallbackInfo& info);
        void Remove(const Napi::CallbackInfo& info);
        void Dispose(const Napi::CallbackInfo& info);
        void Dispose();

        Napi::ObjectReference m_contextObject{};
        Napi::ObjectReference m_canvasTexturePayload{};
        void* m_canvasTextureNativeHandle{};
        uint16_t m_canvasTexturePayloadWidth{};
        uint16_t m_canvasTexturePayloadHeight{};
        uint64_t m_canvasTexturePayloadGeneration{};
        uint16_t m_width{1};
        uint16_t m_height{1};
        bool m_dirty{};
        bool m_clear{};

        void FlushGraphicResources() override;
    };
} // namespace Babylon::Polyfills::Internal
