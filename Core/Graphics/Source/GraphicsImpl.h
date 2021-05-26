#pragma once

#include "BgfxCallback.h"
#include "FrameBufferManager.h"
#include "SafeTimespanGuarantor.h"

#include <arcana/containers/ticketed_collection.h>
#include <arcana/threading/blocking_concurrent_queue.h>
#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>
#include <arcana/threading/affinity.h>

#include <napi/env.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <memory>
#include <map>
#include <unordered_map>

namespace Babylon
{
    struct WindowConfiguration;
    struct ContextConfiguration;

    class GraphicsImpl
    {
    public:
        class UpdateToken final
        {
        public:
            UpdateToken(const UpdateToken& other) = delete;
            UpdateToken(UpdateToken&&) = default;

            bgfx::Encoder* GetEncoder();

        private:
            friend class GraphicsImpl;

            UpdateToken(GraphicsImpl&);

            GraphicsImpl& m_graphicsImpl;
            SafeTimespanGuarantor::SafetyGuarantee m_guarantee;
        };

        class RenderScheduler final
        {
        public:
            template<typename CallableT>
            void operator()(CallableT&& callable)
            {
                m_dispatcher(callable);
            }

        private:
            friend GraphicsImpl;

            arcana::manual_dispatcher<128> m_dispatcher;
        };

        struct TextureInfo final
        {
        public:
            uint16_t Width{};
            uint16_t Height{};
            bool HasMips{};
            uint16_t NumLayers{};
            bgfx::TextureFormat::Enum Format{};
        };

        GraphicsImpl();
        virtual ~GraphicsImpl();

        void UpdateWindow(const WindowConfiguration& config);
        void UpdateContext(const ContextConfiguration& config);
        void Resize(size_t width, size_t height);

        void AddToJavaScript(Napi::Env);
        static GraphicsImpl& GetFromJavaScript(Napi::Env);

        RenderScheduler& BeforeRenderScheduler();
        RenderScheduler& AfterRenderScheduler();

        void EnableRendering();
        void DisableRendering();

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        UpdateToken GetUpdateToken();

        FrameBuffer& AddFrameBuffer(bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool backBuffer);
        void RemoveFrameBuffer(const FrameBuffer& frameBuffer);
        FrameBuffer& DefaultFrameBuffer();

        void AddTexture(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format);
        void RemoveTexture(bgfx::TextureHandle handle);
        TextureInfo GetTextureInfo(bgfx::TextureHandle handle);

        void SetDiagnosticOutput(std::function<void(const char* output)> diagnosticOutput);

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);

        arcana::task<void, std::exception_ptr> ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        float GetDevicePixelRatio();

        using CaptureCallbackTicketT = arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>>::ticket;
        CaptureCallbackTicketT AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback);

        const std::unique_ptr<FrameBufferManager>& GetFrameBufferManager() const { return m_frameBufferManager; }
    private:
        friend class UpdateToken;

        template<typename WindowT>
        WindowT GetNativeWindow();

        void ConfigureBgfxPlatformData(const WindowConfiguration& config, bgfx::PlatformData& platformData);
        void ConfigureBgfxPlatformData(const ContextConfiguration& config, bgfx::PlatformData& platformData);
        void UpdateBgfxState();
        void UpdateBgfxResolution();
        float UpdateDevicePixelRatio();
        void DiscardIfDirty();
        void RequestScreenShots();
        void Frame();
        bgfx::Encoder* GetEncoderForThread();
        void EndEncoders();
        void CaptureCallback(const BgfxCallback::CaptureData&);

        arcana::affinity m_renderThreadAffinity{};
        bool m_rendering{};

        std::unique_ptr<arcana::cancellation_source> m_cancellationSource{};

        struct
        {
            std::recursive_mutex Mutex{};

            struct
            {
                bgfx::Init InitState{};
                bool Initialized{};
                bool Dirty{};
            } Bgfx{};

            struct
            {
                size_t Width{};
                size_t Height{};
                float HardwareScalingLevel{1.0f};
                float DevicePixelRatio{1.0f};
            } Resolution{};
        } m_state;

        BgfxCallback m_bgfxCallback;

        SafeTimespanGuarantor m_safeTimespanGuarantor{};

        RenderScheduler m_beforeRenderScheduler;
        RenderScheduler m_afterRenderScheduler;

        std::unique_ptr<FrameBufferManager> m_frameBufferManager{};

        std::mutex m_captureCallbacksMutex{};
        arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>> m_captureCallbacks{};

        arcana::blocking_concurrent_queue<std::function<void(std::vector<uint8_t>)>> m_screenShotCallbacks{};

        std::map<std::thread::id, bgfx::Encoder*> m_threadIdToEncoder{};
        std::mutex m_threadIdToEncoderMutex{};

        std::queue<std::pair<uint32_t, arcana::task_completion_source<void, std::exception_ptr>>> m_readTextureRequests{};

        std::unordered_map<uint16_t, TextureInfo> m_textureHandleToInfo{};
        std::mutex m_textureHandleToInfoMutex{};
    };
}
