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

        void SerializeOutTo(std::ofstream& stream);
        uint32_t SerializeInFrom(std::ifstream& stream);
        
        class Impl;
        static Impl* GetImpl();
    };
}