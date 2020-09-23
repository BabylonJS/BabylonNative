#pragma once

#include "ShaderCompiler.h"

#include <gsl/gsl>
#include <spirv_cross.hpp>
#include <spirv_parser.hpp>

namespace Babylon::ShaderCompilerCommon
{
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
                Mat4
            };

            std::string Name{};
            uint32_t Offset{};
            uint16_t RegisterSize{};
            TypeEnum Type{};
        };

        uint16_t ByteSize{};
        std::vector<Uniform> Uniforms{};
    };

    void AppendUniformBuffer(std::vector<uint8_t>& bytes, const NonSamplerUniformsInfo& uniformBuffer, bool isFragment);
    void AppendSamplers(std::vector<uint8_t>& bytes, const spirv_cross::Compiler& compiler, const spirv_cross::SmallVector<spirv_cross::Resource>& samplers, std::unordered_map<std::string, uint8_t>& stages);
    NonSamplerUniformsInfo CollectNonSamplerUniforms(spirv_cross::Parser& parser, const spirv_cross::Compiler& compiler);

    struct ShaderInfo
    {
        std::unique_ptr<spirv_cross::Parser> Parser;
        std::unique_ptr<const spirv_cross::Compiler> Compiler;
        gsl::span<uint8_t> Bytes;
    };

    ShaderCompiler::BgfxShaderInfo CreateBgfxShader(ShaderInfo vertexShaderInfo, ShaderInfo fragmentShaderInfo);
}
