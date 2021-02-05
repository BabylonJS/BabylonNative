#include <Babylon/Graphics.h>
#include "../GraphicsImpl.h"
#include <GraphicsPlatform.h>

namespace Babylon
{
    float Graphics::Impl::GetDevicePixelRatio()
    {
        UINT dpi = GetDpiForWindow(GetNativeWindow());
        return (float)dpi / 96.0f;
    }
}