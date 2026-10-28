#include <array>
#include <cstring>
#include <vector>
#include <stdexcept>

#include <bgfx/bgfx.h>
#include "CanvasConstants.h"
#include "FrameBufferPool.h"

namespace Babylon::Polyfills
{
    namespace
    {
        constexpr uint8_t FRAME_BUFFER_TEXTURE_COUNT = 2;
        constexpr uint8_t COLOR_ATTACHMENT_INDEX = 0;
        constexpr uint8_t DEPTH_STENCIL_ATTACHMENT_INDEX = 1;
    }

    const std::vector<FrameBufferPool::PoolBuffer>& FrameBufferPool::GetPoolBuffers()
    {
        return mPoolBuffers;
    }

    void FrameBufferPool::SetDimensions(int width, int height)
    {
        assert(width > 0 && height > 0);
        if (!mPoolBuffers.empty())
        {
            throw std::runtime_error("Cannot set dimensions. FrameBufferPool already has buffers.");
        }

        m_width = width;
        m_height = height;
    }

    void FrameBufferPool::SetGraphicsContext(Graphics::DeviceContext* graphicsContext)
    {
        m_graphicsContext = graphicsContext;
    }

    void FrameBufferPool::Add(int nBuffers)
    {
        if (m_graphicsContext == nullptr)
        {
            throw std::runtime_error("Cannot add framebuffer to pool. Graphics context is not set.");
        }

        for (int i = 0; i < nBuffers; ++i)
        {
            bgfx::FrameBufferHandle TextBuffer{bgfx::kInvalidHandle};
            Graphics::FrameBuffer* FrameBuffer;

            // WebGL requires new render targets to be initialized to zero.
            const bgfx::Memory* mem = bgfx::alloc(static_cast<uint32_t>(m_width) * static_cast<uint32_t>(m_height) * CanvasConstants::RGBA_BYTES_PER_PIXEL);
            std::memset(mem->data, 0, mem->size);
            // Browser blur behavior includes transparent edges from border sampling.
            std::array<bgfx::TextureHandle, FRAME_BUFFER_TEXTURE_COUNT> textures{
                bgfx::createTexture2D(m_width, m_height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | BGFX_SAMPLER_U_BORDER | BGFX_SAMPLER_V_BORDER | BGFX_SAMPLER_BORDER_COLOR(0), mem),
                bgfx::createTexture2D(m_width, m_height, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT | BGFX_SAMPLER_U_BORDER | BGFX_SAMPLER_V_BORDER | BGFX_SAMPLER_BORDER_COLOR(0))};

            // Only request mip generation for formats that support it; depth never does.
            const bgfx::Caps* caps = bgfx::getCaps();
            const uint8_t colorResolve = 0 != (caps->formats[bgfx::TextureFormat::RGBA8] & BGFX_CAPS_FORMAT_TEXTURE_MIP_AUTOGEN)
                ? BGFX_ATTACHMENT_AUTO_GEN_MIPS
                : BGFX_ATTACHMENT_NONE;

            std::array<bgfx::Attachment, textures.size()> attachments{};
            attachments[COLOR_ATTACHMENT_INDEX].init(textures[COLOR_ATTACHMENT_INDEX], bgfx::Access::Write, 0, 1, 0, colorResolve);
            attachments[DEPTH_STENCIL_ATTACHMENT_INDEX].init(textures[DEPTH_STENCIL_ATTACHMENT_INDEX], bgfx::Access::Write, 0, 1, 0, BGFX_ATTACHMENT_NONE);
            TextBuffer = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), true);

            if (!bgfx::isValid(TextBuffer))
            {
                // Failed texture creation returns an invalid handle that cannot be destroyed.
                for (auto texture : textures)
                {
                    if (bgfx::isValid(texture))
                    {
                        bgfx::destroy(texture);
                    }
                }
                throw std::runtime_error{"FrameBufferPool::Add: bgfx::createFrameBuffer returned invalid handle (pool exhausted)"};
            }

            FrameBuffer = new Graphics::FrameBuffer(
                *m_graphicsContext, TextBuffer, m_width, m_height, false, false, false,
                CanvasConstants::NO_DEPTH_STENCIL_ATTACHMENT, false);
            m_available++;
            mPoolBuffers.push_back({FrameBuffer, true});
        }
    }

    void FrameBufferPool::Clear()
    {
        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.frameBuffer)
            {
                buffer.frameBuffer->Dispose();
                delete buffer.frameBuffer;
            }
        }
        m_available = 0;
        mPoolBuffers.clear();
    }

    Graphics::FrameBuffer* FrameBufferPool::Acquire()
    {
        if (m_available == 0)
        {
            Add(1);
        }

        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.isAvailable)
            {
                buffer.isAvailable = false;
                m_available--;
                return buffer.frameBuffer;
            }
        }

        throw std::runtime_error("No available frame buffer in pool.");
    }

    void FrameBufferPool::Release(Graphics::FrameBuffer* frameBuffer)
    {
        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.frameBuffer == frameBuffer)
            {
                buffer.isAvailable = true;
                m_available++;
                return;
            }
        }
    }
}
