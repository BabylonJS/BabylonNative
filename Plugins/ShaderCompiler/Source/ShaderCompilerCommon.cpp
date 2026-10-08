#include "ShaderCompilerCommon.h"
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <glslang/Public/ResourceLimits.h>
#include <algorithm>
#include <cctype>
#include <regex>
#include <set>

#define BGFX_UNIFORM_FRAGMENTBIT UINT8_C(0x10) // Copy-pasta from bgfx_p.h
#define BGFX_UNIFORM_SAMPLERBIT UINT8_C(0x20)  // Copy-pasta from bgfx_p.h

// TODO: this needs to be fixed in bgfx
namespace bgfx
{
    uint16_t attribToId(Attrib::Enum _attr);
}

namespace Babylon::ShaderCompilerCommon
{
    namespace
    {
        struct ShaderToken
        {
            size_t Offset;
            std::string_view Text;
        };

        std::vector<ShaderToken> TokenizeShader(std::string_view source)
        {
            std::vector<ShaderToken> tokens;
            for (size_t offset = 0; offset < source.size();)
            {
                const auto start = offset;
                const auto character = static_cast<unsigned char>(source[offset]);
                if (std::isspace(character))
                {
                    ++offset;
                    continue;
                }
                if (source.substr(offset, 2) == "//")
                {
                    const auto end = source.find('\n', offset + 2);
                    offset = end == std::string_view::npos ? source.size() : end;
                    continue;
                }
                if (source.substr(offset, 2) == "/*")
                {
                    const auto end = source.find("*/", offset + 2);
                    if (end == std::string_view::npos)
                    {
                        throw std::runtime_error{"Unterminated shader comment."};
                    }
                    offset = end + 2;
                    continue;
                }
                if (character == '"')
                {
                    ++offset;
                    while (offset < source.size() && source[offset] != '"')
                    {
                        offset += source[offset] == '\\' && offset + 1 < source.size() ? 2 : 1;
                    }
                    if (offset == source.size())
                    {
                        throw std::runtime_error{"Unterminated shader string."};
                    }
                    ++offset;
                }
                else if (std::isalpha(character) || character == '_')
                {
                    do
                    {
                        ++offset;
                    } while (offset < source.size() &&
                        (std::isalnum(static_cast<unsigned char>(source[offset])) || source[offset] == '_'));
                }
                else
                {
                    ++offset;
                }
                tokens.push_back({start, source.substr(start, offset - start)});
            }
            return tokens;
        }
    }

    std::string PreprocessShader(EShLanguage stage, std::string_view source)
    {
        std::string input{source};
        size_t insertion{};
        const auto tokens = TokenizeShader(source);
        if (tokens.size() >= 2 && tokens[0].Text == "#" && tokens[1].Text == "version")
        {
            const auto end = source.find('\n', tokens[0].Offset);
            if (end == std::string_view::npos)
            {
                input += '\n';
                insertion = input.size();
            }
            else
            {
                insertion = end + 1;
            }
        }
        // Keep #version first, including after glslang serializes the preprocessed source.
        input.insert(insertion, "#extension GL_EXT_shader_implicit_conversions : enable\n");
        glslang::TShader shader{stage};
        const char* data = input.data();
        const int length = gsl::narrow_cast<int>(input.size());
        shader.setStringsWithLengths(&data, &length, 1);
        glslang::TShader::ForbidIncluder includer;
        std::string result;
        if (!shader.preprocess(GetDefaultResources(), 310, EProfile::EEsProfile, true, true, EShMsgDefault, &result, includer))
        {
            throw std::runtime_error{shader.getInfoLog()};
        }
        return result;
    }

    std::map<std::string, std::string> RenameShaderUniforms(std::string& source, std::string* pairedSource)
    {
        const auto first = TokenizeShader(source);
        const auto second = pairedSource ? TokenizeShader(*pairedSource) : std::vector<ShaderToken>{};
        std::set<std::string> names;
        std::set<std::string> candidates;
        std::set<std::string> firstCandidates;
        std::set<std::string> secondCandidates;
        constexpr std::string_view predefined[] = {
            "u_viewRect", "u_viewTexel", "u_view", "u_invView", "u_proj", "u_invProj",
            "u_viewProj", "u_invViewProj", "u_model", "u_modelView", "u_invModelView",
            "u_modelViewProj", "u_alphaRef", "u_alphaRef4", "bgfx_indirectArgBase",
            Graphics::RENDER_TARGET_TRANSFORM_UNIFORM_NAME};
        const auto collect = [&](const std::vector<ShaderToken>& tokens, std::set<std::string>& stageCandidates) {
            bool uniform{};
            for (const auto& token : tokens)
            {
                names.emplace(token.Text);
                if (token.Text == "uniform")
                {
                    uniform = true;
                }
                else if (token.Text == ";" || token.Text == "{")
                {
                    uniform = false;
                }
                else if (uniform && (token.Text == "textureSize" || token.Text.starts_with(Graphics::SAMPLER_STATE_UNIFORM_PREFIX) ||
                    std::find(std::begin(predefined), std::end(predefined), token.Text) != std::end(predefined)))
                {
                    candidates.emplace(token.Text);
                    stageCandidates.emplace(token.Text);
                }
            }
        };
        collect(first, firstCandidates);
        collect(second, secondCandidates);

        std::map<std::string, std::string> renamed;
        std::map<std::string, std::string> originalNames;
        for (const auto& name : candidates)
        {
            const auto prefix = "bnUserUniform_" + name;
            auto replacement = prefix;
            for (size_t suffix = 1; names.count(replacement); ++suffix)
            {
                replacement = prefix + std::to_string(suffix);
            }
            names.emplace(replacement);
            renamed.emplace(name, replacement);
            originalNames.emplace(replacement, name);
        }
        const auto rewrite = [&](std::string& text, const std::vector<ShaderToken>& tokens, const std::set<std::string>& stageCandidates) {
            std::string result;
            size_t copied{};
            for (size_t index = 0; index < tokens.size(); ++index)
            {
                const auto& token = tokens[index];
                const auto replacement = renamed.find(std::string{token.Text});
                if (replacement == renamed.end() || !stageCandidates.count(replacement->first) ||
                    (token.Text == "textureSize" && index + 1 < tokens.size() && tokens[index + 1].Text == "("))
                {
                    continue;
                }
                result.append(text, copied, token.Offset - copied);
                result += replacement->second;
                copied = token.Offset + token.Text.size();
            }
            result.append(text, copied);
            return result;
        };
        if (!renamed.empty())
        {
            auto rewritten = rewrite(source, first, firstCandidates);
            if (pairedSource)
            {
                *pairedSource = rewrite(*pairedSource, second, secondCandidates);
            }
            source = std::move(rewritten);
        }
        return originalNames;
    }

    // The synthetic instance-data locations must line up with the live bgfx::Attrib enum so that
    // per-instance inputs land on the top TEXCOORD semantics bgfx binds them to, and so that they
    // sort above every real vertex attribute (and can therefore be excluded from the shader's
    // attribute table below).
    static_assert(Babylon::Graphics::TEXCOORD0_ATTRIBUTE_LOCATION == static_cast<uint32_t>(bgfx::Attrib::TexCoord0));
    static_assert(Babylon::Graphics::INSTANCE_DATA_FIRST_LOCATION >= static_cast<uint32_t>(bgfx::Attrib::Count));
    static_assert(Babylon::Graphics::INSTANCE_DATA_LAST_LOCATION >= static_cast<uint32_t>(bgfx::Attrib::Count));
    static_assert(Babylon::Graphics::BUILTIN_INSTANCE_DATA_SLOT_COUNT <= Babylon::Graphics::MAX_INSTANCE_DATA_SLOT_COUNT);

    // Patching shader code to append clip space coordinates for the current rendering API.
    // Can be done with glslang shader traversal. Done with string patching for now.
    // Also flips clip-space Y for GL-row-order targets (see RENDER_TARGET_TRANSFORM_UNIFORM_NAME).
    std::string ProcessShaderCoordinates(std::string_view source)
    {
        size_t lastBrace = source.find_last_of('}');
        if (lastBrace == std::string_view::npos)
        {
            throw std::runtime_error{"ProcessShaderCoordinates: Could not find closing brace."};
        }

        std::smatch match;
        const std::string body{source.substr(0, lastBrace)};
        static const std::regex mainPattern{R"(\bvoid\s+main\s*\()"};
        if (!std::regex_search(body, match, mainPattern))
        {
            throw std::runtime_error{"ProcessShaderCoordinates: Could not find main."};
        }

        const std::string uniformName{Graphics::RENDER_TARGET_TRANSFORM_UNIFORM_NAME};
        const auto mainOffset = static_cast<size_t>(match.position(0));
        return body.substr(0, mainOffset) + "uniform vec4 " + uniformName + ";\n" + body.substr(mainOffset) +
               "gl_Position.y *= " + uniformName + ".z; gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0; }";
    }

    std::string ProcessSamplerFlip(std::string_view source)
    {
        // The AST traverser flips 2D/array/volume coordinates, including explicit gradients
        // and integer fetches, while preserving layers and cube directions.
        // Retain this identity passthrough for the existing backend call sites.
        return std::string{source};
    }

    // bgfx shader binary v8+ stores texComponent/texDimension after each uniform; v10+ also
    // stores texFormat. IDs match bgfx src/shader.cpp (0 = unknown / TextureDimension::Count).
    void AppendUniformTextureMeta(std::vector<uint8_t>& bytes, uint8_t texComponent, uint8_t texDimension, uint16_t texFormat)
    {
        AppendBytes(bytes, texComponent);
        AppendBytes(bytes, texDimension);
        AppendBytes(bytes, texFormat);
    }

    // Map SPIR-V image dim to bgfx TextureDimension id. Pure Sampler types have no dim and
    // return 0 so the backend falls back to the bound texture's actual dimension.
    uint8_t TextureDimensionIdFromResource(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
    {
        const auto& type = compiler.get_type(resource.type_id);
        if (type.basetype != spirv_cross::SPIRType::SampledImage && type.basetype != spirv_cross::SPIRType::Image)
        {
            return 0;
        }

        const bool arrayed{type.image.arrayed};
        switch (type.image.dim)
        {
            case spv::Dim1D:
                return 0x01;
            case spv::Dim2D:
                return arrayed ? static_cast<uint8_t>(0x03) : static_cast<uint8_t>(0x02);
            case spv::Dim3D:
                return 0x06;
            case spv::DimCube:
                return arrayed ? static_cast<uint8_t>(0x05) : static_cast<uint8_t>(0x04);
            default:
                return 0;
        }
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
            AppendUniformTextureMeta(bytes);
        }
    }

    void AppendSamplers(std::vector<uint8_t>& bytes, const spirv_cross::Compiler& compiler, const spirv_cross::ParsedIR& originalIr, const spirv_cross::SmallVector<spirv_cross::Resource>& samplers, std::map<std::string, uint8_t>& stages)
    {
#if D3D11 || D3D12
        const spirv_cross::SmallVector<spirv_cross::Resource> images{compiler.get_shader_resources().separate_images};
#endif
        for (const spirv_cross::Resource& sampler : samplers)
        {
            // SPIRV-Cross's HLSL/MSL backends rename resources whose name collides with a reserved
            // keyword of the target language (e.g. a GLSL sampler named "Texture2D" or "Texture2DArray"
            // becomes "_Texture2D" because those are HLSL built-in object types). bgfx's uniform table
            // and Babylon.js look samplers up by their original GLSL name, so the renamed identifier would
            // never bind (the sampler silently samples nothing). Recover the pre-transpile name from the
            // parser's ParsedIR (the Compiler transpiles a private copy, leaving the parser's names intact).
            const std::string& originalName = originalIr.get_name(sampler.id);
            std::string name = originalName.empty() ? sampler.name : originalName;

            AppendBytes(bytes, static_cast<uint8_t>(name.size()));
            AppendBytes(bytes, name);
            AppendBytes(bytes, static_cast<uint8_t>(bgfx::UniformType::Sampler | BGFX_UNIFORM_SAMPLERBIT));

            // num / regIndex / regCount: Vulkan's appender (ShaderCompilerVulkan) fills regIndex.
            // D3D keys each sampler's texture dimension by regIndex, and an unknown dimension
            // makes bgfx bind multisampled depth as multisampled; other backends ignore it.
            uint8_t num{0};
            uint16_t regIndex{0};
            uint16_t regCount{0};
            uint8_t texDimension{TextureDimensionIdFromResource(compiler, sampler)};
#if D3D11 || D3D12
            // D3D samplers are split into a SamplerState and a texture sharing its binding.
            regIndex = static_cast<uint16_t>(compiler.get_decoration(sampler.id, spv::DecorationBinding));
            for (const spirv_cross::Resource& image : images)
            {
                if (compiler.get_decoration(image.id, spv::DecorationBinding) == regIndex)
                {
                    texDimension = TextureDimensionIdFromResource(compiler, image);
                    break;
                }
            }
#endif
            AppendBytes(bytes, num);
            AppendBytes(bytes, regIndex);
            AppendBytes(bytes, regCount);
            AppendUniformTextureMeta(bytes, /*texComponent*/ 0, texDimension);

#if OPENGL
            // A program's vertex and fragment shaders share this stages map, and Babylon's
            // generated GLSL frequently declares the same sampler in both stages. Assign a stage
            // (texture unit) the first time a sampler name is seen and reuse it thereafter; without
            // the guard the second pass would re-run stages[name] = stages.size() on already-present
            // names, which doesn't grow the map, collapsing every such sampler onto the same unit.
            // That produced multiple sampler2D uniforms and a samplerCube all pointing at one unit,
            // which GLES/ANGLE rejects at draw time with GL_INVALID_OPERATION (D3D11/Metal don't
            // validate this, so the bug was GL-only). Each sampler now gets its own distinct unit,
            // mirroring WebGL's Effect._bindSamplerUniformToChannel. Keyed on the recovered
            // pre-transpile name (see above) so both stages agree on the same identifier.
            if (stages.find(name) == stages.end())
            {
                stages[name] = static_cast<uint8_t>(stages.size());
            }
#else
            stages[name] = static_cast<uint8_t>(compiler.get_decoration(sampler.id, spv::DecorationBinding));
#endif
        }
    }

    void AssignUniformBufferBindings(spirv_cross::Compiler& compiler)
    {
        // bgfx's D3D11/D3D12 backends bind their predefined constant buffer at register b0
        // (see VSSetConstantBuffers/PSSetConstantBuffers in renderer_d3d11.cpp), so the "Frame"
        // block synthesized by MoveNonSamplerUniformsIntoStruct must keep DecorationBinding=0.
        // Any other uniform blocks declared by the source shader get sequential bindings
        // starting at 1 so SPIRV-Cross emits distinct `register(bN)` annotations and FXC
        // doesn't reject the shader with X4578 ("cbuffer bank N used more than once").
        const auto resources = compiler.get_shader_resources();

        for (const auto& uniformBuffer : resources.uniform_buffers)
        {
            if (uniformBuffer.name == "Frame")
            {
                compiler.set_decoration(uniformBuffer.id, spv::DecorationBinding, 0);
            }
        }

        uint32_t nextBinding = 1;
        for (const auto& uniformBuffer : resources.uniform_buffers)
        {
            if (uniformBuffer.name != "Frame")
            {
                compiler.set_decoration(uniformBuffer.id, spv::DecorationBinding, nextBinding++);
            }
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
                    type.basetype != spirv_cross::SPIRType::BaseType::Sampler &&
                    type.basetype != spirv_cross::SPIRType::BaseType::Image &&
                    type.basetype != spirv_cross::SPIRType::BaseType::Struct &&
                    type.basetype != spirv_cross::SPIRType::BaseType::AtomicCounter)
                {
                    auto& uniform = info.Uniforms.emplace_back();
                    uniform.Name = compiler.get_name(id);
                    if (uniform.Name.empty())
                    {
                        throw std::runtime_error{"Uniform with empty name detected — likely a UBO block or struct uniform was incorrectly type-converted."};
                    }
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

    namespace
{
    const spirv_cross::SmallVector<spirv_cross::Resource>& SelectSamplers(const spirv_cross::ShaderResources& resources, SamplerResourceSet samplerResources)
    {
        switch (samplerResources)
        {
            case SamplerResourceSet::SeparateImages:
                return resources.separate_images;
            case SamplerResourceSet::SampledImages:
                return resources.sampled_images;
            case SamplerResourceSet::SeparateSamplers:
            default:
                return resources.separate_samplers;
        }
    }

    SamplerResourceSet DefaultSamplerResourceSet()
    {
#if __APPLE__
        // Metal binds images, not samplers.
        return SamplerResourceSet::SeparateImages;
#elif OPENGL
        return SamplerResourceSet::SampledImages;
#else
        return SamplerResourceSet::SeparateSamplers;
#endif
    }
}

Graphics::BgfxShaderInfo CreateBgfxShader(ShaderInfo vertexShaderInfo, ShaderInfo fragmentShaderInfo, std::map<std::string, uint32_t> builtInInstanceDataSlots)
{
    return CreateBgfxShader(
        std::move(vertexShaderInfo),
        std::move(fragmentShaderInfo),
        std::move(builtInInstanceDataSlots),
        DefaultSamplerResourceSet(),
        AppendSamplers);
}

Graphics::BgfxShaderInfo CreateBgfxShader(
    ShaderInfo vertexShaderInfo,
    ShaderInfo fragmentShaderInfo,
    std::map<std::string, uint32_t> builtInInstanceDataSlots,
    SamplerResourceSet samplerResources,
    AppendSamplersFn appendSamplers)
{
    Graphics::BgfxShaderInfo bgfxShaderInfo{};
    bgfxShaderInfo.BuiltInInstanceDataSlots = std::move(builtInInstanceDataSlots);

    // Must match BGFX_SHADER_BIN_VERSION in bgfx tools/shaderc/shaderc.cpp.
    // v12 requires raw SRV/UAV binding masks after the in/out hashes, and
    // uniform entries always carry texComponent/texDimension/texFormat (v8/v10).
    // v13: bgfx resolves sampled multisampled depth for samplers that report a
    // non-multisampled texDimension (older binaries are assumed multisampled).
    constexpr uint8_t BGFX_SHADER_BIN_VERSION{13};

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
        const spirv_cross::SmallVector<spirv_cross::Resource>& samplers{SelectSamplers(resources, samplerResources)};
        size_t numUniforms{uniformsInfo.Uniforms.size() + samplers.size()};

        AppendBytes(vertexBytes, BX_MAKEFOURCC('V', 'S', 'H', BGFX_SHADER_BIN_VERSION));
        AppendBytes(vertexBytes, vertexOutputsHash);
        AppendBytes(vertexBytes, fragmentInputsHash);
        // Raw SRV/UAV masks (bgfx v12). BN runtime shaders don't expose raw buffers here.
        AppendBytes(vertexBytes, static_cast<uint32_t>(0));
        AppendBytes(vertexBytes, static_cast<uint32_t>(0));

        AppendBytes(vertexBytes, static_cast<uint16_t>(numUniforms));
        AppendUniformBuffer(vertexBytes, uniformsInfo, false);
        appendSamplers(vertexBytes, compiler, vertexShaderInfo.Parser->get_parsed_ir(), samplers, bgfxShaderInfo.UniformStages);

        AppendBytes(vertexBytes, static_cast<uint32_t>(vertexShaderInfo.Bytes.size()));
        AppendBytes(vertexBytes, vertexShaderInfo.Bytes);
        AppendBytes(vertexBytes, static_cast<uint8_t>(0));

        // Per-instance vertex attributes are encoded with synthetic locations at/above
        // bgfx::Attrib::Count (they occupy the top TEXCOORD semantics that bgfx binds by
        // semantic rather than via bgfx::Attrib). They must be excluded from the shader's
        // attribute table: bgfx::attribToId only covers real bgfx::Attrib values, and the
        // backends resolve instance data from the instance-data buffer independently. This
        // mirrors bgfx's own reflection, which skips semantics without a bgfx::Attrib mapping.
        uint8_t numVertexAttributes{0};
        for (const spirv_cross::Resource& stageInput : resources.stage_inputs)
        {
            const uint32_t location = compiler.get_decoration(stageInput.id, spv::DecorationLocation);
            if (location < static_cast<uint32_t>(bgfx::Attrib::Count))
            {
                ++numVertexAttributes;
            }
        }

        AppendBytes(vertexBytes, numVertexAttributes);

        for (const spirv_cross::Resource& stageInput : resources.stage_inputs)
        {
            const uint32_t location = compiler.get_decoration(stageInput.id, spv::DecorationLocation);
            if (location < static_cast<uint32_t>(bgfx::Attrib::Count))
            {
                AppendBytes(vertexBytes, bgfx::attribToId(static_cast<bgfx::Attrib::Enum>(location)));
            }

            // Map from symbolName -> originalName to associate babylon.js shader attribute -> Babylon Native attribute location.
            // Instance-data inputs are still exposed here so the consumer can bind their vertex buffers.
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
        const spirv_cross::SmallVector<spirv_cross::Resource>& samplers{SelectSamplers(resources, samplerResources)};
        size_t numUniforms = uniformsInfo.Uniforms.size() + samplers.size();

        AppendBytes(fragmentBytes, BX_MAKEFOURCC('F', 'S', 'H', BGFX_SHADER_BIN_VERSION));
        AppendBytes(fragmentBytes, vertexOutputsHash);
        AppendBytes(fragmentBytes, fragmentInputsHash);
        // Raw SRV/UAV masks (bgfx v12). BN runtime shaders don't expose raw buffers here.
        AppendBytes(fragmentBytes, static_cast<uint32_t>(0));
        AppendBytes(fragmentBytes, static_cast<uint32_t>(0));

        AppendBytes(fragmentBytes, static_cast<uint16_t>(numUniforms));
        AppendUniformBuffer(fragmentBytes, uniformsInfo, true);
        appendSamplers(fragmentBytes, compiler, fragmentShaderInfo.Parser->get_parsed_ir(), samplers, bgfxShaderInfo.UniformStages);

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
