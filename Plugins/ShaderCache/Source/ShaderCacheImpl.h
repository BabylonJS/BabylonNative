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
        using ShaderHash = std::pair<uint64_t, uint64_t>;

        ShaderHash Hash(std::string_view vertexSource, std::string_view fragmentSource);

        std::map<ShaderHash, std::shared_ptr<Graphics::BgfxShaderInfo>> m_cache;
    };
}
