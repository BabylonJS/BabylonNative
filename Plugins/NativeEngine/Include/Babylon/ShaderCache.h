#pragma once
#include <string_view>
#include <memory>

namespace Babylon
{
    class ShaderCache
    {
    public:
        ShaderCache();
        ~ShaderCache();

        uint32_t Serialize(std::ofstream& stream);
        uint32_t Deserialize(std::ifstream& stream);
    };
}