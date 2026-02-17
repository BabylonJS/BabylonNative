#include <Babylon/Plugins/ShaderCompiler.h>

#include "ShaderCompilerCommon.h"
#include "ShaderCompilerTraversers.h"
#include <arcana/experimental/array.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_glsl.hpp>
#include <bgfx/bgfx.h>

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
        ShaderCompilerTraversers::AssignLocationsAndNamesToVertexVaryingsD3D(program, ids, vertexAttributeRenaming);
        ShaderCompilerTraversers::SplitSamplersIntoSamplersAndTextures(program, ids);
        ShaderCompilerTraversers::InvertYDerivativeOperands(program);

        std::vector<uint32_t> spirvVS;
        auto [vertexParser, vertexCompiler] = CompileShader(program, EShLangVertex, spirvVS);

        std::vector<uint32_t> spirvFS;
        auto [fragmentParser, fragmentCompiler] = CompileShader(program, EShLangFragment, spirvFS);

        return CreateBgfxShader(
            {std::move(vertexParser), std::move(vertexCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(spirvVS.data()), spirvVS.size() * sizeof(uint32_t)), std::move(vertexAttributeRenaming)},
            {std::move(fragmentParser), std::move(fragmentCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(spirvFS.data()), spirvFS.size() * sizeof(uint32_t)), {}});
    }
}
