#pragma once

#include <cstddef>
#include <cstdint>

namespace Babylon::Polyfills::CanvasConstants
{
    constexpr size_t RGBA_CHANNEL_COUNT = 4;
    constexpr uint32_t RGBA_BYTES_PER_PIXEL = 4;
    constexpr size_t RED_CHANNEL_INDEX = 0;
    constexpr size_t GREEN_CHANNEL_INDEX = 1;
    constexpr size_t BLUE_CHANNEL_INDEX = 2;
    constexpr size_t ALPHA_CHANNEL_INDEX = 3;

    constexpr uint32_t COLOR_CHANNEL_MAX = 255;
    constexpr uint32_t COLOR_CHANNEL_HALF = 127;
    constexpr uint32_t COLOR_CHANNEL_MASK = 0xff;
    constexpr uint32_t GREEN_CHANNEL_SHIFT = 8;
    constexpr uint32_t BLUE_CHANNEL_SHIFT = 16;
    constexpr uint32_t ALPHA_CHANNEL_SHIFT = 24;

    constexpr int INVALID_NANOVG_IMAGE = -1;
    constexpr int INVALID_NANOVG_FONT = -1;
    constexpr int MIN_NANOVG_RESOURCE_INDEX = 0;
    constexpr int8_t NO_DEPTH_STENCIL_ATTACHMENT = -1;
}
