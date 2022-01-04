#pragma once

#include <bgfx/bgfx.h>

#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)

namespace Babylon
{
#if VULKAN
    constexpr auto inline BgfxDefaultRendererType{bgfx::RendererType::Vulkan};
#elif D3D11
    constexpr auto inline BgfxDefaultRendererType{bgfx::RendererType::Direct3D11};
#elif D3D12
    constexpr auto inline BgfxDefaultRendererType{bgfx::RendererType::Direct3D12};
#endif
}
