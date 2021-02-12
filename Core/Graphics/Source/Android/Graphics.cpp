#include <Babylon/Graphics.h>
#include "../GraphicsImpl.h"
#include <GraphicsPlatform.h>

#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

namespace Babylon
{
    float Graphics::Impl::GetDevicePixelRatio()
    {
        auto dpi = android::global::GetAppContext().getResources().getConfiguration().getDensityDpi();
        return (float)dpi/160.0f;
    }
}