#pragma once

#include <Babylon/Graphics/BgfxShaderInfo.h>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#ifdef SHADER_COMPILER
#include <Babylon/Plugins/ShaderCompiler.h>
#endif

#include <memory>

namespace Babylon
{
    class ShaderProvider
    {
    public:
        std::shared_ptr<Graphics::BgfxShaderInfo> Get(std::string_view vertexSource, std::string_view fragmentSource, const std::map<std::string, uint32_t>& instancedAttributes = {});

        // Compiles a single GLSL compute shader into a bgfx CSH shader binary.
        std::shared_ptr<Graphics::BgfxShaderInfo> GetCompute(std::string_view computeSource);

        // Compiles a transform feedback vertex shader into its emulating compute shader.
        std::shared_ptr<Graphics::BgfxShaderInfo> GetTransformFeedback(std::string_view vertexSource, const std::vector<std::string>& varyings);

    private:
#ifdef SHADER_COMPILER
        Plugins::ShaderCompiler m_shaderCompiler{};
#endif
    };
}
