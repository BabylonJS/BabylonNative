#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>

namespace Babylon::Graphics
{
    struct BgfxShaderInfo
    {
        std::vector<uint8_t> VertexBytes{};
        std::vector<uint8_t> FragmentBytes{};
        std::map<std::string, uint32_t> VertexAttributeLocations{};
        std::map<std::string, uint8_t> UniformStages{};
    };
}
