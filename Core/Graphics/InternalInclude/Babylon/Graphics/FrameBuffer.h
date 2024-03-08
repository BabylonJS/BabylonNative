#pragma once

#include <bgfx/bgfx.h>
#include <optional>

namespace Babylon::Graphics
{
    class DeviceContext;

    struct Rect
    {
        float X{};
        float Y{};
        float Width{};
        float Height{};

        bool Equals(const Rect& other) const;
    };

    class FrameBuffer final
    {
    public:
        FrameBuffer(DeviceContext& context, bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool defaultBackBuffer, bool hasDepth, bool hasStencil);
        ~FrameBuffer();

        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;

        void Dispose();

        bgfx::FrameBufferHandle Handle() const;
        uint16_t Width() const;
        uint16_t Height() const;
        bool DefaultBackBuffer() const;

        void Bind(bgfx::Encoder& encoder);
        void Unbind(bgfx::Encoder& encoder);

        void Clear(bgfx::Encoder& encoder, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
        void SetViewPort(bgfx::Encoder& encoder, float x, float y, float width, float height);
        void SetScissor(bgfx::Encoder& encoder, float x, float y, float width, float height);
        void Submit(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint8_t flags);
        void SetStencil(bgfx::Encoder& encoder, uint32_t stencilState);
        void Blit(bgfx::Encoder& encoder, bgfx::TextureHandle _dst, uint16_t _dstX, uint16_t _dstY, bgfx::TextureHandle _src, uint16_t _srcX = 0, uint16_t _srcY = 0, uint16_t _width = UINT16_MAX, uint16_t _height = UINT16_MAX);

        bool HasDepth() const { return m_hasDepth; }
        bool HasStencil() const { return m_hasStencil; }

    private:
        Rect GetBgfxScissor(float x, float y, float width, float height) const;
        void SetBgfxViewPortAndScissor(bgfx::Encoder& encoder, const Rect& viewPort, const Rect& scissor);

        DeviceContext& m_deviceContext;
        const uintptr_t m_deviceID{};

        bgfx::FrameBufferHandle m_handle{};
        const uint16_t m_width{};
        const uint16_t m_height{};
        const bool m_defaultBackBuffer{};
        const bool m_hasDepth{};
        const bool m_hasStencil{};

        std::optional<bgfx::ViewId> m_viewId{};

        Rect m_bgfxViewPort{0.0f, 0.0f, 1.0f, 1.0f};
        Rect m_desiredViewPort{0.0f, 0.0f, 1.0f, 1.0f};

        Rect m_bgfxScissor{};
        Rect m_desiredScissor{};

        bool m_disposed{};
    };
}
