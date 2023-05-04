#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/RendererType.h>

#include <future>
#include <memory>

namespace Babylon::Graphics
{
    struct PlatformInfo
    {
        DeviceT Device;
    };

    struct Configuration
    {
        // Custom device to use instead of creating one internally.
        DeviceT Device{};

        // The platform specific window.
        WindowT Window{};

        // The resolution width.
        size_t Width{};

        // The resolution height.
        size_t Height{};

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
        Device(const Configuration& config);
        ~Device();

        // Move semantics
        Device(Device&&) noexcept;
        Device& operator=(Device&&) noexcept;

        // Note: This API contract is subject to change in coming versions.
        // Features and functionalities will be added and
        // method and structure might change.

        void UpdateWindow(WindowT window);
        void UpdateSize(size_t width, size_t height);
        void UpdateMSAA(uint8_t value);
        void UpdateAlphaPremultiplied(bool enabled);

        void AddToJavaScript(Napi::Env);

        Napi::Value CreateContext(Napi::Env);

        void EnableRendering();
        void DisableRendering();

        DeviceUpdate GetUpdate(const char* updateName);

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        void SetDiagnosticOutput(std::function<void(const char* output)> outputFunction);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        float GetDevicePixelRatio() const;

        PlatformInfo GetPlatformInfo() const;

    private:
        std::unique_ptr<DeviceImpl> m_impl{};
    };
}
