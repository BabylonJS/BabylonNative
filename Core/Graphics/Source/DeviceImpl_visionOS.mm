#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/DeviceQueries.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window)
    {
        pd.nwh = window;
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::PlatformData& /*pd*/, bgfx::RendererType::Enum& /*renderType*/)
    {
    }

    float GetDevicePixelRatio(WindowT)
    {
      return [UITraitCollection currentTraitCollection].displayScale;
    }
}
