#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/DeviceQueries.h>
#include "DeviceImpl.h"

#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

namespace Babylon::Graphics
{
    void DeviceImpl::ConfigureBgfxSwapChain(bgfx::SwapChain& swapChain, WindowT window)
    {
        swapChain.nwh = window;
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::Init& init)
    {
        // on Android, having no window or context set the renderer API to no op.
        if (!init.swapChain.nwh && !init.platformData.context)
        {
            init.type = bgfx::RendererType::Noop;
        }
    }

    float GetDevicePixelRatio(WindowT)
    {
        // In Android, the baseline DPI is 160dpi.
        // See https://developer.android.com/training/multiscreen/screendensities#dips-pels
        auto dpi = android::global::GetAppContext().getResources().getConfiguration().getDensityDpi();
        return static_cast<float>(dpi) / 160.0f;
    }
}
