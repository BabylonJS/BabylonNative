#include "ShaderProvider.h"
#include <stdexcept>
#include <sstream>

#ifdef SHADER_CACHE
#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/Plugins/ShaderCacheInternal.h>
#endif

namespace Babylon
{
    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderProvider::Get(std::string_view vertexSource, std::string_view fragmentSource)
    {
#ifdef SHADER_CACHE
        if (Plugins::ShaderCache::IsEnabled())
        {
            const auto shaderInfo = Plugins::ShaderCache::GetShader(vertexSource, fragmentSource);
            if (shaderInfo)
            {
                return shaderInfo;
            }
        }
#endif

#ifdef SHADER_COMPILER
        auto compiledShaderInfo = m_shaderCompiler.Compile(vertexSource, fragmentSource);

#ifdef SHADER_CACHE
        if (Plugins::ShaderCache::IsEnabled())
        {
            return Plugins::ShaderCache::AddShader(vertexSource, fragmentSource, compiledShaderInfo);
        }
#endif

        return std::make_shared<Graphics::BgfxShaderInfo>(compiledShaderInfo);
#else
        throw std::runtime_error{"Shader compiler is not available"};
#endif
    }
}
