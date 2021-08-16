#include "ShaderCompilerCommon.h"
#include "ShaderCompiler.h"
#include <bx/bx.h>
#include <bgfx/bgfx.h>

#define BGFX_UNIFORM_FRAGMENTBIT UINT8_C(0x10) // Copy-pasta from bgfx_p.h
#define BGFX_UNIFORM_SAMPLERBIT UINT8_C(0x20)  // Copy-pasta from bgfx_p.h

// TODO: this needs to be fixed in bgfx
namespace bgfx
{
    uint16_t attribToId(Attrib::Enum _attr);
}

namespace Babylon::ShaderCompilerCommon
{
    void AppendUniformBuffer(std::vector<uint8_t>& bytes, const NonSamplerUniformsInfo& uniformBuffer, bool isFragment)
    {
        const uint8_t fragmentBit = (isFragment ? BGFX_UNIFORM_FRAGMENTBIT : 0);

        for (const auto& uniform : uniformBuffer.Uniforms)
        {
            bgfx::UniformType::Enum bgfxType;

            switch (uniform.Type)
            {
            case NonSamplerUniformsInfo::Uniform::TypeEnum::Vec4:
                bgfxType = bgfx::UniformType::Vec4;
                break;
            case NonSamplerUniformsInfo::Uniform::TypeEnum::Mat4:
                bgfxType = bgfx::UniformType::Mat4;
                break;
            default:
                throw std::runtime_error{"Unrecognized uniform type."};
            }

            AppendBytes(bytes, static_cast<uint8_t>(uniform.Name.size()));
            AppendBytes(bytes, uniform.Name);
            AppendBytes(bytes, static_cast<uint8_t>(bgfxType | fragmentBit));
            AppendBytes(bytes, static_cast<uint8_t>(0)); // Value "num" not used by D3D11 pipeline.
            AppendBytes(bytes, static_cast<uint16_t>(uniform.Offset));
            AppendBytes(bytes, static_cast<uint16_t>(uniform.RegisterSize));
        }
    }

    void AppendSamplers(std::vector<uint8_t>& bytes, const spirv_cross::Compiler& compiler, const spirv_cross::SmallVector<spirv_cross::Resource>& samplers, std::unordered_map<std::string, uint8_t>& stages)
    {
        for (const spirv_cross::Resource& sampler : samplers)
        {
            AppendBytes(bytes, static_cast<uint8_t>(sampler.name.size()));
            AppendBytes(bytes, sampler.name);
            AppendBytes(bytes, static_cast<uint8_t>(bgfx::UniformType::Sampler | BGFX_UNIFORM_SAMPLERBIT));

            // TODO : These values (num, regIndex, regCount) are only used by Vulkan and should be set for that API
            AppendBytes(bytes, static_cast<uint8_t>(0));
            AppendBytes(bytes, static_cast<uint16_t>(0));
            AppendBytes(bytes, static_cast<uint16_t>(0));

#if APIOpenGL
            BX_UNUSED(compiler);
            const auto stage{static_cast<uint8_t>(stages.size())};
            stages[sampler.name] = stage;
#else
            stages[sampler.name] = static_cast<uint8_t>(compiler.get_decoration(sampler.id, spv::DecorationBinding));
#endif
        }
    }

    NonSamplerUniformsInfo CollectNonSamplerUniforms(spirv_cross::Parser& parser, const spirv_cross::Compiler& compiler)
    {
        NonSamplerUniformsInfo info{};

        const auto& resources = compiler.get_shader_resources();
        if (resources.uniform_buffers.size() == 1)
        {
            const auto& uniformBuffer = resources.uniform_buffers[0];
            const auto& type = compiler.get_type(uniformBuffer.base_type_id);
            assert(type.basetype == spirv_cross::SPIRType::BaseType::Struct);

            info.ByteSize = static_cast<uint16_t>(type.member_types.empty() ? 0 : compiler.get_declared_struct_size(type));

            info.Uniforms.resize(type.member_types.size());
            for (uint32_t index = 0; index < type.member_types.size(); ++index)
            {
                auto& uniform = info.Uniforms[index];

                uniform.Name = compiler.get_member_name(uniformBuffer.base_type_id, index);
                uniform.Offset = compiler.get_member_decoration(uniformBuffer.base_type_id, index, spv::DecorationOffset);

                const auto spirType = compiler.get_type(type.member_types[index]);
                if (spirType.columns == 1 && 1 <= spirType.vecsize && spirType.vecsize <= 4)
                {
                    uniform.Type = NonSamplerUniformsInfo::Uniform::TypeEnum::Vec4;
                    uniform.RegisterSize = 1;
                }
                else if (spirType.columns == 4 && spirType.vecsize == 4)
                {
                    uniform.Type = NonSamplerUniformsInfo::Uniform::TypeEnum::Mat4;
                    uniform.RegisterSize = 4;
                }
                else
                {
                    throw std::runtime_error{"Unrecognized uniform type."};
                }

                for (const auto size : spirType.array)
                {
                    uniform.RegisterSize *= static_cast<uint16_t>(size);
                }
            }
        }
        else
        {
            info.ByteSize = 0;
            parser.get_parsed_ir().for_each_typed_id<spirv_cross::SPIRVariable>([&](uint32_t id, spirv_cross::SPIRVariable& var) {
                auto& type = compiler.get_type_from_variable(id);
                if (var.storage == spv::StorageClassUniformConstant &&
                    type.basetype != spirv_cross::SPIRType::BaseType::SampledImage &&
                    type.basetype != spirv_cross::SPIRType::BaseType::Sampler)
                {
                    auto& uniform = info.Uniforms.emplace_back();
                    uniform.Name = compiler.get_name(id);
                    uniform.Offset = 0; // Not actually used for anything by OpenGL.

                    if (type.columns == 1 && 1 <= type.vecsize && type.vecsize <= 4)
                    {
                        uniform.Type = NonSamplerUniformsInfo::Uniform::TypeEnum::Vec4;
                        uniform.RegisterSize = 1;
                    }
                    else if (type.columns == 4 && type.vecsize == 4)
                    {
                        uniform.Type = NonSamplerUniformsInfo::Uniform::TypeEnum::Mat4;
                        uniform.RegisterSize = 4;
                    }
                    else
                    {
                        throw std::runtime_error{"Unrecognized uniform type."};
                    }

                    for (const auto size : type.array)
                    {
                        uniform.RegisterSize *= static_cast<uint16_t>(size);
                    }

                    info.ByteSize += 4 * uniform.RegisterSize;
                }
            });
        }

        return info;
    }

    ShaderCompiler::BgfxShaderInfo CreateBgfxShader(ShaderInfo vertexShaderInfo, ShaderInfo fragmentShaderInfo)
    {
        ShaderCompiler::BgfxShaderInfo bgfxShaderInfo{};

        constexpr uint8_t BGFX_SHADER_BIN_VERSION{6};

        // These hashes are generated internally by BGFX's custom shader compilation pipeline,
        // which we don't have access to.  Fortunately, however, they aren't used for anything
        // crucial; they just have to match.
        constexpr uint32_t vertexOutputsHash{0xBAD1DEA};
        constexpr uint32_t fragmentInputsHash{vertexOutputsHash};

        // Vertex Shader
        {
            std::vector<uint8_t>& vertexBytes{bgfxShaderInfo.VertexBytes};

            const auto& compiler{*vertexShaderInfo.Compiler};
            const spirv_cross::ShaderResources resources{compiler.get_shader_resources()};
            auto uniformsInfo{CollectNonSamplerUniforms(*vertexShaderInfo.Parser, compiler)};
#if __APPLE__
            // with metal, we bind images and not samplers
            const spirv_cross::SmallVector<spirv_cross::Resource>& samplers{resources.separate_images};
#elif APIOpenGL
            const spirv_cross::SmallVector<spirv_cross::Resource>& samplers = resources.sampled_images;
#else
            const spirv_cross::SmallVector<spirv_cross::Resource>& samplers = resources.separate_samplers;
#endif
            size_t numUniforms{uniformsInfo.Uniforms.size() + samplers.size()};

            AppendBytes(vertexBytes, BX_MAKEFOURCC('V', 'S', 'H', BGFX_SHADER_BIN_VERSION));
            AppendBytes(vertexBytes, vertexOutputsHash);
            AppendBytes(vertexBytes, fragmentInputsHash);

            AppendBytes(vertexBytes, static_cast<uint16_t>(numUniforms));
            AppendUniformBuffer(vertexBytes, uniformsInfo, false);
            AppendSamplers(vertexBytes, compiler, samplers, bgfxShaderInfo.UniformStages);

            AppendBytes(vertexBytes, static_cast<uint32_t>(vertexShaderInfo.Bytes.size()));
            AppendBytes(vertexBytes, vertexShaderInfo.Bytes);
            AppendBytes(vertexBytes, static_cast<uint8_t>(0));

            AppendBytes(vertexBytes, static_cast<uint8_t>(resources.stage_inputs.size()));

            for (const spirv_cross::Resource& stageInput : resources.stage_inputs)
            {
                const uint32_t location = compiler.get_decoration(stageInput.id, spv::DecorationLocation);
                AppendBytes(vertexBytes, bgfx::attribToId(static_cast<bgfx::Attrib::Enum>(location)));

                // Map from symbolName -> originalName to associate babylon.js shader attribute -> Babylon Native attribute location.
                bgfxShaderInfo.VertexAttributeLocations[vertexShaderInfo.AttributeRenaming[stageInput.name]] = location;
            }
            AppendBytes(vertexBytes, static_cast<uint16_t>(uniformsInfo.ByteSize));
        }

        // Fragment Shader
        {
            std::vector<uint8_t>& fragmentBytes{bgfxShaderInfo.FragmentBytes};

            const spirv_cross::Compiler& compiler = *fragmentShaderInfo.Compiler;
            const spirv_cross::ShaderResources resources = compiler.get_shader_resources();
            const auto uniformsInfo = CollectNonSamplerUniforms(*fragmentShaderInfo.Parser, compiler);
#if __APPLE__
            const spirv_cross::SmallVector<spirv_cross::Resource>& samplers = resources.separate_images;
#elif APIOpenGL
            const spirv_cross::SmallVector<spirv_cross::Resource>& samplers = resources.sampled_images;
#else
            const spirv_cross::SmallVector<spirv_cross::Resource>& samplers = resources.separate_samplers;
#endif
            size_t numUniforms = uniformsInfo.Uniforms.size() + samplers.size();

            AppendBytes(fragmentBytes, BX_MAKEFOURCC('F', 'S', 'H', BGFX_SHADER_BIN_VERSION));
            AppendBytes(fragmentBytes, vertexOutputsHash);
            AppendBytes(fragmentBytes, fragmentInputsHash);

            AppendBytes(fragmentBytes, static_cast<uint16_t>(numUniforms));
            AppendUniformBuffer(fragmentBytes, uniformsInfo, true);
            AppendSamplers(fragmentBytes, compiler, samplers, bgfxShaderInfo.UniformStages);

            AppendBytes(fragmentBytes, static_cast<uint32_t>(fragmentShaderInfo.Bytes.size()));
            AppendBytes(fragmentBytes, fragmentShaderInfo.Bytes);
            AppendBytes(fragmentBytes, static_cast<uint8_t>(0));

            // Fragment shaders don't have attributes.
            AppendBytes(fragmentBytes, static_cast<uint8_t>(0));

            AppendBytes(fragmentBytes, static_cast<uint16_t>(uniformsInfo.ByteSize));
        }

        return bgfxShaderInfo;
    }
}
