#pragma once

#include <Babylon/Plugins/ShaderCompiler.h>

#include <gsl/gsl>
#include <spirv_cross.hpp>
#include <spirv_parser.hpp>
#include <bgfx/bgfx.h>
#include <map>
#include <string>

namespace Babylon::ShaderCompilerCommon
{
    // Mapping from bgfx attribute names to bgfx::Attrib::Enum.
    // Used by both the shader traverser (to assign SPIR-V locations) and
    // the shader binary builder (to write correct attrib IDs and set up
    // VertexAttributeLocations).
    inline const std::map<std::string, bgfx::Attrib::Enum>& GetBgfxNameToAttribMap()
    {
        static const std::map<std::string, bgfx::Attrib::Enum> map = {
            {"a_position",  bgfx::Attrib::Position},
            {"a_normal",    bgfx::Attrib::Normal},
            {"a_tangent",   bgfx::Attrib::Tangent},
            {"a_texcoord0", bgfx::Attrib::TexCoord0},
            {"a_texcoord1", bgfx::Attrib::TexCoord1},
            {"a_texcoord2", bgfx::Attrib::TexCoord2},
            {"a_texcoord3", bgfx::Attrib::TexCoord3},
            {"a_texcoord4", bgfx::Attrib::TexCoord4},
            {"a_texcoord5", bgfx::Attrib::TexCoord5},
            {"a_texcoord6", bgfx::Attrib::TexCoord6},
            {"a_texcoord7", bgfx::Attrib::TexCoord7},
            {"a_color0",    bgfx::Attrib::Color0},
            {"a_indices",   bgfx::Attrib::Indices},
            {"a_weight",    bgfx::Attrib::Weight},
            {"i_data0",     bgfx::Attrib::TexCoord4},
            {"i_data1",     bgfx::Attrib::TexCoord5},
            {"i_data2",     bgfx::Attrib::TexCoord6},
            {"i_data3",     bgfx::Attrib::TexCoord7},
            {"i_data5",     bgfx::Attrib::TexCoord3},
        };
        return map;
    }
    std::string ProcessShaderCoordinates(std::string_view source);

    std::string ProcessSamplerFlip(std::string_view source);

    template<typename AppendageT>
    inline void AppendBytes(std::vector<uint8_t>& bytes, const AppendageT appendage)
    {
        auto ptr = reinterpret_cast<const uint8_t*>(&appendage);
        auto stride = static_cast<std::ptrdiff_t>(sizeof(AppendageT));
        bytes.insert(bytes.end(), ptr, ptr + stride);
    }

    template<typename AppendageT = std::string&>
    inline void AppendBytes(std::vector<uint8_t>& bytes, const std::string& string)
    {
        auto ptr = reinterpret_cast<const uint8_t*>(string.data());
        auto stride = static_cast<std::ptrdiff_t>(string.length());
        bytes.insert(bytes.end(), ptr, ptr + stride);
    }

    template<typename ElementT>
    inline void AppendBytes(std::vector<uint8_t>& bytes, const gsl::span<ElementT>& data)
    {
        auto ptr = reinterpret_cast<const uint8_t*>(data.data());
        auto stride = static_cast<std::ptrdiff_t>(data.size() * sizeof(ElementT));
        bytes.insert(bytes.end(), ptr, ptr + stride);
    }

    struct NonSamplerUniformsInfo
    {
        struct Uniform
        {
            enum class TypeEnum
            {
                Vec4,
                Mat4,
                Mat3,
            };

            std::string Name{};
            uint8_t ElementLength{};
            uint32_t Offset{};
            uint16_t RegisterSize{};
            TypeEnum Type{};
        };

        uint16_t ByteSize{};
        std::vector<Uniform> Uniforms{};
    };

    void AppendUniformBuffer(std::vector<uint8_t>& bytes, const NonSamplerUniformsInfo& uniformBuffer, bool isFragment);
    void AppendSamplers(std::vector<uint8_t>& bytes, const spirv_cross::Compiler& compiler, const spirv_cross::SmallVector<spirv_cross::Resource>& samplers, std::map<std::string, uint8_t>& stages, bool isFragment);
    NonSamplerUniformsInfo CollectNonSamplerUniforms(spirv_cross::Parser& parser, const spirv_cross::Compiler& compiler);

    struct ShaderInfo
    {
        std::unique_ptr<spirv_cross::Parser> Parser;
        std::unique_ptr<const spirv_cross::Compiler> Compiler;
        gsl::span<uint8_t> Bytes;
        std::map<std::string, std::string> AttributeRenaming;
    };

    Graphics::BgfxShaderInfo CreateBgfxShader(ShaderInfo vertexShaderInfo, ShaderInfo fragmentShaderInfo);
}
