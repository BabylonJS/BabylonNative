#include <Babylon/Graphics/Device.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    Device::Device(const Configuration& config)
        : m_impl{new DeviceImpl{config}}
    {
    }

    Device::~Device() = default;

    // Move semantics
    Device::Device(Device&&) noexcept = default;
    Device& Device::operator=(Device&&) noexcept = default;

    void Device::UpdateDevice(DeviceT device) 
    {
       m_impl->UpdateDevice(device);
    }

    void Device::UpdateWindow(WindowT window)
    {
        m_impl->UpdateWindow(window);
    }

    void Device::UpdateSize(size_t width, size_t height)
    {
        m_impl->UpdateSize(width, height);
    }

    void Device::UpdateMSAA(uint8_t value)
    {
        m_impl->UpdateMSAA(value);
    }

    void Device::UpdateAlphaPremultiplied(bool enabled)
    {
        m_impl->UpdateAlphaPremultiplied(enabled);
    }

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
    void Device::UpdateBackBuffer(BackBufferColorT backBufferColor, BackBufferDepthStencilT backBufferDepthStencil)
    {
        m_impl->UpdateBackBuffer(backBufferColor, backBufferDepthStencil);
    }
#endif

    void Device::AddToJavaScript(Napi::Env env)
    {
        m_impl->AddToJavaScript(env);
    }

    Napi::Value Device::CreateContext(Napi::Env env)
    {
        return m_impl->CreateContext(env);
    }

    void Device::EnableRendering()
    {
        m_impl->EnableRendering();
    }

    void Device::DisableRendering()
    {
        m_impl->DisableRendering();
    }

    DeviceUpdate Device::GetUpdate(const char* updateName)
    {
        auto& guarantor = m_impl->GetSafeTimespanGuarantor(updateName);
        return {
            [&guarantor] {
                guarantor.Open();
            },
            [&guarantor](std::function<void()> callback) {
                guarantor.CloseScheduler()(std::move(callback));
                guarantor.RequestClose();
            }};
    }

    void Device::StartRenderingCurrentFrame()
    {
        m_impl->StartRenderingCurrentFrame();
    }

    void Device::FinishRenderingCurrentFrame()
    {
        m_impl->FinishRenderingCurrentFrame();
    }

    void Device::SetDiagnosticOutput(std::function<void(const char* output)> outputFunction)
    {
        m_impl->SetDiagnosticOutput(std::move(outputFunction));
    }

    void Device::SetHardwareScalingLevel(float level)
    {
        m_impl->SetHardwareScalingLevel(level);
    }

    float Device::GetHardwareScalingLevel()
    {
        return m_impl->GetHardwareScalingLevel();
    }

    float Device::GetDevicePixelRatio() const
    {
        return m_impl->GetDevicePixelRatio();
    }

    PlatformInfo Device::GetPlatformInfo() const
    {
        return m_impl->GetPlatformInfo();
    }
}
