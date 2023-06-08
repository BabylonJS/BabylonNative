#include "FrameBuffer.h"
#include "DeviceImpl.h"
#include <arcana/macros.h>
#include <cmath>

namespace Babylon::Graphics
{
    FrameBuffer::FrameBuffer(DeviceContext& context, bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool defaultBackBuffer, bool hasDepth, bool hasStencil)
        : m_context{context}
        , m_handle{handle}
        , m_width{width}
        , m_height{height}
        , m_defaultBackBuffer{defaultBackBuffer}
        , m_hasDepth{hasDepth}
        , m_hasStencil{hasStencil}
        , m_disposed{false}
    {
    }

    FrameBuffer::~FrameBuffer()
    {
        Dispose();
    }

    bgfx::FrameBufferHandle FrameBuffer::Handle() const
    {
        return m_handle;
    }

    uint16_t FrameBuffer::Width() const
    {
        return (m_width == 0 ? static_cast<uint16_t>(m_context.GetWidth() / m_context.GetHardwareScalingLevel()) : m_width);
    }

    uint16_t FrameBuffer::Height() const
    {
        return (m_height == 0 ? static_cast<uint16_t>(m_context.GetHeight() / m_context.GetHardwareScalingLevel()) : m_height);
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

    void FrameBuffer::SetScissor(bgfx::Encoder& encoder, float x, float y, float width, float height)
    {
        encoder.setScissor(
            static_cast<uint16_t>(x),
            static_cast<uint16_t>(y),
            static_cast<uint16_t>(width),
            static_cast<uint16_t>(height));
    }

    void FrameBuffer::Clear(bgfx::Encoder& encoder, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil)
    {
        // BGFX requires us to create a new viewID, this will ensure that the view gets cleaned.
        m_viewId = m_context.AcquireNewViewId(encoder);

        bgfx::setViewMode(m_viewId.value(), bgfx::ViewMode::Sequential);
        bgfx::setViewClear(m_viewId.value(), flags, rgba, depth, stencil);
        bgfx::setViewFrameBuffer(m_viewId.value(), m_handle);

        // BGFX will consider the viewport when cleaning the screen, but WebGL always cleans the entire screen.
        // That's why we always set the viewport to {0, 0, 1, 1} when cleaning.
        bgfx::setViewRect(m_viewId.value(), 0, 0, Width(), Height());
        encoder.touch(m_viewId.value());

        m_bgfxViewPort = {0, 0, 1, 1};
    }

    void FrameBuffer::SetViewPort(bgfx::Encoder& encoder, float x, float y, float width, float height)
    {
        m_desiredViewPort = {x, y, width, height};
        SetBgfxViewPort(encoder, m_desiredViewPort);
    }

    void FrameBuffer::Submit(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint8_t flags)
    {
        SetBgfxViewPort(encoder, m_desiredViewPort);
        encoder.submit(m_viewId.value(), programHandle, 0, flags);
    }

    void FrameBuffer::Blit(bgfx::Encoder& encoder, bgfx::TextureHandle dst, uint16_t dstX, uint16_t dstY, bgfx::TextureHandle src, uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height)
    {
        //In order for Blit to work properly we need to force the creation of a new ViewID.
        SetBgfxViewPort(encoder, m_desiredViewPort);
        encoder.blit(m_viewId.value(), dst, dstX, dstY, src, srcX, srcY, width, height);
    }

    void FrameBuffer::SetStencil(bgfx::Encoder& encoder, uint32_t stencilState)
    {
        encoder.setStencil(m_hasStencil ? stencilState : 0);
    }

    void FrameBuffer::Dispose()
    {
        if (m_disposed)
        {
            return;
        }

        if (bgfx::isValid(m_handle))
        {
            bgfx::destroy(m_handle);
        }
        m_handle = BGFX_INVALID_HANDLE;
        m_disposed = true;
    }

    void FrameBuffer::SetBgfxViewPort(bgfx::Encoder& encoder, const ViewPort& viewPort)
    {
        if (m_viewId.has_value() && viewPort.Equals(m_bgfxViewPort))
        {
            return;
        }

        m_viewId = m_context.AcquireNewViewId(encoder);

        m_bgfxViewPort = viewPort;

        bgfx::setViewMode(m_viewId.value(), bgfx::ViewMode::Sequential);
        bgfx::setViewClear(m_viewId.value(), BGFX_CLEAR_NONE, 0, 1.0f, 0);
        bgfx::setViewFrameBuffer(m_viewId.value(), m_handle);
        bgfx::setViewRect(m_viewId.value(),
            static_cast<uint16_t>(m_bgfxViewPort.X * Width()),
            static_cast<uint16_t>(m_bgfxViewPort.Y * Height()),
            static_cast<uint16_t>(m_bgfxViewPort.Width * Width()),
            static_cast<uint16_t>(m_bgfxViewPort.Height * Height()));
    }

    bool ViewPort::Equals(const ViewPort& other) const
    {
        return std::abs(X - other.X) < std::numeric_limits<float>::epsilon() &&
               std::abs(Y - other.Y) < std::numeric_limits<float>::epsilon() &&
               std::abs(Width - other.Width) < std::numeric_limits<float>::epsilon() &&
               std::abs(Height - other.Height) < std::numeric_limits<float>::epsilon();
    }
}
