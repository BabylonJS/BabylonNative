#include <Babylon/Graphics.h>
#include "../GraphicsImpl.h"

namespace Babylon
{
    float Graphics::Impl::UpdateDevicePixelRatio()
    {
        // In windows, 100% DPI scaling is 96dpi.
        // See https://docs.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels

        // In UWP, DisplayInformation is only accessible from the UI thread.
        // See https://docs.microsoft.com/en-us/uwp/api/windows.graphics.display.displayinformation.getforcurrentview?view=winrt-19041#Windows_Graphics_Display_DisplayInformation_GetForCurrentView

        // TODO: We need to pass a Windows::UI::Core::CoreDispatcher to the graphics component so that we can safely make UI calls.
        // See https://github.com/BabylonJS/BabylonNative/issues/625
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.DevicePixelRatio;
    }
}