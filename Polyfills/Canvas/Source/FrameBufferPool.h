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
        void init(int nBuffers, Graphics::DeviceContext& graphicsContext);
        void clear();
        Graphics::FrameBuffer* acquire();
        void release(Graphics::FrameBuffer* frameBuffer);
        std::vector<PoolBuffer> getPoolBuffers();

        typename std::vector<Graphics::FrameBuffer*>::iterator begin();
        typename std::vector<Graphics::FrameBuffer*>::iterator end();
        typename std::vector<Graphics::FrameBuffer*>::const_iterator begin() const;
        typename std::vector<Graphics::FrameBuffer*>::const_iterator end() const;

    private:
        std::vector<PoolBuffer> mPoolBuffers{};
    };
}
