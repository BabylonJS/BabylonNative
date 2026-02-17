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
            }
        }

        compiler->rename_entry_point("main", "xlatMtlMain", (stage == EShLangVertex) ? spv::ExecutionModelVertex : spv::ExecutionModelFragment);

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

    Graphics::BgfxShaderInfo ShaderCompiler::Compile(std::string_view vertexSource, std::string_view fragmentSource)
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
        auto cutScope = ShaderCompilerTraversers::ChangeUniformTypes(program, ids);
        auto utstScope = ShaderCompilerTraversers::MoveNonSamplerUniformsIntoStruct(program, ids);
        std::map<std::string, std::string> vertexAttributeRenaming = {};
        ShaderCompilerTraversers::AssignLocationsAndNamesToVertexVaryingsMetal(program, ids, vertexAttributeRenaming);
        ShaderCompilerTraversers::SplitSamplersIntoSamplersAndTextures(program, ids);
        ShaderCompilerTraversers::InvertYDerivativeOperands(program);

        std::string vertexMSL(vertexSource.data(), vertexSource.size());
        auto [vertexParser, vertexCompiler] = CompileShader(program, EShLangVertex, vertexMSL);

        std::string fragmentMSL(fragmentSource.data(), fragmentSource.size());
        auto [fragmentParser, fragmentCompiler] = CompileShader(program, EShLangFragment, fragmentMSL);

        return CreateBgfxShader(
            {std::move(vertexParser), std::move(vertexCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(vertexMSL.data()), vertexMSL.size()), std::move(vertexAttributeRenaming)},
            {std::move(fragmentParser), std::move(fragmentCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(fragmentMSL.data()), fragmentMSL.size()), {}});
    }
}
