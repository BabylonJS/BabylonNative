#include <Babylon/Graphics.h>
#include <Babylon/GraphicsPlatform.h>
#include "../GraphicsImpl.h"

#include <winrt/Windows.Graphics.Display.h>

using namespace winrt;
using namespace Windows::Graphics::Display;

namespace Babylon
{
    void Graphics::Impl::ConfigureBgfxPlatformData(GraphicsConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.ndt = config.windowTypePtr;
        pd.nwh = config.windowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
    }

    float Graphics::Impl::UpdateDevicePixelRatio()
    {
        // In UWP, DisplayInformation is only accessible from the UI thread. Calling this method from any other thread will cause a crash.
        // See https://docs.microsoft.com/en-us/uwp/api/windows.graphics.display.displayinformation.getforcurrentview?view=winrt-19041#Windows_Graphics_Display_DisplayInformation_GetForCurrentView

        std::scoped_lock lock{m_state.Mutex};

        // See: https://docs.microsoft.com/en-us/uwp/api/windows.graphics.display.resolutionscale?view=winrt-19041#fields
        m_state.Resolution.DevicePixelRatio = static_cast<float>(DisplayInformation::GetForCurrentView().ResolutionScale())/100;
        return m_state.Resolution.DevicePixelRatio;
    }
}