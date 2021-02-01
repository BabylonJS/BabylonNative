#include "..\Shared\NativeInput.h"
#include <winrt/Windows.Devices.Input.h>

using namespace winrt::Windows::Devices::Input;

namespace Babylon::Plugins
{
    bool NativeInput::Impl::HasMouse()
    {
        return MouseCapabilities::MouseCapabilities().MousePresent() > 0;
    }
}