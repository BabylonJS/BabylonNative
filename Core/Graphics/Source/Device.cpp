#include <Babylon/Graphics/Device.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    class Device::Impl : public DeviceImpl { };

    Device::Device()
        : m_impl{std::make_unique<Impl>()}
    {
    }

    Device::~Device() = default;

    // Move semantics
    Device::Device(Device&&) noexcept = default;
    Device& Device::operator=(Device&&) noexcept = default;

    void Device::UpdateWindow(const WindowConfiguration& config)
    {
        m_impl->UpdateWindow(config);
    }

    void Device::UpdateContext(const DeviceConfiguration& config)
    {
        m_impl->UpdateContext(config);
    }

    std::unique_ptr<Device> Device::Create(const WindowConfiguration& config)
    {
        std::unique_ptr<Device> device{new Device()};
        device->UpdateWindow(config);
        device->UpdateSize(config.Width, config.Height);
        device->UpdateMSAA(config.MSAASamples);
        device->UpdateAlphaPremultiplied(config.AlphaPremultiplied);
        return device;
    }

    std::unique_ptr<Device> Device::Create(const DeviceConfiguration& config)
    {
        std::unique_ptr<Device> device{new Device()};
        device->UpdateContext(config);
        return device;
    }

    void Device::UpdateSize(size_t width, size_t height)
    {
        m_impl->Resize(width, height);
    }

    void Device::UpdateRenderTarget(const RenderTargetInfo& renderTargetInfo)
    {
        m_impl->UpdateRenderTarget(renderTargetInfo);
    }

    void Device::UpdateMSAA(uint8_t value)
    {
        m_impl->SetMSAA(value);
    }

    void Device::UpdateAlphaPremultiplied(bool enabled)
    {
        m_impl->SetAlphaPremultiplied(enabled);
    }

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
            [&guarantor]
            { 
                guarantor.Open();
            },
            [&guarantor](std::function<void()> callback)
            {
                guarantor.CloseScheduler()(std::move(callback));
                guarantor.RequestClose();
            }
        };
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
