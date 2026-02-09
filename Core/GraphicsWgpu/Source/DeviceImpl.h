#pragma once

#include "DeviceContext.h"
#include "SafeTimespanGuarantor.h"
#include "WgpuNative.h"

#include <Babylon/Graphics/Device.h>

#include <arcana/threading/cancellation.h>
#include <arcana/threading/dispatcher.h>

#include <napi/env.h>

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

namespace Babylon::Graphics
{
    class DeviceImpl
    {
    public:
        explicit DeviceImpl(const Configuration& config);
        ~DeviceImpl();

        DeviceImpl(const DeviceImpl&) = delete;
        DeviceImpl& operator=(const DeviceImpl&) = delete;

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

        size_t GetWidth() const;
        size_t GetHeight() const;

        continuation_scheduler<>& BeforeRenderScheduler();
        continuation_scheduler<>& AfterRenderScheduler();

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);

        /* ********** END DEVICE CONTEXT CONTRACT ********** */

        DeviceContext& GetContext()
        {
            return m_context;
        }

    private:
        static float GetDevicePixelRatio(WindowT window);

        uint32_t CurrentRenderWidth() const;
        uint32_t CurrentRenderHeight() const;

        struct
        {
            mutable std::recursive_mutex Mutex{};

            WindowT Window{};
            DeviceT Device{};

            struct
            {
                size_t Width{};
                size_t Height{};
                float HardwareScalingLevel{1.0f};
                float DevicePixelRatio{1.0f};
            } Resolution{};
        } m_state;

        bool m_rendering{false};

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};

        continuation_dispatcher<> m_beforeRenderDispatcher{};
        continuation_dispatcher<> m_afterRenderDispatcher{};

        std::queue<std::function<void(std::vector<uint8_t>)>> m_screenShotCallbacks{};
        std::mutex m_screenShotCallbacksMutex{};

        std::map<std::string, SafeTimespanGuarantor> m_updateSafeTimespans{};
        std::mutex m_updateSafeTimespansMutex{};

        DeviceContext m_context;
        uintptr_t m_deviceId{0};
        std::function<void()> m_renderResetCallback{};
        std::function<void(const char* output)> m_diagnosticOutput{};

        std::shared_ptr<WgpuNative> m_wgpu{};
    };
}
