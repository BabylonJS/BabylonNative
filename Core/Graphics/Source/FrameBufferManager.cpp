#include "FrameBufferManager.h"
#include "FrameBuffer.h"
#include <algorithm>
#include <stdexcept>
#include <array>

namespace Babylon
{
    FrameBufferManager::FrameBufferManager()
        : m_nextViewId{}
        , m_frameBuffers{}
        , m_default{*this, BGFX_INVALID_HANDLE, 0, 0, true}
        , m_backBuffer{*this, BGFX_INVALID_HANDLE, 0, 0, true}
    {
        Resize(bgfx::getStats()->width, bgfx::getStats()->height);
    }

    FrameBuffer& FrameBufferManager::DefaultFrameBuffer()
    {
        return m_default;
    }

    FrameBuffer& FrameBufferManager::FinalFrameBuffer()
    {
        return m_backBuffer;
    }

    FrameBuffer& FrameBufferManager::AddFrameBuffer(bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool backBuffer)
    {
        std::scoped_lock lock{m_frameBuffersMutex};
        m_frameBuffers.emplace_back(*this, handle, width, height, backBuffer);
        return m_frameBuffers.back();
    }

    void FrameBufferManager::RemoveFrameBuffer(const FrameBuffer& frameBuffer)
    {
        std::scoped_lock lock{m_frameBuffersMutex};
        const auto predicate{[&frameBuffer](const FrameBuffer& currentFrameBuffer) { return &currentFrameBuffer == &frameBuffer; }};
        m_frameBuffers.erase(std::find_if(m_frameBuffers.begin(), m_frameBuffers.end(), predicate));
    }

    bgfx::ViewId FrameBufferManager::NewViewId()
    {
        std::scoped_lock lock{m_viewIdMutex};

        bgfx::ViewId viewId{m_nextViewId++};
        if (viewId >= bgfx::getCaps()->limits.maxViews)
        {
            throw std::runtime_error{"Too many views"};
        }

        return viewId;
    }

    void FrameBufferManager::Reset()
    {
        ResetViewId();
        ResetFrameBuffers();
    }

    void FrameBufferManager::ResetViewId()
    {
        std::scoped_lock lock{m_viewIdMutex};

        for (bgfx::ViewId viewId = 0; viewId < m_nextViewId; ++viewId)
        {
            bgfx::resetView(viewId);
        }

        m_nextViewId = 0;
    }

    void FrameBufferManager::ResetFrameBuffers()
    {
        std::scoped_lock lock{m_frameBuffersMutex};

        for (FrameBuffer& frameBuffer : m_frameBuffers)
        {
            frameBuffer.Reset();
        }

        m_default.Reset();
    }

    void FrameBufferManager::AppendTouchedView(bgfx::ViewId viewId)
    {
        if (m_touchedViews.empty() || m_touchedViews.back() != viewId)
        {
            m_touchedViews.push_back(viewId);
        }
    }

    void FrameBufferManager::Resize(uint16_t width, uint16_t height)
    {
        if (bgfx::getCaps()->originBottomLeft)
        {
            // no resize needed for originBottomLeft (OpenGL)
            return;
        }
        // resize the render target used as back buffer
        
        auto previousHandle = m_default.Handle();
        if (bgfx::isValid(previousHandle))
        {
            bgfx::destroy(previousHandle);
        }

        std::array<bgfx::TextureHandle, 2> textures{
            bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT),
            bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT) };
        std::array<bgfx::Attachment, textures.size()> attachments{};
        for (size_t idx = 0; idx < attachments.size(); ++idx)
        {
            attachments[idx].init(textures[idx]);
        }
        m_default.m_handle = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), true);
    }
}
