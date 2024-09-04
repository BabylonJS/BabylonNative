#pragma once
#include <map>
#include <stdint.h>
#include "ShaderCompiler.h"
namespace Babylon
{
    class ShaderCache final
    {
        public:
            ShaderCache();
            ~ShaderCache();
            const ShaderCompiler::BgfxShaderInfo* GetShader(std::string_view vertexSource, std::string_view fragmentSource);
            ShaderCompiler::BgfxShaderInfo* AddShader(std::string_view vertexSource, std::string_view fragmentSource, ShaderCompiler::BgfxShaderInfo);
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