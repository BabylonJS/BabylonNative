#include "FrameBufferManager.h"
#include "FrameBuffer.h"
#include <algorithm>
#include <stdexcept>

namespace Babylon
{
    FrameBufferManager::FrameBufferManager()
        : m_nextViewId{}
        , m_frameBuffers{}
        , m_default{*this, BGFX_INVALID_HANDLE, 0, 0, true}
    {
    }

    FrameBuffer& FrameBufferManager::DefaultFrameBuffer()
    {
        return m_default;
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
}
