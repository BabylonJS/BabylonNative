#pragma once
#include <Babylon/ShaderCache.h>

#include <iostream>
#include <fstream>

namespace Babylon
{
    class ShaderCache::Impl
    {
    public:
        Impl() = default;
        ~Impl() = default;
        const ShaderCompiler::BgfxShaderInfo* GetShader(std::string_view vertexSource, std::string_view fragmentSource);
        ShaderCompiler::BgfxShaderInfo* AddShader(std::string_view vertexSource, std::string_view fragmentSource, ShaderCompiler::BgfxShaderInfo);

        void SerializeOutTo(std::ofstream& stream);
        uint32_t SerializeInFrom(std::ifstream& stream);
    private:

        struct ShaderHash
        {
            size_t vertexHash;
            size_t fragmentHash;
            bool operator < (const ShaderHash& other) const {
                if (vertexHash < other.vertexHash || (vertexHash == other.vertexHash && fragmentHash < other.fragmentHash))
                    return true;
                return false;
            }
        };
        std::map<ShaderHash, ShaderCompiler::BgfxShaderInfo> Cache{};
    };
}