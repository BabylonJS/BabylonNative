#pragma once

#include <Babylon/Graphics/BgfxCallback.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <arcana/containers/ticketed_collection.h>
#include <arcana/threading/blocking_concurrent_queue.h>
#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>
#include <arcana/threading/affinity.h>

#include <napi/env.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <optional>
#include <unordered_map>

namespace Babylon::Graphics
{
    class DeviceImpl
    {
    public:
        DeviceImpl(const Configuration& config);
        virtual ~DeviceImpl();

        // Copy semantics
        DeviceImpl(const DeviceImpl&) = delete;
        DeviceImpl& operator=(const DeviceImpl&) = delete;

        // Move semantics
        DeviceImpl(DeviceImpl&&) noexcept = delete;
        DeviceImpl& operator=(DeviceImpl&&) noexcept = delete;


        /* ********** BEGIN DEVICE CONTRACT ********** */
        void UpdateWindow(WindowT window);
        void UpdateDevice(DeviceT device);
        void UpdateSize(size_t width, size_t height);
        void UpdateMSAA(uint8_t value);
        void UpdateAlphaPremultiplied(bool enabled);

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        void UpdateBackBuffer(BackBufferColorT backBufferColor, BackBufferDepthStencilT backBufferDepthStencil);
#endif

        void AddToJavaScript(Napi::Env);
        static DeviceImpl& GetFromJavaScript(Napi::Env);

        Napi::Value CreateContext(Napi::Env);

        void SetRenderResetCallback(std::function<void()> callback);

        void EnableRendering();
        void DisableRendering();

        void SetDiagnosticOutput(std::function<void(const char* output)> diagnosticOutput);

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        float GetHardwareScalingLevel() const;
        void SetHardwareScalingLevel(float level);

        float GetDevicePixelRatio() const;

        PlatformInfo GetPlatformInfo() const;

        uintptr_t GetId() const;

        /* ********** END DEVICE CONTRACT ********** */

        /* ********** BEGIN DEVICE CONTEXT CONTRACT ********** */

        size_t GetWidth() const { return m_state.Resolution.Width; }
        size_t GetHeight() const { return m_state.Resolution.Height; }

        continuation_scheduler<>& BeforeRenderScheduler();
        continuation_scheduler<>& AfterRenderScheduler();
        continuation_scheduler<>& FrameStartScheduler();

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);

        arcana::task<void, std::exception_ptr> ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data, uint8_t mipLevel);

        using CaptureCallbackTicketT = arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>>::ticket;
        CaptureCallbackTicketT AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback);

        bgfx::ViewId AcquireNewViewId();

        // Frame completion scope support
        void IncrementPendingFrameScopes();
        void DecrementPendingFrameScopes();

        // Active encoder for the current frame.
        void SetActiveEncoder(bgfx::Encoder* encoder);
        bgfx::Encoder* GetActiveEncoder() const;

        /* ********** END DEVICE CONTEXT CONTRACT ********** */

        // TODO: HACK
        DeviceContext& GetContext()
        {
            return m_context;
        }

    private:
        friend class FrameCompletionScope;

        static const bgfx::RendererType::Enum s_bgfxRenderType;
        static void ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window);
        static void ConfigureBgfxRenderType(bgfx::PlatformData& pd, bgfx::RendererType::Enum& renderType);
        static float GetDevicePixelRatio(WindowT window);

        void UpdateBgfxState();
        void UpdateBgfxResolution();
        void RequestScreenShots();
        void Frame();
        void CaptureCallback(const BgfxCallback::CaptureData&);

        arcana::affinity m_renderThreadAffinity{};
        bool m_rendering{};
        bool m_firstFrameStarted{};

        // The single bgfx encoder for the current frame. Acquired in
        // StartRenderingCurrentFrame, ended in FinishRenderingCurrentFrame.
        // Read by all consumers via DeviceContext::GetActiveEncoder() → DeviceImpl::GetActiveEncoder().
        bgfx::Encoder* m_frameEncoder{nullptr};

        std::atomic<bgfx::ViewId> m_nextViewId{0};

        std::optional<arcana::cancellation_source> m_cancellationSource{};

        struct
        {
            // Mutable since const getters need to lock.
            mutable std::recursive_mutex Mutex{};

            struct
            {
                bgfx::Init InitState{};
                bool Initialized{};
                bool Dirty{};
            } Bgfx{};

            struct
            {
                size_t Width{}; // in device-independent pixels
                size_t Height{}; // in device-independent pixels
                float HardwareScalingLevel{1.0f};
                float DevicePixelRatio{1.0f};
            } Resolution{};
        } m_state;

        BgfxCallback m_bgfxCallback;

        continuation_dispatcher<> m_beforeRenderDispatcher{};
        continuation_dispatcher<> m_afterRenderDispatcher{};

        // Ticked by StartRenderingCurrentFrame(). NativeEngine and NativeXr schedule
        // requestAnimationFrame tasks here so they fire once per frame at the right time.
        continuation_dispatcher<> m_frameStartDispatcher{};

        // --- Frame synchronization between main thread and JS thread ---
        //
        // bgfx itself can handle concurrent bgfx::begin() and bgfx::frame() safely
        // (begin blocks on a mutex until frame releases it). However, BabylonNative needs
        // to ensure LOGICAL frame correctness: all encoder commands intended for frame N
        // must be submitted (bgfx::end) before bgfx::frame() for frame N runs, otherwise
        // draw calls are lost or appear in the wrong frame (causing flickering/artifacts).
        //
        // Solution: a blocking gate with a reference counter.
        //
        // m_frameBlocked (bool):
        //   - true  = JS cannot acquire new FrameCompletionScopes (blocks in IncrementPendingFrameScopes)
        //   - false = JS is free to acquire scopes and use encoders
        //   - Set to false in StartRenderingCurrentFrame (opens the gate)
        //   - Set to true in FinishRenderingCurrentFrame after all scopes are released (closes the gate)
        //   - Starts as true (no frame in progress at init)
        //
        // m_pendingFrameScopes (int):
        //   - Count of active FrameCompletionScope instances
        //   - FinishRenderingCurrentFrame waits for this to reach 0
        //   - Incremented by IncrementPendingFrameScopes (called by FrameCompletionScope constructor)
        //   - Decremented by DecrementPendingFrameScopes (called by FrameCompletionScope destructor)
        //
        // m_frameSyncMutex + m_frameSyncCV:
        //   - Protects m_frameBlocked and m_pendingFrameScopes
        //   - CV is waited on by: main thread (for scopes==0) and JS thread (for !blocked)
        //   - CV is notified by: JS thread (scope released) and main thread (unblocked)
        std::mutex m_frameSyncMutex{};
        std::condition_variable m_frameSyncCV{};
        int m_pendingFrameScopes{0};
        bool m_frameBlocked{true};

        std::mutex m_captureCallbacksMutex{};
        arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>> m_captureCallbacks{};

        arcana::blocking_concurrent_queue<std::function<void(std::vector<uint8_t>)>> m_screenShotCallbacks{};

        std::queue<std::pair<uint32_t, arcana::task_completion_source<void, std::exception_ptr>>> m_readTextureRequests{};

        DeviceContext m_context;
        uintptr_t m_bgfxId = 0;
        std::function<void()> m_renderResetCallback;
    };
}
