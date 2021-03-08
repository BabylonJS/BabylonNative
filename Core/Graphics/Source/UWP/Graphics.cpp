#include <Babylon/Graphics.h>
#include <Babylon/GraphicsPlatform.h>
#include "../GraphicsImpl.h"

#include <windows.ui.core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

using namespace ABI::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml::Controls;

namespace Babylon
{
    float Graphics::Impl::UpdateDevicePixelRatio()
    {
        // In windows, 100% DPI scaling is 96dpi.
        // See https://docs.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels

        // In UWP, DisplayInformation is only accessible from the UI thread.
        // See https://docs.microsoft.com/en-us/uwp/api/windows.graphics.display.displayinformation.getforcurrentview?view=winrt-19041#Windows_Graphics_Display_DisplayInformation_GetForCurrentView

        auto windowAsCoreWindow = dynamic_cast<ICoreWindow>(GetNativeWindow());
        if (windowAsCoreWindow){

        } else {
            auto windowAsISwapChainPanel = dynamic_cast<ISwapChainPanel>()
        }
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.DevicePixelRatio;
    }
}