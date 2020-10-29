#pragma once

#include <string_view>
#include <functional>
#include <spirv_cross.hpp>
#include <spirv_parser.hpp>

namespace bgfx
{
    struct Memory;
}

namespace Babylon
{
    /// This class is responsible for compiling the GLSL shader from Babylon.js into
    /// bgfx shader bytes with information about the shader attributes and uniforms.
    class ShaderCompiler final
    {
    public:
        ShaderCompiler();
        ~ShaderCompiler();

        struct BgfxShaderInfo
        {
            std::vector<uint8_t> VertexBytes{};
            std::unordered_map<std::string, uint32_t> VertexAttributeLocations{};
            std::unordered_map<std::string, uint8_t> VertexUniformStages{};

            std::vector<uint8_t> FragmentBytes{};
            std::unordered_map<std::string, uint8_t> FragmentUniformStages{};
        };

        BgfxShaderInfo Compile(std::string_view vertexSource, std::string_view fragmentSource);
    };
}
