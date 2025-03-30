#include <array>
#include <vector>
#include <stdexcept>

#include <bgfx/bgfx.h>
#include "FrameBufferPool.h"

namespace Babylon::Polyfills
{
    std::vector<FrameBufferPool::PoolBuffer> FrameBufferPool::getPoolBuffers()
    {
        return mPoolBuffers;
    }

    void FrameBufferPool::init(int size, Graphics::DeviceContext& graphicsContext)
    {
        for (int i = 0; i < size; ++i)
        {
            bgfx::FrameBufferHandle TextBuffer{bgfx::kInvalidHandle};
            Graphics::FrameBuffer* FrameBuffer;

            int width(256), height(256);
            std::array<bgfx::TextureHandle, 2> textures{
                bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT),
                bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT)};

            std::array<bgfx::Attachment, textures.size()> attachments{};
            for (size_t idx = 0; idx < attachments.size(); ++idx)
            {
                attachments[idx].init(textures[idx]);
            }
            TextBuffer = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), true);

            FrameBuffer = new Graphics::FrameBuffer(graphicsContext, TextBuffer, width, height, false, false, false);
            mPoolBuffers.push_back({FrameBuffer, true});
        }
    }

    void FrameBufferPool::clear()
    {
        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.frameBuffer)
            {
                // TODO: cleanup framebuffers
                //buffer.frameBuffer->Dispose();
                //free(buffer.frameBuffer);
                delete buffer.frameBuffer;
                buffer.frameBuffer = nullptr;
            }
        }
        mPoolBuffers.clear();
    }

    Graphics::FrameBuffer* FrameBufferPool::acquire()
    {
        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.isAvailable)
            {
                buffer.isAvailable = false;
                return buffer.frameBuffer;
            }
        }
        // TODO: If no buffers available, pool wasn't large enough. Should it automatically resize?
        throw std::runtime_error{"No available frame buffer in pool"};
    }

    void FrameBufferPool::release(Graphics::FrameBuffer* frameBuffer)
    {
        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.frameBuffer == frameBuffer)
            {
                // TODO: clear framebuffer?
                buffer.isAvailable = true;
                return;
            }
        }
    }
}
