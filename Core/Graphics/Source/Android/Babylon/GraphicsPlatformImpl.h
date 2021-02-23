#pragma once

#include <bgfx/bgfx.h>

// MSAA is disabled on Android.
// See issue https://github.com/BabylonJS/BabylonNative/issues/494#issuecomment-731135918
// for explanation
#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MAXANISOTROPY)

namespace Babylon
{
    constexpr auto inline BgfxDefaultRendererType{bgfx::RendererType::OpenGLES};
}
