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
        , m_bound{}
    {
        m_default.Bind();
    }

    FrameBuffer& FrameBufferManager::DefaultFrameBuffer()
    {
        return m_default;
    }

    FrameBuffer& FrameBufferManager::BoundFrameBuffer()
    {
        return *m_bound;
    }

    FrameBuffer& FrameBufferManager::AddFrameBuffer(bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool backBuffer)
    {
        m_frameBuffers.push_back(std::make_unique<FrameBuffer>(*this, handle, width, height, backBuffer));
        return *m_frameBuffers.back();
    }

    void FrameBufferManager::RemoveFrameBuffer(const FrameBuffer& frameBuffer)
    {
        if (m_bound == &frameBuffer)
        {
            m_bound = &m_default;
        }

        auto predicate{[&targetFrameBuffer{frameBuffer}](const std::unique_ptr<FrameBuffer>& frameBuffer) { return frameBuffer.get() == &targetFrameBuffer; }};
        m_frameBuffers.erase(std::find_if(m_frameBuffers.begin(), m_frameBuffers.end(), predicate));
    }

    void FrameBufferManager::BindFrameBuffer(FrameBuffer* frameBuffer)
    {
        m_bound = frameBuffer;
    }

    bgfx::ViewId FrameBufferManager::NewViewId()
    {
        bgfx::ViewId viewId{m_nextViewId++};
        if (viewId >= bgfx::getCaps()->limits.maxViews)
        {
            throw std::runtime_error{"Too many views"};
        }

        return viewId;
    }

    void FrameBufferManager::Reset()
    {
        for (bgfx::ViewId viewId = 0; viewId < m_nextViewId; ++viewId)
        {
            bgfx::resetView(viewId);
        }

        m_nextViewId = 0;

        for (std::unique_ptr<FrameBuffer>& frameBuffer : m_frameBuffers)
        {
            frameBuffer->Reset();
        }

        m_default.Reset();
    }
}
