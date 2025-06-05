#pragma once
#include <string_view>
#include <memory>

namespace Babylon
{
    namespace ShaderCache
    {
        void Enabled(bool enabled);
        bool Enabled();
        uint32_t Serialize(std::ofstream& stream);
        uint32_t Deserialize(std::ifstream& stream);
    };
}