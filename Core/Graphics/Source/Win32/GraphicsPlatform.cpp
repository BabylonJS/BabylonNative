#include <Windows.h>

#include <Babylon/Graphics.h>
#include "../GraphicsImpl.h"

namespace Babylon
{
    void GraphicsImpl::ConfigureBgfxPlatformData(GraphicsConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.ndt = nullptr;
        pd.nwh = config.windowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
    }

    float GraphicsImpl::UpdateDevicePixelRatio()
    {
        UINT dpi{GetDpiForWindow(GetNativeWindow())};

        // In windows, 100% DPI scaling is 96dpi.
        // See https://docs.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels
        std::scoped_lock lock{m_state.Mutex};
        m_state.Resolution.DevicePixelRatio = static_cast<float>(dpi) / 96.0f;
        return m_state.Resolution.DevicePixelRatio;
    }
}
