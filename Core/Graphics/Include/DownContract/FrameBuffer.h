#pragma once

#include <bgfx/bgfx.h>
#include <optional>

namespace Babylon
{
    class GraphicsContext;

    struct ViewPort
    {
        float X{0.0f};
        float Y{0.0f};
        float Width{1.0f};
        float Height{1.0f};

        bool Equals(const ViewPort& other) const;
    };

    class FrameBuffer final
    {
    public:
        FrameBuffer(GraphicsContext& graphicsContext, bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool defaultBackBuffer, bool hasDepth, bool hasStencil);
        ~FrameBuffer();

        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer(FrameBuffer&&) = delete;

        bgfx::FrameBufferHandle Handle() const;
        uint16_t Width() const;
        uint16_t Height() const;
        bool DefaultBackBuffer() const;

        void Bind(bgfx::Encoder& encoder);
        void Unbind(bgfx::Encoder& encoder);

        void Clear(bgfx::Encoder& encoder, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
        void SetViewPort(bgfx::Encoder& encoder, float x, float y, float width, float height);
        void Submit(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint8_t flags);
        void SetStencil(bgfx::Encoder& encoder, uint32_t stencilState);
        void Blit(bgfx::Encoder& encoder, bgfx::TextureHandle _dst, uint16_t _dstX, uint16_t _dstY, bgfx::TextureHandle _src, uint16_t _srcX = 0, uint16_t _srcY = 0, uint16_t _width = UINT16_MAX, uint16_t _height = UINT16_MAX);

        bool HasDepth() const { return m_hasDepth; }
        bool HasStencil() const { return m_hasStencil; }

        void Dispose();

    private:

        GraphicsContext& m_context;
        bgfx::FrameBufferHandle m_handle;
        const uint16_t m_width;
        const uint16_t m_height;
        const bool m_defaultBackBuffer;
        const bool m_hasDepth;
        const bool m_hasStencil;

        bgfx::ViewId m_viewId{};
        ViewPort m_viewPort{};
        bool m_hasViewIdBeenUsed{false};

        bool m_disposed{false};
    };
}
