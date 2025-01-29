#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/RendererType.h>

#include <future>
#include <memory>

namespace Babylon::Graphics
{
    struct Configuration
    {
        // Custom device to use instead of creating one internally.
        DeviceT Device{};

        // The platform specific window.
        WindowT Window{};

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        // Color back buffer (only available for D3D11).
        BackBufferColorT BackBufferColor{};

        // Depth stencil back buffer (only available for D3D11).
        BackBufferDepthStencilT BackBufferDepthStencil{};
#endif

        // The resolution width.
        size_t Width{};

        // The resolution height.
        size_t Height{};

        // The device pixel ratio.
        float DevicePixelRatio{};

        // MSAA sample values can be 2, 4, 8 or 16. Other values will disable MSAA.
        uint8_t MSAASamples{};

        // When enabled, back buffer will be premultiplied with alpha value.
        bool AlphaPremultiplied{};
    };

    class Device;

    class DeviceUpdate
    {
    public:
        void Start()
        {
            m_start();
        }

        void RequestFinish(std::function<void()> onFinishCallback)
        {
            m_requestFinish(std::move(onFinishCallback));
        }

        void Finish()
        {
            std::promise<void> promise{};
            auto future = promise.get_future();
            RequestFinish([&promise] { promise.set_value(); });
            future.wait();
        }

    private:
        friend class Device;

        template<typename StartCallableT, typename RequestEndCallableT>
        DeviceUpdate(StartCallableT&& start, RequestEndCallableT&& requestEnd)
            : m_start{std::forward<StartCallableT>(start)}
            , m_requestFinish{std::forward<RequestEndCallableT>(requestEnd)}
        {
        }

        std::function<void()> m_start{};
        std::function<void(std::function<void()>)> m_requestFinish{};
    };

    class DeviceImpl;

    class Device
    {
    public:
        explicit Device(const Configuration& config);
        ~Device();

        // Move semantics
        Device(Device&&) noexcept;
        Device& operator=(Device&&) noexcept;

        // Note: This API contract is subject to change in coming versions.
        // Features and functionalities will be added and
        // method and structure might change.

        void UpdateWindow(WindowT window);
        void UpdateDevice(DeviceT device);
        void UpdateSize(size_t width, size_t height);
        void UpdateDevicePixelRatio(float value);
        void UpdateMSAA(uint8_t value);
        void UpdateAlphaPremultiplied(bool enabled);

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        void UpdateBackBuffer(BackBufferColorT backBufferColor, BackBufferDepthStencilT backBufferDepthStencil = {});
#endif

        void AddToJavaScript(Napi::Env);

        Napi::Value CreateContext(Napi::Env);

        void EnableRendering();
        void DisableRendering();

        DeviceUpdate GetUpdate(const char* updateName);

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        void SetDiagnosticOutput(std::function<void(const char* output)> outputFunction);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float value);

        PlatformInfo GetPlatformInfo() const;

    private:
        std::unique_ptr<DeviceImpl> m_impl{};
    };
}
