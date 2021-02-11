#pragma once

#include "FrameBuffer.h"
#include <vector>
#include <list>
#include <memory>

namespace Babylon
{
    class FrameBufferManager final
    {
    public:
        FrameBufferManager();
        FrameBufferManager(const FrameBufferManager&) = delete;
        FrameBufferManager(FrameBufferManager&&) = delete;

        FrameBuffer& DefaultFrameBuffer();
        FrameBuffer& BoundFrameBuffer();

        FrameBuffer& AddFrameBuffer(bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool backBuffer);
        void RemoveFrameBuffer(const FrameBuffer& frameBuffer);
        void BindFrameBuffer(FrameBuffer* frameBuffer);

        bgfx::ViewId NewViewId();
        void Reset();

    private:
        struct ViewInfo
        {
            bool ClearCalled{false};
            bool SubmitCalled{false};
        };

        bgfx::ViewId m_nextViewId;
        std::list<std::unique_ptr<FrameBuffer>> m_frameBuffers;
        FrameBuffer m_default;
        FrameBuffer* m_bound;
    };
}
