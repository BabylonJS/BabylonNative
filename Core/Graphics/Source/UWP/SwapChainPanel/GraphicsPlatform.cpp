#include <Babylon/Graphics.h>
#include <Babylon/GraphicsPlatform.h>
#include "../GraphicsImpl.h"

#include <winrt/Windows.Graphics.Display.h>

using namespace winrt;
using namespace Windows::Graphics::Display;

namespace Babylon
{
    void GraphicsImpl::ConfigureBgfxPlatformData(const Graphics::Configuration& config, bgfx::PlatformData& pd)
    {
        // Use windowTypePtr == 2 for xaml swap chain panels
        pd.ndt = reinterpret_cast<void*>(2);
        pd.nwh = config.WindowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
    }

    float GraphicsImpl::UpdateDevicePixelRatio()
    {
        // In UWP, DisplayInformation is only accessible from the UI thread. Calling this method from any other thread will cause a crash.
        // See https://docs.microsoft.com/en-us/uwp/api/windows.graphics.display.displayinformation.getforcurrentview?view=winrt-19041#Windows_Graphics_Display_DisplayInformation_GetForCurrentView

        std::scoped_lock lock{m_state.Mutex};

        // See: https://docs.microsoft.com/en-us/uwp/api/windows.graphics.display.resolutionscale?view=winrt-19041#fields
        m_state.Resolution.DevicePixelRatio = static_cast<float>(DisplayInformation::GetForCurrentView().ResolutionScale())/100;
        return m_state.Resolution.DevicePixelRatio;
    }
}