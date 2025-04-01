#include <array>
#include <vector>
#include <stdexcept>

#include <bgfx/bgfx.h>
#include "FrameBufferPool.h"

namespace Babylon::Polyfills
{
    const std::vector<FrameBufferPool::PoolBuffer>& FrameBufferPool::GetPoolBuffers()
    {
        return mPoolBuffers;
    }

    // sets graphics context to be used for creating framebuffers
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

            int width(256), height(256);
            std::array<bgfx::TextureHandle, 1> textures{
                bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT)};

            std::array<bgfx::Attachment, textures.size()> attachments{};
            for (size_t idx = 0; idx < attachments.size(); ++idx)
            {
                attachments[idx].init(textures[idx]);
            }
            TextBuffer = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), true);

            FrameBuffer = new Graphics::FrameBuffer(*m_graphicsContext, TextBuffer, width, height, false, false, false);
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
                if (buffer.frameBuffer->Handle().idx != bgfx::kInvalidHandle)
                {
                    bgfx::destroy(buffer.frameBuffer->Handle());
                }
            }
        }
        m_available = 0;
        mPoolBuffers.clear();
    }

    Graphics::FrameBuffer* FrameBufferPool::Acquire()
    {
        // no buffers in pool, add one
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
    }

    void FrameBufferPool::Release(Graphics::FrameBuffer* frameBuffer)
    {
        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.frameBuffer == frameBuffer)
            {
                // no need to clear framebuffer, as all filter passes are (currently) full screen
                buffer.isAvailable = true;
                return;
            }
        }
    }
}
