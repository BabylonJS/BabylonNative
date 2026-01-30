#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Babylon::Graphics
{
    struct BgfxShaderInfo
    {
        std::vector<uint8_t> VertexBytes{};
        std::vector<uint8_t> FragmentBytes{};
        std::unordered_map<std::string, uint32_t> VertexAttributeLocations{};
        std::unordered_map<std::string, uint8_t> UniformStages{};
    };
}