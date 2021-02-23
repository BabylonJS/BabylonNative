#include <Babylon/Graphics.h>
#include "../GraphicsImpl.h"
#include <Babylon/GraphicsPlatform.h>

namespace Babylon
{
    float Graphics::Impl::GetDevicePixelRatio()
    {
        UINT dpi{GetDpiForWindow(GetNativeWindow())};
        return static_cast<float>(dpi) / 96.0f;
    }
}
