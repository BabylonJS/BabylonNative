#pragma once

#include "FrameBuffer.h"
#include <list>
#include <memory>
#include <mutex>

namespace Babylon
{
    class FrameBufferManager final
    {
    public:
        FrameBufferManager();
        FrameBufferManager(const FrameBufferManager&) = delete;
        FrameBufferManager(FrameBufferManager&&) = delete;

        FrameBuffer& DefaultFrameBuffer();
        FrameBuffer& FinalFrameBuffer();

        FrameBuffer& AddFrameBuffer(bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool backBuffer);
        void RemoveFrameBuffer(const FrameBuffer& frameBuffer);

        bgfx::ViewId NewViewId();
        void Reset();

        void Resize(uint16_t width, uint16_t height);
    private:
        void ResetViewId();
        void ResetFrameBuffers();

        std::mutex m_viewIdMutex{};
        bgfx::ViewId m_nextViewId{};

        std::mutex m_frameBuffersMutex{};
        std::list<FrameBuffer> m_frameBuffers{};

        FrameBuffer m_default;
        FrameBuffer m_backBuffer;
    };
}
