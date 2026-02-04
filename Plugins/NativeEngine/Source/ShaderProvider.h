#pragma once

#include <Babylon/Graphics/BgfxShaderInfo.h>

#ifdef SHADER_COMPILER
#include <Babylon/Plugins/ShaderCompilerInternal.h>
#endif

#include <memory>

namespace Babylon
{
    class ShaderProvider
    {
    public:
        std::shared_ptr<Graphics::BgfxShaderInfo> Get(std::string vertexSource, std::string fragmentSource);

    private:
#ifdef SHADER_COMPILER
        Plugins::ShaderCompiler m_shaderCompiler{};
#endif
    };
}
