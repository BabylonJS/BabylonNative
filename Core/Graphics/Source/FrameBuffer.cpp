#include <Babylon/Graphics/FrameBuffer.h>
#include "DeviceImpl.h"
#include <arcana/macros.h>
#include <cmath>

namespace Babylon::Graphics
{
    FrameBuffer::FrameBuffer(DeviceContext& deviceContext, bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool defaultBackBuffer, bool hasDepth, bool hasStencil, int8_t depthStencilAttachmentIndex)
        : m_deviceContext{deviceContext}
        , m_deviceID{deviceContext.GetDeviceId()}
        , m_handle{handle}
        , m_width{width}
        , m_height{height}
        , m_defaultBackBuffer{defaultBackBuffer}
        , m_hasDepth{hasDepth}
        , m_hasStencil{hasStencil}
        , m_disposed{false}
        , m_depthStencilAttachmentIndex{depthStencilAttachmentIndex}
    {
    }

    FrameBuffer::~FrameBuffer()
    {
        Dispose();
    }

    void FrameBuffer::Dispose()
    {
        if (m_disposed)
        {
            return;
        }

        if (m_deviceID == m_deviceContext.GetDeviceId())
        {
            if (m_depthStencilAttachmentIndex >= 0)
            {
                bgfx::destroy(bgfx::getTexture(m_handle, m_depthStencilAttachmentIndex));
                m_depthStencilAttachmentIndex = -1;
            }

            if (bgfx::isValid(m_handle))
            {
                bgfx::destroy(m_handle);
                m_handle = BGFX_INVALID_HANDLE;
            }
        }

        m_disposed = true;
    }

    bgfx::FrameBufferHandle FrameBuffer::Handle() const
    {
        return m_handle;
    }

    uint16_t FrameBuffer::Width() const
    {
        return (m_width == 0 ? static_cast<uint16_t>(m_deviceContext.GetWidth() / m_deviceContext.GetHardwareScalingLevel()) : m_width);
    }

    uint16_t FrameBuffer::Height() const
    {
        return (m_height == 0 ? static_cast<uint16_t>(m_deviceContext.GetHeight() / m_deviceContext.GetHardwareScalingLevel()) : m_height);
    }

    bool FrameBuffer::DefaultBackBuffer() const
    {
        return m_defaultBackBuffer;
    }

    void FrameBuffer::Bind(bgfx::Encoder&)
    {
        m_viewId.reset();
    }

    void FrameBuffer::Unbind(bgfx::Encoder&)
    {
    }

    void FrameBuffer::Clear(bgfx::Encoder& encoder, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil)
    {
        // BGFX requires us to create a new viewID, this will ensure that the view gets cleared.
        m_viewId = m_deviceContext.AcquireNewViewId(encoder);

        bgfx::setViewMode(m_viewId.value(), bgfx::ViewMode::Sequential);
        bgfx::setViewClear(m_viewId.value(), flags, rgba, depth, stencil);
        bgfx::setViewFrameBuffer(m_viewId.value(), m_handle);

        // If a scissor is not set, WebGL clears the entire screen, so set the view rect to cover the entire screen
        // before clearing to match WebGL's behavior; otherwise BGFX will only clear the view rect.
        //
        // If a scissor is set, we need to set the BGFX view rect to match it before clearing.
        // We set the view rect instead of the view scissor because BGFX clears after the view rect is set and before
        // the view scissor is set.
        //
        // Note that the view rect and view scissor are reset to the desired dimensions before the encoder is submitted.
        if (m_desiredScissor.X == 0.0f && m_desiredScissor.Y == 0.0f && m_desiredScissor.Width == 0.0f && m_desiredScissor.Height == 0.0f)
        {
            bgfx::setViewRect(m_viewId.value(), 0, 0, Width(), Height());
            m_bgfxViewPort = {0, 0, 1, 1};
        }
        else
        {
            bgfx::setViewRect(
                m_viewId.value(),
                static_cast<uint16_t>(m_desiredScissor.X),
                static_cast<uint16_t>(m_desiredScissor.Y),
                static_cast<uint16_t>(m_desiredScissor.Width),
                static_cast<uint16_t>(m_desiredScissor.Height));

            m_bgfxViewPort = {
                m_desiredScissor.X / Width(),
                m_desiredScissor.Y / Height(),
                m_desiredScissor.Width / Width(),
                m_desiredScissor.Height / Height(),
            };
        }

        // Reset the scissor rect now that we have a new view.
        bgfx::setViewScissor(m_viewId.value());
        m_bgfxScissor = {};

        encoder.touch(m_viewId.value());
    }

    void FrameBuffer::SetViewPort(bgfx::Encoder& encoder, float x, float y, float width, float height)
    {
        m_desiredViewPort = {x, y, width, height};
        SetBgfxViewPortAndScissor(encoder, m_desiredViewPort, m_desiredScissor);
    }

    void FrameBuffer::SetScissor(bgfx::Encoder& encoder, float x, float y, float width, float height)
    {
        m_desiredScissor = GetBgfxScissor(x, y, width, height);
        SetBgfxViewPortAndScissor(encoder, m_desiredViewPort, m_desiredScissor);
    }

    void FrameBuffer::Submit(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint8_t flags)
    {
        SetBgfxViewPortAndScissor(encoder, m_desiredViewPort, m_desiredScissor);
        encoder.submit(m_viewId.value(), programHandle, 0, flags);
    }

    void FrameBuffer::Blit(bgfx::Encoder& encoder, bgfx::TextureHandle dst, uint16_t dstX, uint16_t dstY, bgfx::TextureHandle src, uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height)
    {
        // In order for Blit to work properly we need to force the creation of a new ViewID.
        SetBgfxViewPortAndScissor(encoder, m_desiredViewPort, m_desiredScissor);
        encoder.blit(m_viewId.value(), dst, dstX, dstY, src, srcX, srcY, width, height);
    }

    void FrameBuffer::SetStencil(bgfx::Encoder& encoder, uint32_t stencilState)
    {
        encoder.setStencil(m_hasStencil ? stencilState : 0);
    }

    // Returns the given scissor rect converted to the rect used by BGFX.
    Rect FrameBuffer::GetBgfxScissor(float x, float y, float width, float height) const
    {
        // If the given args are all zero then the scissor is being disabled.
        if (x == 0.0f && y == 0.0f && width == 0.0f && height == 0.0f)
        {
            return Rect{};
        }

        x = std::round(x);
        y = std::round(y);
        width = std::round(width);
        height = std::round(height);

        if (x < 0.0f)
        {
            width += x;
            x = 0.0f;
        }

        if (y < 0.0f)
        {
            height += y;
            y = 0.0f;
        }

        if (Width() < x + width)
        {
            width = Width() - x;
        }

        if (Height() < y + height)
        {
            height = Height() - y;
        }

        y = (Height() - y) - height;

        return Rect{x, y, width, height};
    }

    void FrameBuffer::SetBgfxViewPortAndScissor(bgfx::Encoder& encoder, const Rect& viewPort, const Rect& scissor)
    {
        if (m_viewId.has_value() && viewPort.Equals(m_bgfxViewPort) && scissor.Equals(m_bgfxScissor))
        {
            return;
        }

        m_viewId = m_deviceContext.AcquireNewViewId(encoder);

        bgfx::setViewMode(m_viewId.value(), bgfx::ViewMode::Sequential);
        bgfx::setViewClear(m_viewId.value(), BGFX_CLEAR_NONE, 0, 1.0f, 0);
        bgfx::setViewFrameBuffer(m_viewId.value(), m_handle);

        m_bgfxViewPort = viewPort;
        bgfx::setViewRect(m_viewId.value(),
            static_cast<uint16_t>(m_bgfxViewPort.X * Width()),
            static_cast<uint16_t>(m_bgfxViewPort.Y * Height()),
            static_cast<uint16_t>(m_bgfxViewPort.Width * Width()),
            static_cast<uint16_t>(m_bgfxViewPort.Height * Height()));

        m_bgfxScissor = scissor;
        bgfx::setViewScissor(
            m_viewId.value(),
            static_cast<uint16_t>(m_bgfxScissor.X),
            static_cast<uint16_t>(m_bgfxScissor.Y),
            static_cast<uint16_t>(m_bgfxScissor.Width),
            static_cast<uint16_t>(m_bgfxScissor.Height));
    }

    bool Rect::Equals(const Rect& other) const
    {
        return std::abs(X - other.X) < std::numeric_limits<float>::epsilon() &&
               std::abs(Y - other.Y) < std::numeric_limits<float>::epsilon() &&
               std::abs(Width - other.Width) < std::numeric_limits<float>::epsilon() &&
               std::abs(Height - other.Height) < std::numeric_limits<float>::epsilon();
    }
}
