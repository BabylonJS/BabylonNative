#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/RendererType.h>

#include <atomic>
#include <future>
#include <memory>

namespace Babylon::Graphics
{
    enum class DepthStencilFormat
    {
        // Do not create a depth/stencil texture.
        None,

        // Create a 32-bit depth texture with no stencil.
        Depth32,

        // Create a combined 24-bit depth and 8-bit stencil texture.
        Depth24Stencil8,
    };

    struct Configuration
    {
        // Custom device to use instead of creating one internally.
        DeviceT Device{};

        // The platform specific window.
        WindowT Window{};

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        // Color back buffer to use instead of creating one internally.
        // @remarks Only available for D3D11.
        BackBufferColorT BackBufferColor{};

        // Depth stencil back buffer to use instead of creating one internally.
        // @remarks Only available for D3D11. DepthStencilFormat is ignored when specified.
        BackBufferDepthStencilT BackBufferDepthStencil{};
#endif

        // The resolution width in device-independent pixels (a.k.a., dp or dip).
        // @remarks The rendering width is computed from this value divided by the hardware scaling level.
        size_t Width{};

        // The resolution height in device-independent pixels (a.k.a., dp or dip).
        // @remarks The rendering height is computed from this value divided by the hardware scaling level.
        size_t Height{};

        // MSAA sample values can be 2, 4, 8 or 16. Other values will disable MSAA.
        uint8_t MSAASamples{};

        // When enabled, back buffer will be premultiplied with alpha value.
        bool AlphaPremultiplied{};

        // Format to use when creating the depth/stencil texture for the back buffer.
        // Specify DepthStencilFormat::None to not create a depth/stencil texture.
        DepthStencilFormat BackBufferDepthStencilFormat{DepthStencilFormat::Depth24Stencil8};
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
        void UpdateMSAA(uint8_t value);
        void UpdateAlphaPremultiplied(bool enabled);

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        void UpdateBackBuffer(BackBufferColorT backBufferColor, BackBufferDepthStencilT backBufferDepthStencil = {});
#endif

        void AddToJavaScript(Napi::Env);

        Napi::Value CreateContext(Napi::Env);

        void EnableRendering();
        void DisableRendering();

        // Called from the render thread (main/UI thread) to pump bgfx rendering.
        // Blocks until the API thread (JS thread) calls bgfx::frame().
        void RenderFrame(int32_t _timeoutInMs = -1);

        // Shuts down the frame loop and rendering.  Must be called from the
        // render/main thread (the thread that calls RenderFrame).
        // 1. Stops the frame loop and pumps RenderFrame() until the JS thread
        //    finishes the in-flight frame.
        // 2. Dispatches beforeDisableRendering (if provided) and DisableRendering()
        //    to the JS thread, and pumps RenderFrame() until bgfx::shutdown()
        //    completes.
        // After this call the device is shut down; destroy the JS runtime and
        // then the Device.
        void Shutdown(std::function<void()> beforeDisableRendering = {});

        void SetDiagnosticOutput(std::function<void(const char* output)> outputFunction);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        float GetDevicePixelRatio() const;

        PlatformInfo GetPlatformInfo() const;

    private:
        // Starts a self-driving frame loop on the JS thread.
        // Initialises bgfx, opens the guarantor, and uses JsRuntime::Dispatch
        // to continuously pump frames (bgfx::frame) after rAF callbacks.
        void StartFrameLoop(const char* updateName, Napi::Env env);

        // Signal the frame pump to stop.  After calling this, keep pumping
        // RenderFrame() on the render thread to unblock any in-flight bgfx::frame().
        void StopFrameLoop();

        bool IsFrameLoopRunning() const;

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        std::unique_ptr<DeviceImpl> m_impl{};
        JsRuntime* m_jsRuntime{};
        std::atomic<bool> m_frameLoopRunning{false};
        std::atomic<bool> m_stopRequested{false};
    };
}
