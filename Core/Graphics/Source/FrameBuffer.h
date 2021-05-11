#pragma once

#include <bgfx/bgfx.h>
#include <optional>

namespace Babylon
{
    class FrameBufferManager;

    class FrameBuffer
    {
    public:
        friend class FrameBufferManager;

        FrameBuffer(FrameBufferManager& manager, bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool defaultBackBuffer);
        ~FrameBuffer();

        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer(FrameBuffer&&) = delete;

        bgfx::FrameBufferHandle Handle() const;
        uint16_t Width() const;
        uint16_t Height() const;
        bool DefaultBackBuffer() const;

        void Clear(bgfx::Encoder* encoder, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
        void SetViewPort(bgfx::Encoder* encoder, float x, float y, float width, float height);
        void Submit(bgfx::Encoder* encoder, bgfx::ProgramHandle programHandle, uint8_t flags);
        void SetStencil(bgfx::Encoder* encoder, uint32_t stencilState);

    private:
        struct ViewPort
        {
            float X{0.0f};
            float Y{0.0f};
            float Width{1.0f};
            float Height{1.0f};

            bool Equals(const ViewPort& other) const;
        };

        template<bool doTouch> void NewView(bgfx::Encoder* encoder, const ViewPort& viewPort);
        void Reset();

        FrameBufferManager& m_manager;
        bgfx::FrameBufferHandle m_handle;
        const uint16_t m_width;
        const uint16_t m_height;
        const bool m_defaultBackBuffer;
        std::optional<bgfx::ViewId> m_viewId;
        ViewPort m_viewPort;
        std::optional<ViewPort> m_requestedViewPort;
    };
}
