#include <Babylon/Graphics/DeviceContext.h>

#include "DeviceImpl.h"

#include <napi/pointer.h>

namespace Babylon::Graphics
{
    DeviceContext& DeviceContext::GetFromJavaScript(Napi::Env env)
    {
        return DeviceImpl::GetFromJavaScript(env).GetContext();
    }

    Napi::Value DeviceContext::Create(Napi::Env env, DeviceImpl& impl)
    {
        auto* context = new DeviceContext(impl);
        return Napi::Pointer<DeviceContext>::Create(env, context, Napi::NapiPointerDeleter(context));
    }

    DeviceContext& DeviceContext::GetFromJavaScript(Napi::Value value)
    {
        return *value.As<Napi::Pointer<DeviceContext>>().Get();
    }

    DeviceContext::DeviceContext(DeviceImpl& graphicsImpl)
        : m_graphicsImpl{graphicsImpl}
    {
    }

    continuation_scheduler<>& DeviceContext::BeforeRenderScheduler()
    {
        return m_graphicsImpl.BeforeRenderScheduler();
    }

    continuation_scheduler<>& DeviceContext::AfterRenderScheduler()
    {
        return m_graphicsImpl.AfterRenderScheduler();
    }

    continuation_scheduler<>& DeviceContext::FrameStartScheduler()
    {
        return m_graphicsImpl.FrameStartScheduler();
    }

    FrameCompletionScope::FrameCompletionScope(DeviceImpl& impl)
        : m_impl{&impl}
    {
        m_impl->IncrementPendingFrameScopes();
    }

    FrameCompletionScope::FrameCompletionScope(FrameCompletionScope&& other) noexcept
        : m_impl{other.m_impl}
    {
        other.m_impl = nullptr;
    }

    FrameCompletionScope::~FrameCompletionScope()
    {
        if (m_impl)
        {
            m_impl->DecrementPendingFrameScopes();
        }
    }

    FrameCompletionScope DeviceContext::AcquireFrameCompletionScope()
    {
        return FrameCompletionScope{m_graphicsImpl};
    }

    void DeviceContext::SetActiveEncoder(bgfx::Encoder* encoder)
    {
        m_graphicsImpl.SetActiveEncoder(encoder);
    }

    bgfx::Encoder* DeviceContext::GetActiveEncoder()
    {
        return m_graphicsImpl.GetActiveEncoder();
    }

    void DeviceContext::RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback)
    {
        return m_graphicsImpl.RequestScreenShot(std::move(callback));
    }

    void DeviceContext::SetRenderResetCallback(std::function<void()> callback)
    {
        return m_graphicsImpl.SetRenderResetCallback(std::move(callback));
    }

    arcana::task<void, std::exception_ptr> DeviceContext::ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data, uint8_t mipLevel)
    {
        return m_graphicsImpl.ReadTextureAsync(handle, data, mipLevel);
    }

    float DeviceContext::GetHardwareScalingLevel()
    {
        return m_graphicsImpl.GetHardwareScalingLevel();
    }

    void DeviceContext::SetHardwareScalingLevel(float level)
    {
        m_graphicsImpl.SetHardwareScalingLevel(level);
    }

    size_t DeviceContext::GetWidth() const
    {
        return m_graphicsImpl.GetWidth();
    }

    size_t DeviceContext::GetHeight() const
    {
        return m_graphicsImpl.GetHeight();
    }

    float DeviceContext::GetDevicePixelRatio()
    {
        return m_graphicsImpl.GetDevicePixelRatio();
    }

    DeviceContext::CaptureCallbackTicketT DeviceContext::AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback)
    {
        return m_graphicsImpl.AddCaptureCallback(std::move(callback));
    }

    bgfx::ViewId DeviceContext::AcquireNewViewId()
    {
        return m_graphicsImpl.AcquireNewViewId();
    }

    void DeviceContext::AddTexture(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format)
    {
        std::scoped_lock lock{m_textureHandleToInfoMutex};
        TextureInfo textureInfo{width, height, hasMips, numLayers, format};
        m_textureHandleToInfo.emplace(handle.idx, textureInfo);
    }

    void DeviceContext::RemoveTexture(bgfx::TextureHandle handle)
    {
        std::scoped_lock lock{m_textureHandleToInfoMutex};
        m_textureHandleToInfo.erase(handle.idx);
    }

    TextureInfo DeviceContext::GetTextureInfo(bgfx::TextureHandle handle)
    {
        std::scoped_lock lock{m_textureHandleToInfoMutex};
        return m_textureHandleToInfo[handle.idx];
    }

    uintptr_t DeviceContext::GetDeviceId() const
    {
       return m_graphicsImpl.GetId();
    }
}
