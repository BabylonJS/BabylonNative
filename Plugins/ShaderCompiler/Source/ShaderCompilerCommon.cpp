#include "ShaderCompilerCommon.h"
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
    // Patching shader code to append clip space coordinates for the current rendering API.
    // Can be done with glslang shader traversal. Done with string patching for now.
    std::string ProcessShaderCoordinates(std::string_view source)
    {
        size_t lastBrace = source.find_last_of('}');
        if (lastBrace == std::string_view::npos)
        {
            throw std::runtime_error{"ProcessShaderCoordinates: Could not find closing brace."};
        }

        return std::string{source}.substr(0, lastBrace) + "gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0; }";
    }

    std::string ProcessSamplerFlip(std::string_view source)
    {
        static const std::string shaderNameDefineStr = "#define SHADER_NAME";
        const auto shaderNameDefine = source.find(shaderNameDefineStr);
        if (shaderNameDefine == std::string::npos)
        {
            throw std::runtime_error{"ProcessSamplerFlip: Could not find shader name define."};
        }

        static const auto textureSamplerFunctions = R"(
            highp vec2 flip(highp vec2 uv)
            {
                return vec2(uv.x, 1. - uv.y);
            }
            highp vec3 flip(highp vec3 uv)
            {
                return uv;
            }

            #define texture(x,y) texture(x, flip(y))
            #define textureLod(x,y,z) textureLod(x, flip(y), z)
            #define texelFetch(tex, uv, lod) texelFetch((tex), ivec2((uv).x, textureSize((tex), (lod)).y - 1 - (uv).y), (lod))
            #define SHADER_NAME)";

        std::string result{source};
        result.replace(shaderNameDefine, shaderNameDefineStr.length(), textureSamplerFunctions);
        return result;
    }

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
                case NonSamplerUniformsInfo::Uniform::TypeEnum::Mat3:
                    bgfxType = bgfx::UniformType::Mat3;
                    break;
                default:
                    throw std::runtime_error{"Unrecognized uniform type."};
            }

            AppendBytes(bytes, static_cast<uint8_t>(uniform.Name.size()));
            AppendBytes(bytes, uniform.Name);
            AppendBytes(bytes, static_cast<uint8_t>(bgfxType | fragmentBit));
            AppendBytes(bytes, static_cast<uint8_t>(uniform.ElementLength));
            AppendBytes(bytes, static_cast<uint16_t>(uniform.Offset));
            AppendBytes(bytes, static_cast<uint16_t>(uniform.RegisterSize));
        }
    }

    void AppendSamplers(std::vector<uint8_t>& bytes, const spirv_cross::Compiler& compiler, const spirv_cross::SmallVector<spirv_cross::Resource>& samplers, std::map<std::string, uint8_t>& stages, bool isFragment)
    {
        for (const spirv_cross::Resource& sampler : samplers)
        {
            AppendBytes(bytes, static_cast<uint8_t>(sampler.name.size()));
            AppendBytes(bytes, sampler.name);
            AppendBytes(bytes, static_cast<uint8_t>(bgfx::UniformType::Sampler | BGFX_UNIFORM_SAMPLERBIT));

#if VULKAN
            // bgfx's Vulkan renderer uses regIndex as the texture descriptor binding.
            // The sampler's SPIR-V binding is texture_binding + kSpirvSamplerShift (16),
            // so subtract 16 to recover the texture binding that bgfx expects.
            const auto samplerBinding = compiler.get_decoration(sampler.id, spv::DecorationBinding);
            const uint16_t regIndex = static_cast<uint16_t>(samplerBinding >= 16 ? samplerBinding - 16 : samplerBinding);
            AppendBytes(bytes, static_cast<uint8_t>(0));
            AppendBytes(bytes, regIndex);
            AppendBytes(bytes, static_cast<uint16_t>(0));
#else
            BX_UNUSED(isFragment);
            // These values (num, regIndex, regCount) are only used by Vulkan.
            AppendBytes(bytes, static_cast<uint8_t>(0));
            AppendBytes(bytes, static_cast<uint16_t>(0));
            AppendBytes(bytes, static_cast<uint16_t>(0));
#endif

#if OPENGL
            BX_UNUSED(compiler);
            const auto stage{static_cast<uint8_t>(stages.size())};
            stages[sampler.name] = stage;
#elif VULKAN
            // Stage index must match what bgfx computes: regIndex - reverseShift.
            // For old binding model: reverseShift = (fragment ? 48 : 0) + 16
            const uint8_t reverseShift = static_cast<uint8_t>(isFragment ? 64 : 16);
            stages[sampler.name] = static_cast<uint8_t>(regIndex - reverseShift);
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
                else if (spirType.columns == 3 && spirType.vecsize == 3)
                {
                    uniform.Type = NonSamplerUniformsInfo::Uniform::TypeEnum::Mat3;
                    uniform.RegisterSize = 4;
                }
                else
                {
                    throw std::runtime_error{"Unrecognized uniform type."};
                }

                if (spirType.array.size() == 1)
                {
                    uniform.ElementLength = static_cast<uint8_t>(spirType.array[0]);
                    uniform.RegisterSize *= uniform.ElementLength;
                }
                else if (spirType.array.size() > 1)
                {
                    throw std::runtime_error{"Unsupported multidimensional array."};
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
                    else if (type.columns == 3 && type.vecsize == 3)
                    {
                        uniform.Type = NonSamplerUniformsInfo::Uniform::TypeEnum::Mat3;
                        uniform.RegisterSize = 4;
                    }
                    else
                    {
                        throw std::runtime_error{"Unrecognized uniform type."};
                    }

                    if (type.array.size() == 1)
                    {
                        uniform.ElementLength = static_cast<uint8_t>(type.array[0]);
                        uniform.RegisterSize *= uniform.ElementLength;
                    }
                    else if (type.array.size() > 1)
                    {
                        throw std::runtime_error{"Unsupported multidimensional array."};
                    }

                    info.ByteSize += 4 * uniform.RegisterSize;
                }
            });
        }

        return info;
    }

    Graphics::BgfxShaderInfo CreateBgfxShader(ShaderInfo vertexShaderInfo, ShaderInfo fragmentShaderInfo)
    {
        Graphics::BgfxShaderInfo bgfxShaderInfo{};

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
#elif OPENGL
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
            AppendSamplers(vertexBytes, compiler, samplers, bgfxShaderInfo.UniformStages, false);

            AppendBytes(vertexBytes, static_cast<uint32_t>(vertexShaderInfo.Bytes.size()));
            AppendBytes(vertexBytes, vertexShaderInfo.Bytes);
            AppendBytes(vertexBytes, static_cast<uint8_t>(0));

            AppendBytes(vertexBytes, static_cast<uint8_t>(resources.stage_inputs.size()));

#if VULKAN
            // bgfx's Vulkan renderer uses the attribute's index in the shader binary
            // as the Vulkan location (via m_attrRemap[attr] = index). The SPIR-V uses
            // Attrib::Enum values as Locations (e.g., TexCoord0=10). To make
            // m_attrRemap[attr] == Location, we must write attributes at indices that
            // match their Location, padding gaps with a dummy ID (0) that bgfx skips.
            {
                static const std::map<std::string, bgfx::Attrib::Enum> nameToAttrib = {
                    {"a_position", bgfx::Attrib::Position},
                    {"a_normal", bgfx::Attrib::Normal},
                    {"a_tangent", bgfx::Attrib::Tangent},
                    {"a_texcoord0", bgfx::Attrib::TexCoord0},
                    {"a_texcoord1", bgfx::Attrib::TexCoord1},
                    {"a_texcoord2", bgfx::Attrib::TexCoord2},
                    {"a_texcoord3", bgfx::Attrib::TexCoord3},
                    {"a_color0", bgfx::Attrib::Color0},
                    {"a_indices", bgfx::Attrib::Indices},
                    {"a_weight", bgfx::Attrib::Weight},
                };
                uint32_t maxLocation = 0;
                for (const spirv_cross::Resource& stageInput : resources.stage_inputs)
                {
                    const uint32_t loc = compiler.get_decoration(stageInput.id, spv::DecorationLocation);
                    if (loc > maxLocation) maxLocation = loc;
                }
                // Build a location-indexed map
                std::vector<uint16_t> attribIds(maxLocation + 1, 0);
                for (const spirv_cross::Resource& stageInput : resources.stage_inputs)
                {
                    const uint32_t loc = compiler.get_decoration(stageInput.id, spv::DecorationLocation);
                    auto it = nameToAttrib.find(stageInput.name);
                    bgfx::Attrib::Enum attrib = (it != nameToAttrib.end())
                        ? it->second
                        : static_cast<bgfx::Attrib::Enum>(loc);
                    attribIds[loc] = bgfx::attribToId(attrib);

                    const std::string& originalName = vertexShaderInfo.AttributeRenaming[stageInput.name];
                    bgfxShaderInfo.VertexAttributeLocations[originalName] = static_cast<uint32_t>(attrib);
                }
                // Rewrite the count to include padding entries
                vertexBytes.resize(vertexBytes.size() - 1); // remove the count we just wrote
                AppendBytes(vertexBytes, static_cast<uint8_t>(attribIds.size()));
                for (uint16_t id : attribIds)
                {
                    AppendBytes(vertexBytes, id);
                }
            }
#else
            for (const spirv_cross::Resource& stageInput : resources.stage_inputs)
            {
                const uint32_t location = compiler.get_decoration(stageInput.id, spv::DecorationLocation);
                AppendBytes(vertexBytes, bgfx::attribToId(static_cast<bgfx::Attrib::Enum>(location)));

                // Map from symbolName -> originalName to associate babylon.js shader attribute -> Babylon Native attribute location.
                bgfxShaderInfo.VertexAttributeLocations[vertexShaderInfo.AttributeRenaming[stageInput.name]] = location;
            }
#endif
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
#elif OPENGL
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
            AppendSamplers(fragmentBytes, compiler, samplers, bgfxShaderInfo.UniformStages, true);

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
