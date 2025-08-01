#pragma once

#include "BgfxCallback.h"
#include "SafeTimespanGuarantor.h"
#include "DeviceContext.h"

#include <Babylon/Graphics/Device.h>

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
        void UpdateDevicePixelRatio(float value);
        void SetRenderResetCallback(std::function<void()> callback);

        void AddToJavaScript(Napi::Env);
        static DeviceImpl& GetFromJavaScript(Napi::Env);

        Napi::Value CreateContext(Napi::Env);

        void EnableRendering();
        void DisableRendering();

        SafeTimespanGuarantor& GetSafeTimespanGuarantor(const char* updateName);

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

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);

        arcana::task<void, std::exception_ptr> ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data, uint8_t mipLevel);

        using CaptureCallbackTicketT = arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>>::ticket;
        CaptureCallbackTicketT AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback);

        bgfx::ViewId AcquireNewViewId(bgfx::Encoder&);

        /* ********** END DEVICE CONTEXT CONTRACT ********** */

        // TODO: HACK
        DeviceContext& GetContext()
        {
            return m_context;
        }

    private:
        friend class UpdateToken;

        static const bgfx::RendererType::Enum s_bgfxRenderType;
        static void ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window);
        static void ConfigureBgfxRenderType(bgfx::PlatformData& pd, bgfx::RendererType::Enum& renderType);
        static float GetDevicePixelRatio(WindowT window);

        void UpdateBgfxState();
        void UpdateBgfxResolution();
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

        DeviceContext m_context;
        uintptr_t m_bgfxId = 0;
        std::function<void()> m_renderResetCallback;
    };
}
