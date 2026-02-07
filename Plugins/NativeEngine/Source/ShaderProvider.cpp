#include "ShaderProvider.h"
#include <stdexcept>
#include <sstream>

#ifdef SHADER_CACHE
#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/Plugins/ShaderCacheInternal.h>
#endif

#include <bgfx/bgfx.h>

#ifdef OPENGL
constexpr bool IsOpenGL = true;
#else
constexpr bool IsOpenGL = false;
#endif

namespace
{
#ifdef SHADER_COMPILER
    void CheckShaderCompilerAssumptions()
    {
        const auto* caps = bgfx::getCaps();
        if (caps->homogeneousDepth != IsOpenGL || caps->originBottomLeft != IsOpenGL)
        {
            throw std::runtime_error{"Shader compiler assumptions are not met."};
        }
    }
#endif
}

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
        CheckShaderCompilerAssumptions();

#ifdef SHADER_CACHE
        if (Plugins::ShaderCache::IsEnabled())
        {
            auto compiledShaderInfo = m_shaderCompiler.Compile(vertexSource, fragmentSource);
            return Plugins::ShaderCache::AddShader(vertexSource, fragmentSource, compiledShaderInfo);
        }
#endif

        return std::make_shared<Graphics::BgfxShaderInfo>(m_shaderCompiler.Compile(vertexSource, fragmentSource));
#else
        throw std::runtime_error{"Shader compiler is not available"};
#endif
    }
}
