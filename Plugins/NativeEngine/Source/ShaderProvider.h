#pragma once

#include <Babylon/Graphics/BgfxShaderInfo.h>

#ifdef SHADER_COMPILER
#include <Babylon/Plugins/ShaderCompiler.h>
#endif

#include <memory>

namespace Babylon
{
    class ShaderProvider
    {
    public:
        std::shared_ptr<Graphics::BgfxShaderInfo> Get(std::string_view vertexSource, std::string_view fragmentSource);

    private:
#ifdef SHADER_COMPILER
        Plugins::ShaderCompiler m_shaderCompiler{};
#endif
    };
}
