#include <Babylon/Graphics.h>
#include <Babylon/Win32/Graphics.h>

namespace Babylon
{
    float Graphics::Impl::GetDevicePixelRatio()
    {
        //UINT dpi = GetDpiForWindow(*((HWND*)(m_graphics.GetNativeWindow())));
        return (float)96.0f / 96.0f;
    }
}