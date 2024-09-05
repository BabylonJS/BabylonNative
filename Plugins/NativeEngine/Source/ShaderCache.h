#pragma once
#include <Babylon/ShaderCache.h>

#include <iostream>
#include <fstream>

namespace Babylon
{
    class ShaderCacheImpl
    {
    public:
        ShaderCacheImpl() = default;
        ~ShaderCacheImpl() = default;
        const ShaderCompiler::BgfxShaderInfo* GetShader(std::string_view vertexSource, std::string_view fragmentSource);
        void AddShader(std::string_view vertexSource, std::string_view fragmentSource, ShaderCompiler::BgfxShaderInfo shaderInfo);

        uint32_t Serialize(std::ofstream& stream);
        uint32_t Deserialize(std::ifstream& stream);

        static ShaderCacheImpl* GetImpl();
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
        static inline std::unique_ptr<ShaderCacheImpl> Instance{};
        friend ShaderCache;
    };
}