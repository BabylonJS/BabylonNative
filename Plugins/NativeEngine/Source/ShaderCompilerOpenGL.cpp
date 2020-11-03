#include "ShaderCompiler.h"
#include "ShaderCompilerCommon.h"
#include "ShaderCompilerTraversers.h"
#include "ResourceLimits.h"
#include <arcana/experimental/array.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_glsl.hpp>

namespace Babylon
{
    extern const TBuiltInResource DefaultTBuiltInResource;

    namespace
    {
        void AddShader(glslang::TProgram& program, glslang::TShader& shader, std::string_view source)
        {
            const std::array<const char*, 1> sources{source.data()};
            shader.setStrings(sources.data(), gsl::narrow_cast<int>(sources.size()));

            if (!shader.parse(&DefaultTBuiltInResource, 310, EProfile::EEsProfile, true, true, EShMsgDefault))
            {
                throw std::runtime_error(shader.getInfoDebugLog());
            }

            program.addShader(&shader);
        }

        std::pair<std::unique_ptr<spirv_cross::Parser>, std::unique_ptr<spirv_cross::Compiler>> CompileShader(glslang::TProgram& program, EShLanguage stage, std::string& glsl)
        {
            std::vector<uint32_t> spirv;
            glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

            auto parser = std::make_unique<spirv_cross::Parser>(std::move(spirv));
            parser->parse();

            auto compiler = std::make_unique<spirv_cross::CompilerGLSL>(parser->get_parsed_ir());

            spirv_cross::CompilerGLSL::Options options = compiler->get_common_options();

            options.version = 300;
            options.es = true;

            compiler->set_common_options(options);

            glsl = compiler->compile();

            return{std::move(parser), std::move(compiler)};
        }
    }

    ShaderCompiler::ShaderCompiler()
    {
        glslang::InitializeProcess();
    }

    ShaderCompiler::~ShaderCompiler()
    {
        glslang::FinalizeProcess();
    }

    ShaderCompiler::BgfxShaderInfo ShaderCompiler::Compile(std::string_view vertexSource, std::string_view fragmentSource)
    {
        glslang::TProgram program;

        glslang::TShader vertexShader{EShLangVertex};
        AddShader(program, vertexShader, vertexSource);

        glslang::TShader fragmentShader{EShLangFragment};
        AddShader(program, fragmentShader, fragmentSource);

        glslang::SpvVersion spv{};
        spv.spv = 0x10000;
        vertexShader.getIntermediate()->setSpv(spv);
        fragmentShader.getIntermediate()->setSpv(spv);

        if (!program.link(EShMsgDefault))
        {
            throw std::exception();
        }

        ShaderCompilerTraversers::IdGenerator ids{};
        auto cutScope = ShaderCompilerTraversers::ChangeUniformTypes(program, ids);
        std::unordered_map<std::string, std::string> vertexAttributeRenaming = {};
        ShaderCompilerTraversers::AssignLocationsAndNamesToVertexVaryings(program, ids, vertexAttributeRenaming);

        std::string vertexGLSL(vertexSource.data(), vertexSource.size());
        auto [vertexParser, vertexCompiler] = CompileShader(program, EShLangVertex, vertexGLSL);

        std::string fragmentGLSL(fragmentSource.data(), fragmentSource.size());
        auto [fragmentParser, fragmentCompiler] = CompileShader(program, EShLangFragment, fragmentGLSL);

        return ShaderCompilerCommon::CreateBgfxShader(
            {std::move(vertexParser), std::move(vertexCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(vertexGLSL.data()), vertexGLSL.size()), std::move(vertexAttributeRenaming)},
            {std::move(fragmentParser), std::move(fragmentCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(fragmentGLSL.data()), fragmentGLSL.size()), {}});
    }
}
