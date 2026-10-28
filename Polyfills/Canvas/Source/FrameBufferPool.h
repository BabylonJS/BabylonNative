#pragma once

#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>

namespace Babylon::Polyfills
{
    class FrameBufferPool final
    {
    public:
        struct PoolBuffer
        {
            Graphics::FrameBuffer* frameBuffer;
            bool isAvailable;
        };
        Graphics::FrameBuffer* Acquire();
        void Add(int nBuffers);
        void Clear();
        void Release(Graphics::FrameBuffer* frameBuffer);
        void SetDimensions(int width, int height);
        void SetGraphicsContext(Graphics::DeviceContext *graphicsContext);
        const std::vector<PoolBuffer>& GetPoolBuffers();

    private:
        std::vector<PoolBuffer> mPoolBuffers{};
        Graphics::DeviceContext* m_graphicsContext;
        int m_available{0};
        int m_width{256};
        int m_height{256};
    };
}
