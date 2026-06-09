#pragma once

#include <string_view>
#include <cstdint>
#include <map>
#include <string>
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

        /// `instancedAttributes` maps consumer-bound per-instance vertex-attribute names
        /// (in addition to the built-in instanced names) to the bgfx per-instance attribute
        /// location (a top TEXCOORD semantic, TexCoord7 down) they must occupy. The location
        /// is derived from the draw-time instance packing order so the shader reads each
        /// attribute from the slot bgfx fills. An empty map preserves the legacy per-vertex
        /// mapping for all non-built-in attributes.
        Graphics::BgfxShaderInfo Compile(std::string_view vertexSource, std::string_view fragmentSource, const std::map<std::string, uint32_t>& instancedAttributes = {});
    };
}
