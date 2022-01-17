#pragma once

// MSAA is disabled on iOS because of an issue in bgfx
// see https://github.com/BabylonJS/BabylonReactNative/issues/215
// and https://github.com/bkaradzic/bgfx/issues/2620
constexpr uint32_t BGFX_RESET_FLAGS = BGFX_RESET_VSYNC /*| BGFX_RESET_MSAA_X4*/ | BGFX_RESET_MAXANISOTROPY;