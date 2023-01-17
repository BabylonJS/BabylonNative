#include "FrameBuffer.h"
#include "DeviceImpl.h"
#include <arcana/macros.h>
#include <cmath>

namespace Babylon::Graphics
{
    namespace
    {
        void SetDefaultClearMode(bgfx::ViewId viewId, bgfx::FrameBufferHandle handle, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil)
        {
            bgfx::setViewMode(viewId, bgfx::ViewMode::Sequential);
            bgfx::setViewClear(viewId, flags, rgba, depth, stencil);
            bgfx::setViewFrameBuffer(viewId, handle);
        }

        void setViewPort(bgfx::ViewId viewId, const ViewPort& viewPort, uint16_t width, uint16_t height)
        {
            bgfx::setViewRect(viewId,
                static_cast<uint16_t>(viewPort.X * width),
                static_cast<uint16_t>(viewPort.Y * height),
                static_cast<uint16_t>(viewPort.Width * width),
                static_cast<uint16_t>(viewPort.Height * height));
        }
    }

    FrameBuffer::FrameBuffer(DeviceContext& context, bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool defaultBackBuffer, bool hasDepth, bool hasStencil)
        : m_context{context}
        , m_handle{handle}
        , m_width{width}
        , m_height{height}
        , m_defaultBackBuffer{defaultBackBuffer}
        , m_hasDepth(hasDepth)
        , m_hasStencil(hasStencil)
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
        return (m_width == 0 ? static_cast<uint16_t>(m_context.GetWidth()) : m_width);
    }

    uint16_t FrameBuffer::Height() const
    {
        return (m_height == 0 ? static_cast<uint16_t>(m_context.GetHeight()) : m_height);
    }

    bool FrameBuffer::DefaultBackBuffer() const
    {
        return m_defaultBackBuffer;
    }

    void FrameBuffer::Bind(bgfx::Encoder& encoder)
    {
        m_viewId = m_context.AcquireNewViewId(encoder);
        
        //Reset view state for next frame.
        m_viewPort = {0, 0, 1, 1};
        m_flags = BGFX_CLEAR_NONE;
        m_rgba = 0x000000ff;
        m_depth = 1.0f;
        m_stencil = 0;
        
        SetDefaultClearMode(m_viewId, m_handle, BGFX_CLEAR_NONE, 0x000000ff, 1.0f, 0);
        setViewPort(m_viewId, m_viewPort, Width(), Height());
        m_hasViewIdBeenUsed = false;
    }

    void FrameBuffer::Unbind(bgfx::Encoder&)
    {
    }

    void FrameBuffer::Clear(bgfx::Encoder& encoder, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil)
    {
        m_flags = flags;
        m_rgba = rgba;
        m_depth = depth;
        m_stencil = stencil;

        if (m_hasViewIdBeenUsed)
        {
            m_viewId = m_context.AcquireNewViewId(encoder);
            SetDefaultClearMode(m_viewId, m_handle, m_flags, m_rgba, m_depth, m_stencil);
            setViewPort(m_viewId, m_viewPort, Width(), Height());
        }
        else
        {
            bgfx::setViewClear(m_viewId, m_flags, m_rgba, m_depth, m_stencil);
        }

        m_hasViewIdBeenUsed = true;
        encoder.touch(m_viewId);
    }

    void FrameBuffer::SetViewPort(bgfx::Encoder& encoder, float x, float y, float width, float height)
    {
        if (m_viewPort.X == x && m_viewPort.Y == y && m_viewPort.Width == width && m_viewPort.Height == height)
        {
            return;
        }

        m_viewPort = {x, y, width, height};

        if (m_hasViewIdBeenUsed)
        {
            m_viewId = m_context.AcquireNewViewId(encoder);
            SetDefaultClearMode(m_viewId, m_handle, m_flags, m_rgba, m_depth, m_stencil);
        }

        setViewPort(m_viewId, m_viewPort, Width(), Height());
        m_hasViewIdBeenUsed = true;
    }

    void FrameBuffer::Submit(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint8_t flags)
    {
        encoder.submit(m_viewId, programHandle, 0, flags);
        m_hasViewIdBeenUsed = true;
    }

    void FrameBuffer::Blit(bgfx::Encoder& encoder, bgfx::TextureHandle dst, uint16_t dstX, uint16_t dstY, bgfx::TextureHandle src, uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height)
    {
        // 1 blit per view, create a new viewId for each blit
        // TODO: Really? Why? Is this from the examples or something?
        m_viewId = m_context.AcquireNewViewId(encoder);
        SetDefaultClearMode(m_viewId, m_handle, m_flags, m_rgba, m_depth, m_stencil);
        setViewPort(m_viewId, m_viewPort, Width(), Height());

        encoder.blit(m_viewId, dst, dstX, dstY, src, srcX, srcY, width, height);
        m_hasViewIdBeenUsed = true;
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

    bool ViewPort::Equals(const ViewPort& other) const
    {
        return
            std::abs(X - other.X) < std::numeric_limits<float>::epsilon() &&
            std::abs(Y - other.Y) < std::numeric_limits<float>::epsilon() &&
            std::abs(Width - other.Width) < std::numeric_limits<float>::epsilon() &&
            std::abs(Height - other.Height) < std::numeric_limits<float>::epsilon();
    }
}
