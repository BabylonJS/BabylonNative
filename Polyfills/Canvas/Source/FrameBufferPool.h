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
        void Clear();
        void Init(int nBuffers, Graphics::DeviceContext& graphicsContext);
        void Release(Graphics::FrameBuffer* frameBuffer);
        const std::vector<PoolBuffer>& GetPoolBuffers();

    private:
        std::vector<PoolBuffer> mPoolBuffers{};
    };
}
