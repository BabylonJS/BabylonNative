#pragma once

#include <Babylon/Plugins/ShaderCacheInternal.h>

#include <fstream>
#include <memory>
#include <string_view>
#include <map>

#include "xxhash.h"

namespace Babylon::Plugins::ShaderCache
{
    class ShaderCacheImpl final
    {
    public:
        ShaderCacheImpl() = default;

        uint32_t Save(std::ofstream& stream);
        uint32_t Load(std::ifstream& stream);

        void Clear();

        std::shared_ptr<Graphics::BgfxShaderInfo> AddShader(std::string_view vertexSource, std::string_view fragmentSource, Graphics::BgfxShaderInfo shaderInfo);
        std::shared_ptr<Graphics::BgfxShaderInfo> GetShader(std::string_view vertexSource, std::string_view fragmentSource);

        static inline std::unique_ptr<ShaderCacheImpl> Instance;

    private:
        struct ShaderHash
        {
            uint64_t VertexHash{};
            uint64_t FragmentHash{};

            ShaderHash() = default;

            ShaderHash(std::string_view vertexSource, std::string_view fragmentSource)
                : VertexHash{XXH3_64bits(vertexSource.data(), vertexSource.size())}
                , FragmentHash{XXH3_64bits(fragmentSource.data(), fragmentSource.size())}
            {
            }

            bool operator<(const ShaderHash& other) const
            {
                return VertexHash < other.VertexHash || (VertexHash == other.VertexHash && FragmentHash < other.FragmentHash);
            }
        };

        std::map<ShaderHash, std::shared_ptr<Graphics::BgfxShaderInfo>> m_cache;
    };
}
