#pragma once

#include <bgfx/bgfx.h>

// MSAA is disabled on Android
// more infos here : https://github.com/BabylonJS/BabylonNative/issues/507
constexpr uint32_t BGFX_RESET_FLAGS = BGFX_RESET_VSYNC /*| BGFX_RESET_MSAA_X4*/ | BGFX_RESET_MAXANISOTROPY;