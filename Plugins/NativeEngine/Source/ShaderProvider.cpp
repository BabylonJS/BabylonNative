#include "ShaderProvider.h"
#include <stdexcept>
#include <sstream>

#ifdef SHADER_CACHE
#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/Plugins/ShaderCacheInternal.h>
#endif

#include <bgfx/bgfx.h>

namespace
{
#ifdef SHADER_COMPILER
    void CheckShaderCompilerAssumptions()
    {
#ifdef OPENGL
        constexpr bool IsOpenGL = true;
#else
        constexpr bool IsOpenGL = false;
#endif

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
    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderProvider::Get(std::string_view vertexSource, std::string_view fragmentSource, [[maybe_unused]] const std::map<std::string, uint32_t>& instancedAttributes)
    {
#ifdef SHADER_CACHE
        // Instanced variants are keyed by their routed attributes as well as the sources.
        if (Plugins::ShaderCache::IsEnabled())
        {
            const auto shaderInfo = Plugins::ShaderCache::GetShader(vertexSource, fragmentSource, instancedAttributes);
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
            auto compiledShaderInfo = m_shaderCompiler.Compile(vertexSource, fragmentSource, instancedAttributes);
            return Plugins::ShaderCache::AddShader(vertexSource, fragmentSource, compiledShaderInfo, instancedAttributes);
        }
#endif

        return std::make_shared<Graphics::BgfxShaderInfo>(m_shaderCompiler.Compile(vertexSource, fragmentSource, instancedAttributes));
#else
        throw std::runtime_error{"Shader compiler is not available"};
#endif
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderProvider::GetCompute([[maybe_unused]] std::string_view computeSource)
    {
#ifdef SHADER_COMPILER
        CheckShaderCompilerAssumptions();
        return std::make_shared<Graphics::BgfxShaderInfo>(m_shaderCompiler.CompileCompute(computeSource));
#else
        throw std::runtime_error{"Shader compiler is not available"};
#endif
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderProvider::GetTransformFeedback([[maybe_unused]] std::string_view vertexSource, [[maybe_unused]] const std::vector<std::string>& varyings)
    {
#ifdef SHADER_CACHE
        if (Plugins::ShaderCache::IsEnabled())
        {
            const auto shaderInfo = Plugins::ShaderCache::GetTransformFeedbackShader(vertexSource, varyings);
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
            return Plugins::ShaderCache::AddTransformFeedbackShader(vertexSource, varyings, m_shaderCompiler.CompileTransformFeedback(vertexSource, varyings));
        }
#endif

        return std::make_shared<Graphics::BgfxShaderInfo>(m_shaderCompiler.CompileTransformFeedback(vertexSource, varyings));
#else
        throw std::runtime_error{"Shader compiler is not available"};
#endif
    }
}
