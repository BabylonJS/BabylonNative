#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Direct3D12;

    void DeviceImpl::ConfigureBgfxPlatformData(const ContextConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.context = config.Context;
    }
}
