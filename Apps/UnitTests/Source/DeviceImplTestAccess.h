#pragma once

#include "../../../Core/Graphics/Source/DeviceImpl.h"

#include <mutex>

namespace Babylon::Graphics
{
    struct DeviceImplTestAccess
    {
        static void LoseDevice(DeviceImpl& device)
        {
            static_cast<bgfx::CallbackI&>(device.m_bgfxCallback).fatal(__FILE__, __LINE__, bgfx::Fatal::DeviceLost, "simulated device loss");
        }

        static void SubmitScreenshots(DeviceImpl& device)
        {
            device.RequestScreenShots();
        }

        static void FlushViews(DeviceImpl& device)
        {
            std::scoped_lock lock{device.m_frameSyncMutex};
            device.PerformMidFrameViewFlush();
        }

        static void SetFrameBlocked(DeviceImpl& device, bool blocked)
        {
            {
                std::scoped_lock lock{device.m_frameSyncMutex};
                device.m_frameBlocked = blocked;
            }
            device.m_frameSyncCV.notify_all();
        }

        static size_t CachedVertexLayouts(DeviceImpl& device)
        {
            std::scoped_lock lock{device.m_frameVertexLayoutsMutex};
            return device.m_frameVertexLayouts.size();
        }

        static size_t PendingReadbacks(const DeviceImpl& device)
        {
            return device.m_readTextureRequests.size();
        }
    };
}
