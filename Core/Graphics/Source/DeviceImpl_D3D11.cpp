#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Direct3D11;

    void DeviceImpl::ConfigureBgfxPlatformData(const DeviceConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.context = config.Device;
    }

    void DeviceImpl::ConfigureBgfxPlatformData(const RenderTargetInfo& config, bgfx::PlatformData& pd)
    {
        pd.backBuffer = (void*)config.RenderTarget;
    }
}
