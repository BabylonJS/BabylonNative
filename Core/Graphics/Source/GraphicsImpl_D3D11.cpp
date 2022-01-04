#include <Babylon/GraphicsRendererType.h>
#include "GraphicsImpl.h"

namespace Babylon
{
    const bgfx::RendererType::Enum GraphicsImpl::s_bgfxRenderType = bgfx::RendererType::Direct3D11;

    void GraphicsImpl::ConfigureBgfxPlatformData(const ContextConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.context = config.Context;
    }
}
