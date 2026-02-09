#include "DeviceImpl.h"

#include <Babylon/JsRuntime.h>
#include <napi/napi.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

namespace
{
    constexpr auto JS_GRAPHICS_NAME = "_Graphics";
}

namespace Babylon::Graphics
{
    DeviceImpl::DeviceImpl(const Configuration& config)
        : m_context{*this}
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Window = config.Window;
        m_state.Device = config.Device;
        m_state.Resolution.Width = std::max<size_t>(1, config.Width);
        m_state.Resolution.Height = std::max<size_t>(1, config.Height);
        m_state.Resolution.HardwareScalingLevel = 1.0f;
        m_state.Resolution.DevicePixelRatio = GetDevicePixelRatio(config.Window);
    }

    DeviceImpl::~DeviceImpl()
    {
        DisableRendering();
    }

    void DeviceImpl::UpdateWindow(WindowT window)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Window = window;
        m_state.Resolution.DevicePixelRatio = GetDevicePixelRatio(window);
    }

    void DeviceImpl::UpdateDevice(DeviceT device)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Device = device;
    }

    void DeviceImpl::UpdateSize(size_t width, size_t height)
    {
        std::shared_ptr<WgpuNative> wgpu{};
        uint32_t renderWidth{};
        uint32_t renderHeight{};

        {
            std::scoped_lock lock{m_state.Mutex};

            m_state.Resolution.Width = std::max<size_t>(1, width);
            m_state.Resolution.Height = std::max<size_t>(1, height);

            wgpu = m_wgpu;
            if (wgpu)
            {
                renderWidth = CurrentRenderWidth();
                renderHeight = CurrentRenderHeight();
            }
        }

        if (wgpu)
        {
            wgpu->Resize(renderWidth, renderHeight);
        }
    }

    void DeviceImpl::UpdateMSAA(uint8_t value)
    {
        if (m_diagnosticOutput && value > 1)
        {
            m_diagnosticOutput("WGPU backend currently ignores MSAA configuration.");
        }
    }

    void DeviceImpl::UpdateAlphaPremultiplied(bool enabled)
    {
        if (m_diagnosticOutput && enabled)
        {
            m_diagnosticOutput("WGPU backend does not yet apply alpha premultiplication controls.");
        }
    }

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
    void DeviceImpl::UpdateBackBuffer(BackBufferColorT, BackBufferDepthStencilT)
    {
        if (m_diagnosticOutput)
        {
            m_diagnosticOutput("WGPU backend ignores externally supplied back buffers.");
        }
    }
#endif

    void DeviceImpl::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_GRAPHICS_NAME, Napi::External<DeviceImpl>::New(env, this));
    }

    DeviceImpl& DeviceImpl::GetFromJavaScript(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
                    .Get(JS_GRAPHICS_NAME)
                    .As<Napi::External<DeviceImpl>>()
                    .Data();
    }

    Napi::Value DeviceImpl::CreateContext(Napi::Env env)
    {
        return DeviceContext::Create(env, *this);
    }

    void DeviceImpl::SetRenderResetCallback(std::function<void()> callback)
    {
        m_renderResetCallback = std::move(callback);
    }

    void DeviceImpl::EnableRendering()
    {
        bool shouldTriggerReset{};
        std::shared_ptr<WgpuNative> wgpu{};

        {
            std::scoped_lock lock{m_state.Mutex};
            if (m_rendering)
            {
                return;
            }

            m_cancellationSource = std::make_shared<arcana::cancellation_source>();

            WgpuBootstrapConfig config{};
            config.Width = CurrentRenderWidth();
            config.Height = CurrentRenderHeight();
            config.PreferLowPower = false;
            config.EnableValidation = false;
#if defined(__APPLE__)
#if TARGET_OS_VISION
            config.SurfaceLayer = (__bridge void*)m_state.Window;
#else
            config.SurfaceLayer = m_state.Window != nullptr ? (__bridge void*)m_state.Window.layer : nullptr;
#endif
#endif

            m_wgpu = std::make_shared<WgpuNative>(config);
            if (!m_wgpu->IsValid())
            {
                m_wgpu.reset();
                m_cancellationSource.reset();
                throw std::runtime_error{"Failed to initialize WGPU backend."};
            }

            m_rendering = true;
            shouldTriggerReset = m_deviceId != 0;
            wgpu = m_wgpu;
        }

        if (m_diagnosticOutput && wgpu)
        {
            auto info = wgpu->GetInfo();
            std::ostringstream stream{};
            stream << "WGPU initialized (backend=" << info.Backend
                   << ", vendor=0x" << std::hex << info.VendorId
                   << ", device=0x" << info.DeviceId << std::dec
                   << ", adapter=\"" << info.AdapterName << "\").";
            const auto text = stream.str();
            m_diagnosticOutput(text.c_str());
        }

        if (shouldTriggerReset && m_renderResetCallback)
        {
            m_renderResetCallback();
        }
    }

    void DeviceImpl::DisableRendering()
    {
        std::queue<std::function<void(std::vector<uint8_t>)>> pendingScreenShots{};
        std::shared_ptr<arcana::cancellation_source> cancellationSource{};

        {
            std::scoped_lock lock{m_state.Mutex};

            if (!m_rendering)
            {
                return;
            }

            cancellationSource = m_cancellationSource;

            m_wgpu.reset();
            m_cancellationSource.reset();
            m_rendering = false;
            m_deviceId++;

            std::scoped_lock screenShotLock{m_screenShotCallbacksMutex};
            pendingScreenShots.swap(m_screenShotCallbacks);
        }

        if (cancellationSource)
        {
            cancellationSource->cancel();
        }

        while (!pendingScreenShots.empty())
        {
            pendingScreenShots.front()({});
            pendingScreenShots.pop();
        }
    }

    SafeTimespanGuarantor& DeviceImpl::GetSafeTimespanGuarantor(const char* updateName)
    {
        std::scoped_lock lock{m_updateSafeTimespansMutex};
        auto [iter, inserted] = m_updateSafeTimespans.try_emplace(updateName, [this]() {
            std::scoped_lock stateLock{m_state.Mutex};
            return m_cancellationSource;
        });
        if (inserted)
        {
            iter->second.Unlock();
        }

        return iter->second;
    }

    void DeviceImpl::SetDiagnosticOutput(std::function<void(const char* output)> diagnosticOutput)
    {
        m_diagnosticOutput = std::move(diagnosticOutput);
    }

    void DeviceImpl::StartRenderingCurrentFrame()
    {
        EnableRendering();

        std::shared_ptr<arcana::cancellation_source> cancellationSource{};
        {
            std::scoped_lock lock{m_state.Mutex};
            if (!m_rendering)
            {
                return;
            }

            cancellationSource = m_cancellationSource;
        }

        if (!cancellationSource)
        {
            return;
        }

        m_beforeRenderDispatcher.tick(*cancellationSource);
    }

    void DeviceImpl::FinishRenderingCurrentFrame()
    {
        std::shared_ptr<arcana::cancellation_source> cancellationSource{};
        std::shared_ptr<WgpuNative> wgpu{};
        size_t renderWidth{};
        size_t renderHeight{};

        {
            std::scoped_lock lock{m_state.Mutex};
            if (!m_rendering)
            {
                return;
            }

            cancellationSource = m_cancellationSource;
            wgpu = m_wgpu;
            if (wgpu)
            {
                renderWidth = CurrentRenderWidth();
                renderHeight = CurrentRenderHeight();
            }
        }

        if (!cancellationSource)
        {
            return;
        }

        if (wgpu)
        {
            wgpu->Render();
        }

        m_afterRenderDispatcher.tick(*cancellationSource);

        std::queue<std::function<void(std::vector<uint8_t>)>> pendingCallbacks{};
        {
            std::scoped_lock lock{m_screenShotCallbacksMutex};
            pendingCallbacks.swap(m_screenShotCallbacks);
        }

        if (pendingCallbacks.empty())
        {
            return;
        }

        if (renderWidth == 0 || renderHeight == 0)
        {
            renderWidth = 1;
            renderHeight = 1;
        }

        std::vector<uint8_t> blackFrame(renderWidth * renderHeight * 4u, 0);

        while (!pendingCallbacks.empty())
        {
            pendingCallbacks.front()(blackFrame);
            pendingCallbacks.pop();
        }
    }

    float DeviceImpl::GetHardwareScalingLevel() const
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.HardwareScalingLevel;
    }

    void DeviceImpl::SetHardwareScalingLevel(float level)
    {
        std::shared_ptr<WgpuNative> wgpu{};
        uint32_t renderWidth{};
        uint32_t renderHeight{};

        {
            std::scoped_lock lock{m_state.Mutex};

            m_state.Resolution.HardwareScalingLevel = std::max(level, 0.0001f);

            wgpu = m_wgpu;
            if (wgpu)
            {
                renderWidth = CurrentRenderWidth();
                renderHeight = CurrentRenderHeight();
            }
        }

        if (wgpu)
        {
            wgpu->Resize(renderWidth, renderHeight);
        }
    }

    float DeviceImpl::GetDevicePixelRatio() const
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.DevicePixelRatio;
    }

    PlatformInfo DeviceImpl::GetPlatformInfo() const
    {
        return {};
    }

    uintptr_t DeviceImpl::GetId() const
    {
        return m_deviceId;
    }

    size_t DeviceImpl::GetWidth() const
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.Width;
    }

    size_t DeviceImpl::GetHeight() const
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.Height;
    }

    continuation_scheduler<>& DeviceImpl::BeforeRenderScheduler()
    {
        return m_beforeRenderDispatcher.scheduler();
    }

    continuation_scheduler<>& DeviceImpl::AfterRenderScheduler()
    {
        return m_afterRenderDispatcher.scheduler();
    }

    void DeviceImpl::RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback)
    {
        std::scoped_lock lock{m_screenShotCallbacksMutex};
        m_screenShotCallbacks.emplace(std::move(callback));
    }

    float DeviceImpl::GetDevicePixelRatio(WindowT)
    {
        return 1.0f;
    }

    uint32_t DeviceImpl::CurrentRenderWidth() const
    {
        std::scoped_lock lock{m_state.Mutex};
        const auto width = static_cast<float>(m_state.Resolution.Width);
        const auto level = std::max(m_state.Resolution.HardwareScalingLevel, 0.0001f);
        return std::max<uint32_t>(1, static_cast<uint32_t>(std::floor(width / level)));
    }

    uint32_t DeviceImpl::CurrentRenderHeight() const
    {
        std::scoped_lock lock{m_state.Mutex};
        const auto height = static_cast<float>(m_state.Resolution.Height);
        const auto level = std::max(m_state.Resolution.HardwareScalingLevel, 0.0001f);
        return std::max<uint32_t>(1, static_cast<uint32_t>(std::floor(height / level)));
    }
}
