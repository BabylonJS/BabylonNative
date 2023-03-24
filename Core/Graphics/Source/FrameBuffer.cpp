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

    void FrameBuffer::Bind(bgfx::Encoder& encoder)
    {
        //BGFX might reset view state after drawing is done.
        //In order drawing using multiple render targets (like shadows) to work properly we must reset view clear state when we bind, making sure it is in a valid state.
        ViewState emptyState{};
        emptyState.Flags = BGFX_CLEAR_NONE;
        emptyState.Rgba = 0x000000ff;
        emptyState.Stencil = 0;
        emptyState.Depth = 1.0f;
        emptyState.ViewPort = m_bgfxViewState.ViewPort;

        SetBgfxViewState(encoder, emptyState, true);
    }

    void FrameBuffer::Unbind(bgfx::Encoder&)
    {
    }

    void FrameBuffer::Clear(bgfx::Encoder& encoder, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil)
    {
        m_expectedViewState.Flags = flags;
        m_expectedViewState.Rgba = rgba;
        m_expectedViewState.Depth = depth;
        m_expectedViewState.Stencil = stencil;

        ViewState viewState = m_expectedViewState;
        viewState.ViewPort = {0, 0, 1, 1};

        //Set state to full viewport to perform a clear (webGL clear does not consider viewport).
        SetBgfxViewState(encoder, viewState);
        encoder.touch(m_viewId);

        //Set viewport back to the expected value.
        SetBgfxViewState(encoder, m_expectedViewState);
    }

    void FrameBuffer::SetViewPort(bgfx::Encoder& encoder, float x, float y, float width, float height)
    {
        m_expectedViewState.ViewPort = {x, y, width, height};
        SetBgfxViewState(encoder, m_expectedViewState);
    }

    void FrameBuffer::Submit(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint8_t flags)
    {
        encoder.submit(m_viewId, programHandle, 0, flags);
    }

    void FrameBuffer::Blit(bgfx::Encoder& encoder, bgfx::TextureHandle dst, uint16_t dstX, uint16_t dstY, bgfx::TextureHandle src, uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height)
    {
        //In order for Blit to work properly we need to force the creation of a new ViewID.
        SetBgfxViewState(encoder, m_expectedViewState, true);
        encoder.blit(m_viewId, dst, dstX, dstY, src, srcX, srcY, width, height);
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

    void FrameBuffer::SetBgfxViewState(bgfx::Encoder& encoder, const ViewState& viewState, bool force)
    {
        if (viewState.Equals(m_bgfxViewState) && !force)
            return;

        m_viewId = m_context.AcquireNewViewId(encoder);

        m_bgfxViewState = viewState;

        bgfx::setViewMode(m_viewId, bgfx::ViewMode::Sequential);
        bgfx::setViewClear(m_viewId, m_bgfxViewState.Flags, m_bgfxViewState.Rgba, m_bgfxViewState.Depth, m_bgfxViewState.Stencil);
        bgfx::setViewFrameBuffer(m_viewId, m_handle);
        bgfx::setViewRect(m_viewId,
            static_cast<uint16_t>(m_bgfxViewState.ViewPort.X * Width()),
            static_cast<uint16_t>(m_bgfxViewState.ViewPort.Y * Height()),
            static_cast<uint16_t>(m_bgfxViewState.ViewPort.Width * Width()),
            static_cast<uint16_t>(m_bgfxViewState.ViewPort.Height * Height()));
    }

    bool ViewPort::Equals(const ViewPort& other) const
    {
        return std::abs(X - other.X) < std::numeric_limits<float>::epsilon() &&
               std::abs(Y - other.Y) < std::numeric_limits<float>::epsilon() &&
               std::abs(Width - other.Width) < std::numeric_limits<float>::epsilon() &&
               std::abs(Height - other.Height) < std::numeric_limits<float>::epsilon();
    }

    bool ViewState::Equals(const ViewState& other) const
    {
        return ViewPort.Equals(other.ViewPort) &&
               Flags == other.Flags &&
               Rgba == other.Rgba &&
               Stencil == other.Stencil &&
               std::abs(Depth - other.Depth) < std::numeric_limits<float>::epsilon();
    }
}
