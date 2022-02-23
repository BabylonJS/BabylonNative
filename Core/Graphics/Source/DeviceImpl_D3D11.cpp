#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Direct3D11;

    void DeviceImpl::ConfigureBgfxPlatformData(const DeviceConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.context = config.Device;

        if (config.Backbuffer != nullptr)
        {
            pd.backBuffer = config.Backbuffer;
        }

        if (config.DepthStencil != nullptr)
        {
            pd.backBufferDS = config.DepthStencil;
        }
    }

    void DeviceImpl::UpdateBgfxBackBuffer(const BackBufferUpdateInfo& update, bgfx::PlatformData& pd) 
    {
        pd.backBuffer = update.Backbuffer;
        pd.backBufferDS = update.DepthStencil;
    }
}
