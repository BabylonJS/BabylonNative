#include "GraphicsContext.h"

#include "GraphicsImpl.h"

namespace Babylon
{
    UpdateToken::UpdateToken(GraphicsContext& context, SafeTimespanGuarantor& guarantor)
        : m_context{context}
        , m_guarantee{guarantor.GetSafetyGuarantee()}
    {
    }

    bgfx::Encoder* UpdateToken::GetEncoder()
    {
        return m_context.m_graphicsImpl.GetEncoderForThread();
    }
}

namespace Babylon
{
    GraphicsContext::GraphicsContext(GraphicsImpl& graphicsImpl)
        : m_graphicsImpl{graphicsImpl}
    {
    }

    continuation_scheduler<>& GraphicsContext::BeforeRenderScheduler()
    {
        return m_graphicsImpl.BeforeRenderScheduler();
    }

    continuation_scheduler<>& GraphicsContext::AfterRenderScheduler()
    {
        return m_graphicsImpl.AfterRenderScheduler();
    }

    Update GraphicsContext::GetUpdate(const char* updateName)
    {
        return {m_graphicsImpl.GetSafeTimespanGuarantor(updateName), *this};
    }

    void GraphicsContext::RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback)
    {
        return m_graphicsImpl.RequestScreenShot(std::move(callback));
    }

    arcana::task<void, std::exception_ptr> GraphicsContext::ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data)
    {
        return m_graphicsImpl.ReadTextureAsync(handle, data);
    }

    float GraphicsContext::GetHardwareScalingLevel()
    {
        return m_graphicsImpl.GetHardwareScalingLevel();
    }

    void GraphicsContext::SetHardwareScalingLevel(float level)
    {
        m_graphicsImpl.SetHardwareScalingLevel(level);
    }

    size_t GraphicsContext::GetWidth() const
    {
        return m_graphicsImpl.GetWidth();
    }

    size_t GraphicsContext::GetHeight() const
    {
        return m_graphicsImpl.GetHeight();
    }

    float GraphicsContext::GetDevicePixelRatio()
    {
        return m_graphicsImpl.GetDevicePixelRatio();
    }

    GraphicsContext::CaptureCallbackTicketT GraphicsContext::AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback)
    {
        return m_graphicsImpl.AddCaptureCallback(std::move(callback));
    }

    bgfx::ViewId GraphicsContext::AcquireNewViewId(bgfx::Encoder& encoder)
    {
        return m_graphicsImpl.AcquireNewViewId(encoder);
    }

    void GraphicsContext::AddTexture(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format)
    {
        std::scoped_lock lock{m_textureHandleToInfoMutex};
        TextureInfo textureInfo{width, height, hasMips, numLayers, format};
        m_textureHandleToInfo.emplace(handle.idx, textureInfo);
    }

    void GraphicsContext::RemoveTexture(bgfx::TextureHandle handle)
    {
        std::scoped_lock lock{m_textureHandleToInfoMutex};
        m_textureHandleToInfo.erase(handle.idx);
    }

    TextureInfo GraphicsContext::GetTextureInfo(bgfx::TextureHandle handle)
    {
        std::scoped_lock lock{m_textureHandleToInfoMutex};
        return m_textureHandleToInfo[handle.idx];
    }
}
