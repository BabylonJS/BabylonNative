#include <Babylon/Graphics/FrameBuffer.h>
#include "DeviceImpl.h"
#include <arcana/macros.h>
#include <cmath>

namespace
{
    // setViewClear accepts eight attachment indices.
    constexpr uint8_t MaxColorAttachments{8};

    std::optional<uint32_t> PackClearColor(const std::array<float, 4>& color)
    {
        uint32_t packed{};
        for (const float component : color)
        {
            if (!std::isfinite(component) || component < 0 || component > 1)
            {
                return std::nullopt;
            }
            const auto byte = static_cast<uint32_t>(std::round(component * 255.f));
            if (static_cast<float>(byte) / 255.f != component)
            {
                return std::nullopt;
            }
            packed = (packed << 8) | byte;
        }
        return packed;
    }

    // Only D3D11/12 treat UINT8_MAX as an attachment skip; other backends clamp it into the palette.
    bool SupportsClearAttachmentMasking()
    {
        switch (bgfx::getRendererType())
        {
            case bgfx::RendererType::Direct3D11:
            case bgfx::RendererType::Direct3D12:
                return true;
            default:
                return false;
        }
    }
}

namespace Babylon::Graphics
{
    FrameBuffer::FrameBuffer(DeviceContext& deviceContext, bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool defaultBackBuffer, bool hasDepth, bool hasStencil, int8_t depthStencilAttachmentIndex, bool isMultisampled, uint8_t depthOneVolumeAttachmentMask)
        : m_deviceContext{deviceContext}
        , m_deviceID{deviceContext.GetDeviceId()}
        , m_handle{handle}
        , m_width{width}
        , m_height{height}
        , m_defaultBackBuffer{defaultBackBuffer}
        // XR uses default framebuffer semantics but supplies an explicit render target.
        , m_useDeviceBackBuffer{defaultBackBuffer && !bgfx::isValid(handle)}
        , m_hasDepth{hasDepth}
        , m_hasStencil{hasStencil}
        , m_isMultisampled{isMultisampled}
        , m_depthOneVolumeAttachmentMask{depthOneVolumeAttachmentMask}
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
        return m_useDeviceBackBuffer ? m_deviceContext.GetBackBufferHandle() : m_handle;
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

    bool FrameBuffer::IsMultisampled() const
    {
        // The window's sample count can change without recreating its wrapper.
        return m_defaultBackBuffer && !bgfx::isValid(m_handle) ? m_deviceContext.GetMSAASamples() > 1 : m_isMultisampled;
    }

    void FrameBuffer::Bind()
    {
        m_viewId.reset();
    }

    void FrameBuffer::Unbind()
    {
        if (m_depthOneVolumeAttachmentMask == 0 || !m_viewId.has_value())
        {
            return;
        }
        const auto view = m_deviceContext.AcquireNewViewId();
        bgfx::resetView(view);
        for (uint8_t attachment = 0; attachment < MaxColorAttachments; ++attachment)
        {
            if ((m_depthOneVolumeAttachmentMask & (1 << attachment)) != 0)
            {
                bgfx::TextureRegion source{};
                source.init(bgfx::getTexture(m_handle, attachment), 0, 0, Width(), Height());
                source.depth = 1;
                auto destination = source;
                destination.z = 1;
                m_deviceContext.GetActiveEncoder()->blit(view, destination, source);
            }
        }
    }

    void FrameBuffer::Clear(bgfx::Encoder& encoder, uint16_t flags, float r, float g, float b, float a, float depth, uint8_t stencil, uint8_t colorAttachmentMask)
    {
        const bool maskColorAttachments{colorAttachmentMask != UINT8_MAX && bgfx::isValid(m_handle) && SupportsClearAttachmentMasking()};
        const std::array<float, 4> color{r, g, b, a};
        const auto packedColor = maskColorAttachments ? std::nullopt : PackClearColor(color);
        const bool usePalette = (flags & BGFX_CLEAR_COLOR) != 0 && !packedColor.has_value();
        const auto paletteIndex = usePalette ? m_deviceContext.AcquireClearPaletteIndex(color) : uint8_t{};

        // BGFX requires us to create a new viewID, this will ensure that the view gets cleared.
        m_viewId = m_deviceContext.AcquireNewViewId();
        m_viewIdGeneration = m_deviceContext.ViewIdGeneration();

        bgfx::setViewMode(m_viewId.value(), bgfx::ViewMode::Sequential);
        // Float palette entries preserve HDR clears and per-attachment masks.
        if (usePalette)
        {
            uint8_t indices[MaxColorAttachments];
            for (uint8_t attachment = 0; attachment < MaxColorAttachments; ++attachment)
            {
                indices[attachment] = !maskColorAttachments || (colorAttachmentMask & (1 << attachment)) != 0 ? paletteIndex : UINT8_MAX;
            }
            bgfx::setViewClear(m_viewId.value(), flags, depth, stencil,
                indices[0], indices[1], indices[2], indices[3], indices[4], indices[5], indices[6], indices[7]);
        }
        else
        {
            bgfx::setViewClear(m_viewId.value(), flags, packedColor.value_or(0), depth, stencil);
        }
        bgfx::setViewFrameBuffer(m_viewId.value(), Handle());

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

        // Keep texture bindings and uniform writes (including OpenGL sampler indices) across the empty draw.
        constexpr uint8_t discardFlags{BGFX_DISCARD_ALL & ~(BGFX_DISCARD_BINDINGS | BGFX_DISCARD_STATE)};
        encoder.discard(discardFlags);
        encoder.submit(m_viewId.value(), BGFX_INVALID_HANDLE, 0, discardFlags);
    }

    void FrameBuffer::SetViewPort(float x, float y, float width, float height)
    {
        m_desiredViewPort = {x, y, width, height};
        SetBgfxViewPortAndScissor(m_desiredViewPort, m_desiredScissor);
    }

    void FrameBuffer::SetScissor(float x, float y, float width, float height)
    {
        m_desiredScissor = GetBgfxScissor(x, y, width, height);
        SetBgfxViewPortAndScissor(m_desiredViewPort, m_desiredScissor);
    }

    void FrameBuffer::Submit(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint8_t flags)
    {
        SetBgfxViewPortAndScissor(m_desiredViewPort, m_desiredScissor);
        encoder.submit(m_viewId.value(), programHandle, 0, flags);
    }

    void FrameBuffer::Blit(bgfx::Encoder& encoder, bgfx::TextureHandle dst, uint16_t dstX, uint16_t dstY, bgfx::TextureHandle src, uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height)
    {
        // In order for Blit to work properly we need to force the creation of a new ViewID.
        SetBgfxViewPortAndScissor(m_desiredViewPort, m_desiredScissor);

        // bgfx blit now takes TextureRegion pairs. UINT16_MAX was the old "whole texture"
        // sentinel; zero width/height now means "rest of mip from x/y".
        const uint16_t regionWidth{width == UINT16_MAX ? static_cast<uint16_t>(0) : width};
        const uint16_t regionHeight{height == UINT16_MAX ? static_cast<uint16_t>(0) : height};

        bgfx::TextureRegion dstRegion{};
        dstRegion.init(dst, dstX, dstY, regionWidth, regionHeight);
        bgfx::TextureRegion srcRegion{};
        srcRegion.init(src, srcX, srcY, regionWidth, regionHeight);
        encoder.blit(m_viewId.value(), dstRegion, srcRegion);
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

    void FrameBuffer::SetBgfxViewPortAndScissor(const Rect& viewPort, const Rect& scissor)
    {
        // A texture initialization clear may have inserted a view since our last draw.
        if (m_viewId.has_value() && m_viewId.value() + 1u == m_deviceContext.PeekNextViewId() &&
            m_viewIdGeneration == m_deviceContext.ViewIdGeneration() &&
            viewPort.Equals(m_bgfxViewPort) && scissor.Equals(m_bgfxScissor))
        {
            return;
        }

        m_viewId = m_deviceContext.AcquireNewViewId();
        m_viewIdGeneration = m_deviceContext.ViewIdGeneration();

        bgfx::setViewMode(m_viewId.value(), bgfx::ViewMode::Sequential);
        bgfx::setViewClear(m_viewId.value(), BGFX_CLEAR_NONE, 0, 1.0f, 0);
        bgfx::setViewFrameBuffer(m_viewId.value(), Handle());

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
