#include <Windows.h>

#include <Babylon/Graphics.h>
#include "../GraphicsImpl.h"

namespace Babylon
{
    void Graphics::Impl::SetNativeWindow(GraphicsConfiguration config) {
        SetNativeWindowInternal(config);
    }

    float Graphics::Impl::UpdateDevicePixelRatio()
    {
        UINT dpi{GetDpiForWindow(GetNativeWindow())};

        // In windows, 100% DPI scaling is 96dpi.
        // See https://docs.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels
        std::scoped_lock lock{m_state.Mutex};
        m_state.Resolution.DevicePixelRatio = static_cast<float>(dpi) / 96.0f;
        return m_state.Resolution.DevicePixelRatio;
    }
}
