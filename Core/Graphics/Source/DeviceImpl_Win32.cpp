#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/DeviceQueries.h>
#include "DeviceImpl.h"
#include <Windows.h>

namespace Babylon::Graphics
{
    void DeviceImpl::ConfigureBgfxSwapChain(bgfx::SwapChain& swapChain, WindowT window)
    {
        swapChain.nwh = window;
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::Init& /*init*/)
    {
    }

    float GetDevicePixelRatio(WindowT window)
    {
        UINT dpi = GetDpiForWindow(window);

        // 100% DPI scaling is 96dpi.
        // See https://docs.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels
        return static_cast<float>(dpi) / 96.0f;
    }
}
