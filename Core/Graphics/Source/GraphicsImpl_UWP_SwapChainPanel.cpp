#include <Babylon/GraphicsPlatform.h>
#include "GraphicsImpl.h"
#include <winrt/Windows.Graphics.Display.h>

namespace Babylon
{
    const bool GraphicsImpl::s_bgfxFlipAfterRender = false;

    void GraphicsImpl::ConfigureBgfxPlatformData(const WindowConfiguration& config, bgfx::PlatformData& pd)
    {
        // Use windowTypePtr == 2 for xaml swap chain panels
        pd.ndt = reinterpret_cast<void*>(2);
        pd.nwh = winrt::get_abi(config.Window);
    }

    float GraphicsImpl::GetDevicePixelRatio(const WindowConfiguration&)
    {
        // In UWP, DisplayInformation is only accessible from the UI thread. Calling this method from any other thread will cause a crash.
        // See https://docs.microsoft.com/en-us/uwp/api/windows.graphics.display.displayinformation.getforcurrentview?view=winrt-19041#Windows_Graphics_Display_DisplayInformation_GetForCurrentView
        // See: https://docs.microsoft.com/en-us/uwp/api/windows.graphics.display.resolutionscale?view=winrt-19041#fields
        return static_cast<float>(winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView().ResolutionScale()) / 100.0f;
    }
}