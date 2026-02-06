#pragma once

#include <cstdint>
#include <fstream>

namespace Babylon::Plugins::ShaderCache
{
    // Enables the shader cache.
    void Enable();

    // Disables the shader cache.
    void Disable();

    // Returns whether the shader cache is enabled.
    bool IsEnabled();

    // Clears the shader cache.
    void Clear();

    // Saves the shader cache to an output file stream.
    // Returns the number of entries in the shader cache.
    uint32_t Save(std::ofstream& stream);

    // Loads the shader cache from an input file stream.
    // Returns the number of entries in the shader cache.
    uint32_t Load(std::ifstream& stream);
}
