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

    void FrameBuffer::Clear(bgfx::Encoder& encoder, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil)
    {
        // BGFX requires us to create a new viewID, this will ensure that the view gets cleared.
        m_viewId = m_context.AcquireNewViewId(encoder);

        bgfx::setViewMode(m_viewId.value(), bgfx::ViewMode::Sequential);
        bgfx::setViewClear(m_viewId.value(), flags, rgba, depth, stencil);
        bgfx::setViewFrameBuffer(m_viewId.value(), m_handle);

        // If a scissor is set, we need to set the BGFX view rect to match it before clearing.
        // We set the view rect instead of the view scissor because BGFX clears after the view rect is set and before
        // the view scissor is set.
        // 
        // If no scissor is set, WebGL cleans the entire screen, so set the view rect to cover the entire screen before
        // clearing to match WebGL's behavior; otherwise BGFX will only clear the view rect.
        //
        // Note that the view rect and view scissor are reset to the desired dimensions before the encoder is submitted.
        if (m_desiredScissor.Width != 0.0 || m_desiredScissor.Height != 0.0)
        {
            const auto flippedScissor = GetFlippedScissor();
            bgfx::setViewRect(
                m_viewId.value(),
                static_cast<uint16_t>(flippedScissor.X),
                static_cast<uint16_t>(flippedScissor.Y),
                static_cast<uint16_t>(flippedScissor.Width),
                static_cast<uint16_t>(flippedScissor.Height));

            m_bgfxViewPort = {
                m_desiredScissor.X / Width(),
                m_desiredScissor.Y / Height(),
                m_desiredScissor.Width / Width(),
                m_desiredScissor.Height / Height(),
            };
        }
        else
        {
            bgfx::setViewRect(m_viewId.value(), 0, 0, Width(), Height());
            m_bgfxViewPort = {0, 0, 1, 1};
        }
        encoder.touch(m_viewId.value());

    }

    void FrameBuffer::SetViewPort(bgfx::Encoder& encoder, float x, float y, float width, float height)
    {
        m_desiredViewPort = {x, y, width, height};
        SetBgfxViewPortAndScissor(encoder, m_desiredViewPort, m_desiredScissor);
    }

    void FrameBuffer::SetScissor(bgfx::Encoder& encoder, float x, float y, float width, float height)
    {
        m_desiredScissor = {x, y, width, height};
        SetBgfxViewPortAndScissor(encoder, m_desiredViewPort, m_desiredScissor);
    }

    void FrameBuffer::Submit(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint8_t flags)
    {
        SetBgfxViewPortAndScissor(encoder, m_desiredViewPort, m_desiredScissor);
        encoder.submit(m_viewId.value(), programHandle, 0, flags);
    }

    void FrameBuffer::Blit(bgfx::Encoder& encoder, bgfx::TextureHandle dst, uint16_t dstX, uint16_t dstY, bgfx::TextureHandle src, uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height)
    {
        //In order for Blit to work properly we need to force the creation of a new ViewID.
        SetBgfxViewPortAndScissor(encoder, m_desiredViewPort, m_desiredScissor);
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

    /// Returns the desired scissor rect with the Y coordinate flipped.
    ///
    /// In WebGL the scissor x/y coords are the lower left of the rect, but in BGFX the scissor x/y coords are the
    /// upper left. This function returns the given WebGL scissor rect converted to a BGFX scissor rect and makes sure
    /// y is not negative.
    ///
    /// Note that this function sets the y coord to 0 if the desired scissor's height is 0 because BGFX requires the
    /// x, y, width, and height to all be zero to disable the scissor. Without this extra check this function will set
    /// the y coord to the framebuffer's height, which will make BGFX set an invalid scissor rect (the top and bottom
    /// of the scissor rect will have the same value), and may crash the renderer in some cases.
    ///
    Rect FrameBuffer::GetFlippedScissor() const
    {
        Rect scissor = m_desiredScissor;
        scissor.Y = m_desiredScissor.Height == 0.0f ? 0.0f : std::max(0.0f, (Height() - m_desiredScissor.Y) - m_desiredScissor.Height);
        return scissor;
    }

    void FrameBuffer::SetBgfxViewPortAndScissor(bgfx::Encoder& encoder, const Rect& viewPort, const Rect& scissor)
    {
        if (m_viewId.has_value() && viewPort.Equals(m_bgfxViewPort) && scissor.Equals(m_bgfxScissor))
        {
            return;
        }

        m_viewId = m_context.AcquireNewViewId(encoder);

        m_bgfxViewPort = viewPort;
        m_bgfxScissor = scissor;

        bgfx::setViewMode(m_viewId.value(), bgfx::ViewMode::Sequential);
        bgfx::setViewClear(m_viewId.value(), BGFX_CLEAR_NONE, 0, 1.0f, 0);
        bgfx::setViewFrameBuffer(m_viewId.value(), m_handle);
        bgfx::setViewRect(m_viewId.value(),
            static_cast<uint16_t>(m_bgfxViewPort.X * Width()),
            static_cast<uint16_t>(m_bgfxViewPort.Y * Height()),
            static_cast<uint16_t>(m_bgfxViewPort.Width * Width()),
            static_cast<uint16_t>(m_bgfxViewPort.Height * Height()));

        const auto flippedScissor = GetFlippedScissor();        
        bgfx::setViewScissor(
            m_viewId.value(),
            static_cast<uint16_t>(flippedScissor.X),
            static_cast<uint16_t>(flippedScissor.Y),
            static_cast<uint16_t>(flippedScissor.Width),
            static_cast<uint16_t>(flippedScissor.Height));
    }

    bool Rect::Equals(const Rect& other) const
    {
        return std::abs(X - other.X) < std::numeric_limits<float>::epsilon() &&
               std::abs(Y - other.Y) < std::numeric_limits<float>::epsilon() &&
               std::abs(Width - other.Width) < std::numeric_limits<float>::epsilon() &&
               std::abs(Height - other.Height) < std::numeric_limits<float>::epsilon();
    }
}
