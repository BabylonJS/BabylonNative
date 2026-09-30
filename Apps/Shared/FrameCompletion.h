#pragma once

#include <Babylon/Graphics/Device.h>

#include <chrono>
#include <future>

namespace Babylon::Apps
{
    // Frame-thread only, with a frame already open. Returns with the frame closed.
    inline void FinishRenderingWhenReady(Graphics::Device& device, std::future<void> completion)
    {
        while (completion.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready)
        {
            device.FinishRenderingCurrentFrame();
            device.StartRenderingCurrentFrame();
        }
        device.FinishRenderingCurrentFrame();
        completion.get();
    }
}
