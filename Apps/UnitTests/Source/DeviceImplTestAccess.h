#pragma once

#include "../../../Core/Graphics/Source/DeviceImpl.h"

#include <functional>
#include <mutex>
#include <utility>

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

#ifdef BABYLON_NATIVE_GRAPHICS_TEST_HOOKS
        static void SetFrameScopeHooks(
            DeviceImpl& device,
            std::function<void(std::unique_lock<std::mutex>&)> queued,
            std::function<void()> closing)
        {
            device.m_frameScopeQueuedTestHook = std::move(queued);
            device.m_frameClosingTestHook = std::move(closing);
        }

        static int WaitingFrameScopes(const DeviceImpl& device)
        {
            // Called by a hook while the frame synchronization mutex is held.
            return device.m_waitingFrameScopes;
        }

        static bool FrameScopesDrained(DeviceImpl& device)
        {
            std::scoped_lock lock{device.m_frameSyncMutex};
            return device.m_pendingFrameScopes == 0 && device.m_waitingFrameScopes == 0;
        }
#endif
    };
}
