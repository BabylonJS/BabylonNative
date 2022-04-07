#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Metal;

    void DeviceImpl::ConfigureBgfxPlatformData(const DeviceConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.context = config.Device;
    }
}
