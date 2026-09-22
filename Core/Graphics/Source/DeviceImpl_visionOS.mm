#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/DeviceQueries.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    void DeviceImpl::ConfigureBgfxSwapChain(bgfx::SwapChain& swapChain, WindowT window)
    {
        swapChain.nwh = window;
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::Init& /*init*/)
    {
    }

    float GetDevicePixelRatio(WindowT)
    {
      return [UITraitCollection currentTraitCollection].displayScale;
    }
}
