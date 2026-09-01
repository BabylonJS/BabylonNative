#include <Babylon/Plugins/ShaderCompiler.h>

#include "ShaderCompilerCommon.h"
#include "ShaderCompilerTraversers.h"
#include <arcana/experimental/array.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Include/intermediate.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_glsl.hpp>
#include <bgfx/bgfx.h>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#define BGFX_UNIFORM_SAMPLERBIT UINT8_C(0x20) // Copy-pasta from bgfx_p.h


namespace
{
    // Match bgfx/src/shader.h binding layout used by the Vulkan backend for shader
    // binary v11+ (m_oldBindingModel == false). UBOs occupy bindings 0/1; images
    // start at kSpirvBindShift; separate samplers sit kSpirvSamplerShift above
    // their paired image. See shaderc_spirv.cpp setShiftBinding(...) and
    // renderer_vk.cpp (regIndex - kSpirvBindShift).
    constexpr unsigned kSpirvVertexBinding = 0;
    constexpr unsigned kSpirvFragmentBinding = 1;
    constexpr unsigned kSpirvBindShift = 2;
    constexpr unsigned kSpirvSamplerShift = 16;
    // bgfx Vulkan m_bindInfo is indexed by sampler stage 0..15.
    constexpr unsigned kMaxBgfxTextureStages = 16;

    void CollectStageUniforms(
        glslang::TIntermediate* intermediate,
        std::vector<glslang::TIntermSymbol*>& textures,
        std::vector<glslang::TIntermSymbol*>& pureSamplers,
        std::vector<glslang::TIntermSymbol*>& ubos)
    {
        if (intermediate == nullptr)
        {
            return;
        }

        auto* root = intermediate->getTreeRoot()->getAsAggregate();
        if (root == nullptr || root->getSequence().empty())
        {
            return;
        }

        auto* linkerObjects = root->getSequence().back()->getAsAggregate();
        if (linkerObjects == nullptr)
        {
            return;
        }

        for (glslang::TIntermNode* node : linkerObjects->getSequence())
        {
            auto* symbol = node->getAsSymbolNode();
            if (symbol == nullptr)
            {
                continue;
            }

            const auto& type = symbol->getType();
            if (type.getQualifier().storage != glslang::EvqUniform)
            {
                continue;
            }

            if (type.getBasicType() == glslang::EbtSampler)
            {
                const auto& sampler = type.getSampler();
                if (sampler.sampler)
                {
                    pureSamplers.push_back(symbol);
                }
                else
                {
                    textures.push_back(symbol);
                }
            }
            else
            {
                ubos.push_back(symbol);
            }
        }
    }

    void ApplyBgfxVulkanResourceBindings(glslang::TProgram& program)
    {
        // Count distinct sampler slots shared across VS/FS (SamplerSplitter assigns one
        // layoutBinding per unique name). bgfx only has 16 texture stages.
        {
            std::set<unsigned> uniqueSlots;
            auto collectSlots = [&](glslang::TIntermediate* intermediate) {
                std::vector<glslang::TIntermSymbol*> textures;
                std::vector<glslang::TIntermSymbol*> pureSamplers;
                std::vector<glslang::TIntermSymbol*> ubos;
                CollectStageUniforms(intermediate, textures, pureSamplers, ubos);
                for (auto* texture : textures)
                {
                    uniqueSlots.insert(texture->getType().getQualifier().layoutBinding);
                }
            };
            collectSlots(program.getIntermediate(EShLangVertex));
            collectSlots(program.getIntermediate(EShLangFragment));
            if (uniqueSlots.size() > kMaxBgfxTextureStages)
            {
                throw std::runtime_error{
                    "Vulkan shader uses more than 16 distinct sampler textures; bgfx supports at most 16 texture stages"};
            }
        }

        auto applyStage = [](glslang::TIntermediate* intermediate, unsigned uboBinding) {
            std::vector<glslang::TIntermSymbol*> textures;
            std::vector<glslang::TIntermSymbol*> pureSamplers;
            std::vector<glslang::TIntermSymbol*> ubos;
            CollectStageUniforms(intermediate, textures, pureSamplers, ubos);

            // SamplerSplitterTraverser assigns one shared layoutBinding per sampler name
            // across VS/FS. Preserve that slot and only apply bgfx's fixed shifts —
            // do not compact per-stage (different subsets would desync the same
            // sampler across stages and overwrite UniformStages).
            for (auto* texture : textures)
            {
                const unsigned originalBinding = texture->getType().getQualifier().layoutBinding;
                const unsigned imageBinding = kSpirvBindShift + originalBinding;
                texture->getWritableType().getQualifier().layoutBinding = imageBinding;

                std::string texName = texture->getName().c_str();
                std::string baseName = texName;
                constexpr char kSuffix[] = "Texture";
                constexpr size_t kSuffixLen = sizeof(kSuffix) - 1;
                if (baseName.size() > kSuffixLen && baseName.compare(baseName.size() - kSuffixLen, kSuffixLen, kSuffix) == 0)
                {
                    baseName.resize(baseName.size() - kSuffixLen);
                }

                for (auto* samplerSymbol : pureSamplers)
                {
                    if (std::string{samplerSymbol->getName().c_str()} == baseName)
                    {
                        samplerSymbol->getWritableType().getQualifier().layoutBinding = imageBinding + kSpirvSamplerShift;
                        break;
                    }
                }
            }

            for (auto* ubo : ubos)
            {
                // Frame is the UBO synthesized by MoveNonSamplerUniformsIntoStruct.
                // Extra user UBOs are rare on the Vulkan path; leave their existing
                // bindings alone so we do not collide with the image range.
                if (std::string{ubo->getName().c_str()} == "Frame" || ubos.size() == 1)
                {
                    ubo->getWritableType().getQualifier().layoutBinding = uboBinding;
                }
            }
        };

        applyStage(program.getIntermediate(EShLangVertex), kSpirvVertexBinding);
        applyStage(program.getIntermediate(EShLangFragment), kSpirvFragmentBinding);
    }

    // Vulkan-only bgfx sampler packaging: separate_images with Texture-suffix stripped,
    // regIndex = image descriptor binding, UniformStages = regIndex - kSpirvBindShift.
    void AppendSamplersVulkan(
        std::vector<uint8_t>& bytes,
        const spirv_cross::Compiler& compiler,
        const spirv_cross::ParsedIR& originalIr,
        const spirv_cross::SmallVector<spirv_cross::Resource>& samplers,
        std::map<std::string, uint8_t>& stages)
    {
        using namespace Babylon::ShaderCompilerCommon;

        for (const spirv_cross::Resource& sampler : samplers)
        {
            const std::string& originalName = originalIr.get_name(sampler.id);
            std::string name = originalName.empty() ? sampler.name : originalName;

            // Separate images are named with a "Texture" suffix by SplitSamplersIntoSamplersAndTextures.
            // bgfx/shaderc strip that suffix so the packaged uniform name matches the original GLSL
            // sampler identifier (and therefore Babylon.js).
            constexpr char kTextureSuffix[] = "Texture";
            constexpr size_t kTextureSuffixLen = sizeof(kTextureSuffix) - 1;
            if (name.size() > kTextureSuffixLen && name.compare(name.size() - kTextureSuffixLen, kTextureSuffixLen, kTextureSuffix) == 0)
            {
                name.resize(name.size() - kTextureSuffixLen);
            }

            AppendBytes(bytes, static_cast<uint8_t>(name.size()));
            AppendBytes(bytes, name);
            AppendBytes(bytes, static_cast<uint8_t>(bgfx::UniformType::Sampler | BGFX_UNIFORM_SAMPLERBIT));

            // createShader computes stage = regIndex - kSpirvBindShift; writing 0 underflows.
            const uint8_t num{0};
            const uint16_t regIndex{static_cast<uint16_t>(compiler.get_decoration(sampler.id, spv::DecorationBinding))};
            const uint16_t regCount{0};
            AppendBytes(bytes, num);
            AppendBytes(bytes, regIndex);
            AppendBytes(bytes, regCount);
            AppendUniformTextureMeta(bytes, /*texComponent*/ 0, TextureDimensionIdFromResource(compiler, sampler));

            stages[name] = regIndex >= kSpirvBindShift
                ? static_cast<uint8_t>(regIndex - kSpirvBindShift)
                : static_cast<uint8_t>(0);
        }
    }

    void AddShader(glslang::TProgram& program, glslang::TShader& shader, std::string_view source)
    {
        const std::array<const char*, 1> sources{source.data()};
        shader.setStrings(sources.data(), gsl::narrow_cast<int>(sources.size()));

        auto defaultTBuiltInResource = GetDefaultResources();

        if (!shader.parse(defaultTBuiltInResource, 310, EProfile::EEsProfile, true, true, EShMsgDefault))
        {
            throw std::runtime_error(shader.getInfoLog());
        }

        program.addShader(&shader);
    }

    std::pair<std::unique_ptr<spirv_cross::Parser>, std::unique_ptr<spirv_cross::Compiler>> CompileShader(glslang::TProgram& program, EShLanguage stage, std::vector<uint32_t>& spirv)
    {
        spv::SpvBuildLogger logger;
        glslang::SpvOptions spvOptions;
        spvOptions.validate = true;
        spvOptions.disableOptimizer = true;
        glslang::GlslangToSpv(*program.getIntermediate(stage), spirv, &logger, &spvOptions);

        auto parser = std::make_unique<spirv_cross::Parser>(spirv);
        parser->parse();

        auto compiler = std::make_unique<spirv_cross::CompilerGLSL>(parser->get_parsed_ir());
        return {std::move(parser), std::move(compiler)};
    }
}

namespace Babylon::Plugins
{
    using namespace ShaderCompilerCommon;

    ShaderCompiler::ShaderCompiler()
    {
        glslang::InitializeProcess();
    }

    ShaderCompiler::~ShaderCompiler()
    {
        glslang::FinalizeProcess();
    }

    Graphics::BgfxShaderInfo ShaderCompiler::Compile(std::string_view vertexSource, std::string_view fragmentSource, const std::map<std::string, uint32_t>& instancedAttributes)
    {
        glslang::TProgram program;

        glslang::TShader vertexShader{EShLangVertex};
        AddShader(program, vertexShader, ProcessSamplerFlip(ProcessShaderCoordinates(vertexSource)));

        glslang::TShader fragmentShader{EShLangFragment};
        AddShader(program, fragmentShader, ProcessSamplerFlip(fragmentSource));

        glslang::SpvVersion spv{};
        spv.spv = 0x10000;
        vertexShader.getIntermediate()->setSpv(spv);
        fragmentShader.getIntermediate()->setSpv(spv);

        if (!program.link(EShMsgDefault))
        {
            throw std::runtime_error{program.getInfoLog()};
        }

        ShaderCompilerTraversers::IdGenerator ids{};
        // Flip 2D texture sample coordinates (replaces the former ProcessSamplerFlip texture() macro).
        ShaderCompilerTraversers::FlipSamplerCoordinates(program);
        auto cutScope = ShaderCompilerTraversers::ChangeUniformTypes(program, ids);
        auto utstScope = ShaderCompilerTraversers::MoveNonSamplerUniformsIntoStruct(program, ids);
        std::map<std::string, std::string> vertexAttributeRenaming = {};
        auto builtInInstanceDataSlots = ShaderCompilerTraversers::AssignLocationsAndNamesToVertexVaryingsD3D(program, ids, vertexAttributeRenaming, instancedAttributes);
        ShaderCompilerTraversers::SplitSamplersIntoSamplersAndTextures(program, ids);
        ShaderCompilerTraversers::SplitSamplerFunctionParameters(program, ids);
        ApplyBgfxVulkanResourceBindings(program);
        ShaderCompilerTraversers::ZeroInitializeStructLocals(program);
        ShaderCompilerTraversers::InvertYDerivativeOperands(program);

        std::vector<uint32_t> spirvVS;
        auto [vertexParser, vertexCompiler] = CompileShader(program, EShLangVertex, spirvVS);

        std::vector<uint32_t> spirvFS;
        auto [fragmentParser, fragmentCompiler] = CompileShader(program, EShLangFragment, spirvFS);

        return CreateBgfxShader(
            {std::move(vertexParser), std::move(vertexCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(spirvVS.data()), spirvVS.size() * sizeof(uint32_t)), std::move(vertexAttributeRenaming)},
            {std::move(fragmentParser), std::move(fragmentCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(spirvFS.data()), spirvFS.size() * sizeof(uint32_t)), {}},
            std::move(builtInInstanceDataSlots),
            SamplerResourceSet::SeparateImages,
            AppendSamplersVulkan);
    }
}
