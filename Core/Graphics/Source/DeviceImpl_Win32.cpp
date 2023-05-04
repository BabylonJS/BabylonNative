#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"
#include <Windows.h>

namespace Babylon::Graphics
{
    const bool DeviceImpl::s_bgfxFlipAfterRender = false;

    void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window)
    {
        pd.nwh = window;
    }

    float DeviceImpl::GetDevicePixelRatio(WindowT window)
    {
        UINT dpi = GetDpiForWindow(window);

        // 100% DPI scaling is 96dpi.
        // See https://docs.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels
        return static_cast<float>(dpi) / 96.0f;
    }
}
