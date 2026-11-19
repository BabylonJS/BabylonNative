#pragma once

#include <gsl/span>
#include <cstdint>
#include <string>

namespace Babylon::Polyfills::Internal
{
    std::string EncodeCanvasPNG(uint32_t width, uint32_t height, gsl::span<const uint8_t> rgba);
}
