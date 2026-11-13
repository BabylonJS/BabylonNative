#pragma once

#include <bgfx/bgfx.h>
#include <memory>
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

    struct DepthStencilTexture
    {
        bgfx::TextureHandle Handle{bgfx::kInvalidHandle};
        bgfx::TextureFormat::Enum Format{bgfx::TextureFormat::Unknown};
        uint64_t Flags{};
        uint16_t NumLayers{1};
    };

    // Depth/stencil generated for a render target. Every framebuffer of the target (one per cube face or array
    // layer) attaches the same one, like WebGL attaches a single depth renderbuffer to all faces of a target.
    // It is destroyed with the last framebuffer that references it, unless it was released.
    class GeneratedDepthStencil final
    {
    public:
        GeneratedDepthStencil(DeviceContext& deviceContext, bgfx::TextureHandle handle, uint16_t width, uint16_t height, bgfx::TextureFormat::Enum format, uint64_t flags, uint16_t numLayers);
        ~GeneratedDepthStencil();

        GeneratedDepthStencil(const GeneratedDepthStencil&) = delete;
        GeneratedDepthStencil& operator=(const GeneratedDepthStencil&) = delete;

        bgfx::TextureHandle Handle() const { return m_texture.Handle; }

        // Whether a framebuffer of the given size and depth/stencil description can attach this one.
        bool Matches(uint16_t width, uint16_t height, bgfx::TextureFormat::Enum format, uint64_t flags, uint16_t numLayers) const;

        // Hands the texture over; the caller becomes responsible for destroying it. Framebuffers keep rendering to it.
        DepthStencilTexture Release();

    private:
        DeviceContext& m_deviceContext;
        const uintptr_t m_deviceID{};
        const DepthStencilTexture m_texture{};
        const uint16_t m_width{};
        const uint16_t m_height{};
        bool m_released{};
    };

    class FrameBuffer final
    {
    public:
        FrameBuffer(DeviceContext& context, bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool defaultBackBuffer, bool hasDepth, bool hasStencil, int8_t depthStencilAttachmentIndex = -1, bool isMultisampled = false, uint8_t depthOneVolumeAttachmentMask = 0);
        ~FrameBuffer();

        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;

        void Dispose();

        bgfx::FrameBufferHandle Handle() const;
        uint16_t Width() const;
        uint16_t Height() const;
        bool DefaultBackBuffer() const;
        bool IsMultisampled() const;

        void Bind();
        void Unbind();

        // Floating-point colors preserve HDR values through bgfx's clear palette.
        // More than 16 distinct palette colors in a physical frame throws rather than overwriting pending clears.
        // ignoreScissor clears the whole framebuffer regardless of the current scissor.
        void Clear(bgfx::Encoder& encoder, uint16_t flags, float r, float g, float b, float a, float depth, uint8_t stencil, uint8_t colorAttachmentMask = UINT8_MAX, bool ignoreScissor = false);
        void SetViewPort(float x, float y, float width, float height);
        void SetScissor(float x, float y, float width, float height);
        // Adopts a scissor without starting a view, for the next draw after Bind. WebGL scissor state is
        // global rather than per framebuffer, so a newly bound framebuffer inherits the current one.
        void InheritScissor(float x, float y, float width, float height);
        void Submit(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint8_t flags);
        void Compute(bgfx::Encoder& encoder, bgfx::ProgramHandle programHandle, uint32_t numX, uint32_t numY, uint32_t numZ);
        void SetStencil(bgfx::Encoder& encoder, uint32_t stencilState);
        void Blit(bgfx::Encoder& encoder, bgfx::TextureHandle dst, uint16_t dstX, uint16_t dstY, bgfx::TextureHandle src, uint16_t srcX = 0, uint16_t srcY = 0, uint16_t width = UINT16_MAX, uint16_t height = UINT16_MAX);

        bool HasDepth() const { return m_hasDepth; }
        bool HasStencil() const { return m_hasStencil; }

        // The generated depth/stencil this framebuffer attaches, possibly together with the other faces or layers
        // of its render target.
        void SetGeneratedDepthStencil(std::shared_ptr<GeneratedDepthStencil> depthStencil);

        // Hands over the generated depth/stencil this framebuffer attaches; the caller becomes responsible for
        // destroying it. This and the other framebuffers of the render target keep rendering to it. Returns an
        // invalid handle when the framebuffer has no generated depth/stencil or it was already handed over.
        DepthStencilTexture ReleaseDepthStencil();

        // Rendered upside down so rows are stored in GL order on a top-left-origin backend.
        // Viewport and scissor rects are mirrored to match (see RENDER_TARGET_TRANSFORM_UNIFORM_NAME).
        void SetGLRowOrder(bool glRowOrder) { m_glRowOrder = glRowOrder; }
        bool GLRowOrder() const { return m_glRowOrder; }

    private:
        Rect ToTargetRect(const Rect& rect, float height) const;
        Rect GetBgfxScissor(float x, float y, float width, float height) const;
        void SetBgfxViewPortAndScissor(const Rect& viewPort, const Rect& scissor);

        DeviceContext& m_deviceContext;
        const uintptr_t m_deviceID{};

        bgfx::FrameBufferHandle m_handle{};
        const uint16_t m_width{};
        const uint16_t m_height{};
        const bool m_defaultBackBuffer{};
        const bool m_useDeviceBackBuffer{};
        const bool m_hasDepth{};
        const bool m_hasStencil{};
        const bool m_isMultisampled{};
        const uint8_t m_depthOneVolumeAttachmentMask{};

        std::optional<bgfx::ViewId> m_viewId{};

        // Generation that m_viewId was acquired in. A mid-frame view flush resets the device's
        // view counter, which makes a retained id sort after freshly acquired ones; comparing
        // against DeviceContext::ViewIdGeneration() detects that and forces a re-acquire.
        uint32_t m_viewIdGeneration{0};

        Rect m_bgfxViewPort{0.0f, 0.0f, 1.0f, 1.0f};
        Rect m_desiredViewPort{0.0f, 0.0f, 1.0f, 1.0f};

        Rect m_bgfxScissor{};
        Rect m_desiredScissor{};

        bool m_disposed{};
        bool m_glRowOrder{};
        int8_t m_depthStencilAttachmentIndex{-1};
        std::shared_ptr<GeneratedDepthStencil> m_generatedDepthStencil{};
    };
}
