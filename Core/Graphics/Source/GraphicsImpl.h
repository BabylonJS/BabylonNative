#pragma once

#include "BgfxCallback.h"
#include "SafeTimespanGuarantor.h"
#include "GraphicsContext.h"

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
#include <optional>
#include <unordered_map>

namespace Babylon
{
    struct WindowConfiguration;
    struct ContextConfiguration;

    class GraphicsImpl
    {
    public:
        /* ********** BEGIN UPWARD-FACING CONTRACT ********** */

        GraphicsImpl();
        virtual ~GraphicsImpl();

        void UpdateWindow(const WindowConfiguration& config);
        void UpdateContext(const ContextConfiguration& config);
        void Resize(size_t width, size_t height);

        void AddToJavaScript(Napi::Env);
        static GraphicsImpl& GetFromJavaScript(Napi::Env);

        void EnableRendering();
        void DisableRendering();

        void SetDiagnosticOutput(std::function<void(const char* output)> diagnosticOutput);

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        SafeTimespanGuarantor& GetSafeTimespanGuarantor(const char* updateName);

        /* ********** END UPWARD-FACING CONTRACT ********** */

        /* ********** BEGIN CONTEXT-FACING CONTRACT ********** */

        continuation_scheduler<>& BeforeRenderScheduler();
        continuation_scheduler<>& AfterRenderScheduler();

        Update GetUpdate(const char* updateName);

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);

        arcana::task<void, std::exception_ptr> ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        size_t GetWidth() const;
        size_t GetHeight() const;
        float GetDevicePixelRatio();

        using CaptureCallbackTicketT = arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>>::ticket;
        CaptureCallbackTicketT AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback);

        bgfx::ViewId AcquireNewViewId(bgfx::Encoder&);

        /* ********** END DOWNWARD-FACING CONTRACT ********** */

        // TODO: HACK
        GraphicsContext& GetContext()
        {
            return m_context;
        }

    private:
        friend class UpdateToken;

        static const bool s_bgfxFlipAfterRender;
        static const bgfx::RendererType::Enum s_bgfxRenderType;
        static void ConfigureBgfxPlatformData(const WindowConfiguration& config, bgfx::PlatformData& platformData);
        static void ConfigureBgfxPlatformData(const ContextConfiguration& config, bgfx::PlatformData& platformData);
        static float GetDevicePixelRatio(const WindowConfiguration& config);

        void UpdateBgfxState();
        void UpdateBgfxResolution();
        void DiscardIfDirty();
        void RequestScreenShots();
        void Frame();
        bgfx::Encoder* GetEncoderForThread();
        void EndEncoders();
        void CaptureCallback(const BgfxCallback::CaptureData&);

        arcana::affinity m_renderThreadAffinity{};
        bool m_rendering{};

        std::atomic<bgfx::ViewId> m_nextViewId{0};

        std::optional<arcana::cancellation_source> m_cancellationSource{};

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

        continuation_dispatcher<> m_beforeRenderDispatcher{};
        continuation_dispatcher<> m_afterRenderDispatcher{};

        std::mutex m_captureCallbacksMutex{};
        arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>> m_captureCallbacks{};

        arcana::blocking_concurrent_queue<std::function<void(std::vector<uint8_t>)>> m_screenShotCallbacks{};

        std::map<std::thread::id, bgfx::Encoder*> m_threadIdToEncoder{};
        std::mutex m_threadIdToEncoderMutex{};

        std::queue<std::pair<uint32_t, arcana::task_completion_source<void, std::exception_ptr>>> m_readTextureRequests{};

        std::map<std::string, SafeTimespanGuarantor> m_updateSafeTimespans{};
        std::mutex m_updateSafeTimespansMutex{};

        GraphicsContext m_context;
    };
}
