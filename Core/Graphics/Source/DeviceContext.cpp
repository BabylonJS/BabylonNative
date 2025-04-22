#include "DeviceContext.h"

#include "DeviceImpl.h"

#include <napi/pointer.h>

namespace Babylon::Graphics
{
    UpdateToken::UpdateToken(DeviceContext& context, SafeTimespanGuarantor& guarantor)
        : m_context{context}
        , m_guarantee{guarantor.GetSafetyGuarantee()}
    {
    }

    bgfx::Encoder* UpdateToken::GetEncoder()
    {
        return m_context.m_graphicsImpl.GetEncoderForThread();
    }
}

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

    Update DeviceContext::GetUpdate(const char* updateName)
    {
        return {m_graphicsImpl.GetSafeTimespanGuarantor(updateName), *this};
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

    bgfx::ViewId DeviceContext::AcquireNewViewId(bgfx::Encoder& encoder)
    {
        m_currentViewIsUsed = false;
        m_lastAcquiredViewId = m_graphicsImpl.AcquireNewViewId(encoder);
        return m_lastAcquiredViewId;
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

    void DeviceContext::Blit(bgfx::Encoder& encoder, bgfx::TextureHandle dst, uint16_t dstX, uint16_t dstY, bgfx::TextureHandle src, uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height)
    {
        // Increment viewId so blit happens after the last drawcall of the last viewId if drawcall(s) have been submitted to the view.
        // Otherwise, reuse last acquired viewId to not waste an Id.
        auto viewId = m_lastAcquiredViewId;
        if (!m_currentViewIsUsed)
        {
            viewId = AcquireNewViewId(encoder);
            bgfx::touch(viewId);
        }
        encoder.blit(viewId, dst, dstX, dstY, src, srcX, srcY, width, height);
    }

    void DeviceContext::InvalidateView()
    {
        m_currentViewIsUsed = true;
    }
}
