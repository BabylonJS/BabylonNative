#pragma once
#include <string_view>
#include <memory>

namespace Babylon
{
    namespace ShaderCache
    {
        void Enable(bool enabled);
        bool IsEnabled();
        uint32_t Serialize(std::ofstream& stream);
        uint32_t Deserialize(std::ifstream& stream);
    };
}