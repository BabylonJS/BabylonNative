#include <array>
#include <vector>
#include <stdexcept>

#include <bimg/bimg.h>
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
            int width(512), height(512);

            // make sure render targets are filled with 0 : https://registry.khronos.org/webgl/specs/latest/1.0/#TEXIMAGE2D
            bgfx::ReleaseFn releaseFn{[](void*, void* userData) {
                bimg::imageFree(static_cast<bimg::ImageContainer*>(userData));
            }};

            bimg::ImageContainer* image = bimg::imageAlloc(&Babylon::Graphics::DeviceContext::GetDefaultAllocator(), bimg::TextureFormat::RGBA8, width, height, 1 /*depth*/, 1, false /*cubeMap*/, false /*hasMips*/);
            const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size, releaseFn, image);
            bx::memSet(image->m_data, 0, image->m_size);
            // TODO: move sampler flags to nanovg_babylon.cpp
            std::array<bgfx::TextureHandle, 2> textures{
                bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | BGFX_SAMPLER_UVW_MIRROR , mem),
                bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT | BGFX_SAMPLER_UVW_MIRROR)};

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
                buffer.frameBuffer->Dispose();
                delete buffer.frameBuffer;
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
