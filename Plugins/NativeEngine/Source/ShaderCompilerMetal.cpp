#include "ShaderCompiler.h"
#include "ShaderCompilerCommon.h"
#include "ShaderCompilerTraversers.h"
#include "ResourceLimits.h"
#include <arcana/experimental/array.h>
#include <bgfx/bgfx.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_msl.hpp>

namespace Babylon
{
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

        std::pair<std::unique_ptr<spirv_cross::Parser>, std::unique_ptr<spirv_cross::Compiler>> CompileShader(glslang::TProgram& program, EShLanguage stage, std::string& shaderResult)
        {
            std::vector<uint32_t> spirv;
            glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

            auto parser = std::make_unique<spirv_cross::Parser>(std::move(spirv));
            parser->parse();

            auto compiler = std::make_unique<spirv_cross::CompilerMSL>(parser->get_parsed_ir());

            auto resources = compiler->get_shader_resources();
            for (auto& resource : resources.uniform_buffers)
            {
                compiler->set_name(resource.id, "_mtl_u");
            }

            // Disable location for varying stage to force compiler to do it with variable names
            if (stage == EShLangVertex)
            {
                for (auto& output : resources.stage_outputs)
                {
                    compiler->set_decoration(output.id, spv::DecorationLocation, -1);
                }
            }
            else
            {
              for (auto& input : resources.stage_inputs)
                {
                    compiler->set_decoration(input.id, spv::DecorationLocation, -1);
                }
            }
            
            // rename textures without the 'texture' suffix so it's bindable from .js
            for (auto& resource : resources.separate_images)
            {
                std::string imageName = resource.name;
                if (imageName.find("Texture") != std::string::npos)
                {
                    imageName.replace(imageName.find("Texture"), std::string::npos, "");
                    compiler->set_name(resource.id, imageName);
                }
            }
            
            compiler->rename_entry_point("main", "xlatMtlMain", (stage == EShLangVertex) ? spv::ExecutionModelVertex : spv::ExecutionModelFragment);

            shaderResult = compiler->compile();
            return{std::move(parser), std::move(compiler)};
        }
    }
}

namespace Babylon
{
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
        InvertYDerivativeOperands(fragmentShader);

        glslang::SpvVersion spv{};
        spv.spv = 0x10000;
        vertexShader.getIntermediate()->setSpv(spv);
        fragmentShader.getIntermediate()->setSpv(spv);

        if (!program.link(EShMsgDefault))
        {
            throw std::exception();//program.getInfoDebugLog());
        }

        ShaderCompilerTraversers::IdGenerator ids{};
        auto cutScope = ShaderCompilerTraversers::ChangeUniformTypes(program, ids);
        auto utstScope = ShaderCompilerTraversers::MoveNonSamplerUniformsIntoStruct(program, ids);
        ShaderCompilerTraversers::AssignLocationsAndNamesToVertexVaryings(program, ids);
        ShaderCompilerTraversers::SplitSamplersIntoSamplersAndTextures(program, ids);

        std::string vertexGLSL(vertexSource.data(), vertexSource.size());
        auto [vertexParser, vertexCompiler] = CompileShader(program, EShLangVertex, vertexGLSL);

        std::string fragmentGLSL(fragmentSource.data(), fragmentSource.size());
        auto [fragmentParser, fragmentCompiler] = CompileShader(program, EShLangFragment, fragmentGLSL);

        return ShaderCompilerCommon::CreateBgfxShader(
            {std::move(vertexParser), std::move(vertexCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(vertexGLSL.data()), vertexGLSL.size())},
            {std::move(fragmentParser), std::move(fragmentCompiler), gsl::make_span(reinterpret_cast<uint8_t*>(fragmentGLSL.data()), fragmentGLSL.size())});
    }
}
