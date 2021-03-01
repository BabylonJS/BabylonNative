#include <Babylon/Graphics.h>
#include "../GraphicsImpl.h"

#include <winrt/Windows.Graphics.Display.h>

using namespace winrt::Windows::Graphics::Display;

namespace Babylon
{
    float Graphics::Impl::UpdateDevicePixelRatio()
    {
        // In windows, 100% DPI scaling is 96dpi.
        // See https://docs.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels
        std::scoped_lock lock{m_state.Mutex};
        m_state.Resolution.DevicePixelRatio = DisplayInformation::GetForCurrentView().LogicalDpi() / 96.0f;
        return m_state.Resolution.DevicePixelRatio;
    }
}