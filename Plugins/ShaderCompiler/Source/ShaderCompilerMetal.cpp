#include <Babylon/Plugins/ShaderCompiler.h>

#include "ShaderCompilerCommon.h"
#include "ShaderCompilerTraversers.h"
#include <arcana/experimental/array.h>
#include <bgfx/bgfx.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_msl.hpp>

namespace
{
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

    std::pair<std::unique_ptr<spirv_cross::Parser>, std::unique_ptr<spirv_cross::Compiler>> CompileShader(glslang::TProgram& program, EShLanguage stage, std::string& shaderResult)
    {
        std::vector<uint32_t> spirv;
        glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

        auto parser = std::make_unique<spirv_cross::Parser>(std::move(spirv));
        parser->parse();

        auto compiler = std::make_unique<spirv_cross::CompilerMSL>(parser->get_parsed_ir());

        // Enable decoration for texture binding
        spirv_cross::CompilerMSL::Options opts{};
        opts.enable_decoration_binding = true;
        compiler->set_msl_options(opts);

        auto resources = compiler->get_shader_resources();
        for (auto& resource : resources.uniform_buffers)
        {
            compiler->set_name(resource.id, "_mtl_u");
        }

        if (stage == EShLangCompute)
        {
            // bgfx's Metal compute binding: uniforms in buffer 0, storage buffer stage N in
            // buffer N + 1, and a texture/sampler at its stage. Compute keeps combined samplers.
            // Each resource kind gets its own descriptor set, so the generated uniform block (which
            // has no binding, i.e. 0) and storage binding 0 do not share one remapping key.
            const auto bind = [&](const spirv_cross::Resource& resource, uint32_t set, uint32_t buffer, uint32_t texture) {
                compiler->set_decoration(resource.id, spv::DecorationDescriptorSet, set);
                spirv_cross::MSLResourceBinding binding{};
                binding.stage = spv::ExecutionModelGLCompute;
                binding.desc_set = set;
                binding.binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
                binding.msl_buffer = buffer;
                binding.msl_texture = texture;
                binding.msl_sampler = texture;
                compiler->add_msl_resource_binding(binding);
            };
            for (const auto& resource : resources.uniform_buffers)
            {
                bind(resource, 0, 0, 0);
            }
            for (const auto& resource : resources.storage_buffers)
            {
                bind(resource, 1, compiler->get_decoration(resource.id, spv::DecorationBinding) + 1, 0);
            }
            for (const auto& resource : resources.sampled_images)
            {
                bind(resource, 2, 0, compiler->get_decoration(resource.id, spv::DecorationBinding));
            }
        }

        // WebGL shaders use a single sampler uniform, Metal separates it into a sampler and a texture, appending "Texture" and "Sampler" to the original WebGL names.
        // Remove 'Texture' from the uniform name to match JS expected names.
        for (auto& resource : resources.separate_images)
        {
            std::string imageName = resource.name;
            if (imageName.find("Texture") != std::string::npos)
            {
                // Using rfind ensures the correct "Texture" is removed from WebGL uniforms with "texture" in their original name. (e.g., "shadowTexture1")
                imageName.replace(imageName.rfind("Texture"), std::string::npos, "");
                compiler->set_name(resource.id, imageName);

                // The Compiler transpiles a private copy of the IR, so rename the parser's copy
                // too: AppendSamplers reads the sampler's app-facing name from there.
                parser->get_parsed_ir().set_name(resource.id, imageName);
            }
        }

        const auto executionModel = stage == EShLangVertex ? spv::ExecutionModelVertex
            : stage == EShLangFragment                     ? spv::ExecutionModelFragment
                                                           : spv::ExecutionModelGLCompute;
        compiler->rename_entry_point("main", "xlatMtlMain", executionModel);

        shaderResult = compiler->compile();
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
        auto vertexText = PreprocessShader(EShLangVertex, vertexSource);
        auto fragmentText = PreprocessShader(EShLangFragment, fragmentSource);
        auto uniformNames = RenameShaderUniforms(vertexText, &fragmentText);
        vertexSource = vertexText;
        fragmentSource = fragmentText;
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
        ShaderCompilerTraversers::FlipSamplerCoordinates(program, ids);
        // Must precede the uniform struct move, which collects the uniform this declares.
        ShaderCompilerTraversers::FlipFragCoordY(program, ids);
        auto cutScope = ShaderCompilerTraversers::ChangeUniformTypes(program, ids);
        auto utstScope = ShaderCompilerTraversers::MoveNonSamplerUniformsIntoStruct(program, ids);
        std::map<std::string, std::string> vertexAttributeRenaming = {};
        auto builtInInstanceDataSlots = ShaderCompilerTraversers::AssignLocationsAndNamesToVertexVaryingsMetal(program, ids, vertexAttributeRenaming, instancedAttributes);
        ShaderCompilerTraversers::AssignInterStageVaryingLocations(program, ids);
        ShaderCompilerTraversers::SplitSamplersIntoSamplersAndTextures(program, ids);
        ShaderCompilerTraversers::SplitSamplerFunctionParameters(program, ids);
        ShaderCompilerTraversers::ZeroInitializeStructLocals(program);

        std::string vertexMSL(vertexSource.data(), vertexSource.size());
        auto [vertexParser, vertexCompiler] = CompileShader(program, EShLangVertex, vertexMSL);

        std::string fragmentMSL(fragmentSource.data(), fragmentSource.size());
        auto [fragmentParser, fragmentCompiler] = CompileShader(program, EShLangFragment, fragmentMSL);

        auto result = CreateBgfxShader(
            {std::move(vertexParser), std::move(vertexCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(vertexMSL.data()), vertexMSL.size()), std::move(vertexAttributeRenaming)},
            {std::move(fragmentParser), std::move(fragmentCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(fragmentMSL.data()), fragmentMSL.size()), {}},
            std::move(builtInInstanceDataSlots));
        result.UniformNames = std::move(uniformNames);
        return result;
    }

    Graphics::BgfxShaderInfo ShaderCompiler::CompileCompute(std::string_view computeSource)
    {
        auto source = PreprocessShader(EShLangCompute, computeSource);
        auto uniformNames = RenameShaderUniforms(source);
        computeSource = source;
        glslang::TProgram program;

        glslang::TShader computeShader{EShLangCompute};
        AddShader(program, computeShader, ProcessSamplerFlip(computeSource));

        glslang::SpvVersion spv{};
        spv.spv = 0x10000;
        computeShader.getIntermediate()->setSpv(spv);

        if (!program.link(EShMsgDefault))
        {
            throw std::runtime_error{program.getInfoLog()};
        }

        ShaderCompilerTraversers::IdGenerator ids{};
        ShaderCompilerTraversers::FlipSamplerCoordinates(program, ids);
        auto cutScope = ShaderCompilerTraversers::ChangeUniformTypes(program, ids);
        auto utstScope = ShaderCompilerTraversers::MoveNonSamplerUniformsIntoStruct(program, ids);
        ShaderCompilerTraversers::ZeroInitializeStructLocals(program);

        std::string computeMSL;
        auto [computeParser, computeCompiler] = CompileShader(program, EShLangCompute, computeMSL);

        auto result = CreateBgfxComputeShader(
            {std::move(computeParser), std::move(computeCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(computeMSL.data()), computeMSL.size()), {}});
        result.UniformNames = std::move(uniformNames);
        return result;
    }
}
