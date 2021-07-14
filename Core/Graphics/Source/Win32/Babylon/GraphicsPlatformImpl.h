#pragma once

#include <bgfx/bgfx.h>

#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)

namespace Babylon
{
#if APIVulkan
    constexpr auto inline BgfxDefaultRendererType{bgfx::RendererType::Vulkan};
#else
    constexpr auto inline BgfxDefaultRendererType{bgfx::RendererType::Direct3D11};
#endif
}
