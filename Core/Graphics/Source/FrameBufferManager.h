#pragma once

#include "FrameBuffer.h"
#include <list>
#include <memory>
#include <mutex>
#include <vector>

namespace Babylon
{
    class FrameBufferManager final
    {
    public:
        FrameBufferManager();
        FrameBufferManager(const FrameBufferManager&) = delete;
        FrameBufferManager(FrameBufferManager&&) = delete;

        FrameBuffer& DefaultFrameBuffer();

        FrameBuffer& AddFrameBuffer(bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool backBuffer);
        void RemoveFrameBuffer(const FrameBuffer& frameBuffer);

        bgfx::ViewId NewViewId();
        void Reset();

        // Draw calls can happen independently of NewViewId calls.
        // For example, clear on backbuffer then draw calls on renderTarget and then drawcalls on backbuffer
        // To preserve drawcalls orders, we have to keep track of touched views and draw views in that 
        // particular order.
        void AppendTouchedView(bgfx::ViewId viewId);
        void ClearTouchedViews() { m_touchedViews.clear(); }
        const std::vector<bgfx::ViewId> GetTouchedViews() const { return m_touchedViews; }

    private:
        void ResetViewId();
        void ResetFrameBuffers();

        std::mutex m_viewIdMutex{};
        bgfx::ViewId m_nextViewId{};

        std::mutex m_frameBuffersMutex{};
        std::list<FrameBuffer> m_frameBuffers{};

        std::vector<bgfx::ViewId> m_touchedViews;

        FrameBuffer m_default;
    };
}
