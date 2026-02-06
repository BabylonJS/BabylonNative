#pragma once

#include <string_view>
#include <Babylon/Graphics/BgfxShaderInfo.h>

namespace Babylon::Plugins
{
    /// This class is responsible for compiling the GLSL shader from Babylon.js into
    /// bgfx shader bytes with information about the shader attributes and uniforms.
    class ShaderCompiler final
    {
    public:
        ShaderCompiler();
        ~ShaderCompiler();

        Graphics::BgfxShaderInfo Compile(std::string_view vertexSource, std::string_view fragmentSource);
    };
}
