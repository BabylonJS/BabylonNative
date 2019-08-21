#include "ShaderCompiler.h"
#include "ResourceLimits.h"
#include <arcana/experimental/array.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_glsl.hpp>
#include "Console.h"

namespace babylon
{
    extern const TBuiltInResource DefaultTBuiltInResource;

    namespace
    {
        void AddShader(glslang::TProgram& program, glslang::TShader& shader, std::string_view source)
        {
            const std::array<const char*, 1> sources{ source.data() };
            shader.setStrings(sources.data(), gsl::narrow_cast<int>(sources.size()));
            shader.setEnvInput(glslang::EShSourceGlsl, shader.getStage(), glslang::EShClientOpenGL, 100);
            shader.setEnvClient(glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);
            shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

            // TODO: Do this to avoid the work around for dFdy?
            if (!shader.parse(&DefaultTBuiltInResource, 430, false, EShMsgDefault))
            {
                Console::Error(shader.getInfoDebugLog());
                throw std::exception();
            }

            program.addShader(&shader);
        }

        std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
        {
            size_t start_pos = 0;
            while ((start_pos = str.find(from, start_pos)) != std::string::npos)
            {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
            return str;
        }

        std::unique_ptr<spirv_cross::Compiler> CompileShader(glslang::TProgram& program, EShLanguage stage, std::string& glsl)
        {
            std::vector<uint32_t> spirv;
            glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

            spirv_cross::Parser parser{ std::move(spirv) };
            parser.parse();

            auto compiler = std::make_unique<spirv_cross::CompilerGLSL>(parser.get_parsed_ir());

            // remove "uniform Frame { .... }" . Keep the uniforms
            const std::string frame = "uniform Frame {";
            const std::string frameEnd = "};";
            size_t pos = glsl.find(frame);
            if (pos != std::string::npos)
            {
                glsl.replace(pos, frame.size(), "");
                pos = glsl.find(frameEnd, pos);
                if (pos != std::string::npos)
                {
                    glsl.replace(pos, frameEnd.size(), "");
                }
            }

#ifndef ANDROID
            glsl = std::string("#version 430\n") + glsl;
#endif
            
            return std::move(compiler);
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

    void ShaderCompiler::Compile(std::string_view vertexSource, std::string_view fragmentSource, std::function<void(ShaderInfo, ShaderInfo)> onCompiled)
    {
        glslang::TProgram program;

        glslang::TShader vertexShader{ EShLangVertex };
        AddShader(program, vertexShader, vertexSource);

        glslang::TShader fragmentShader{ EShLangFragment };
        AddShader(program, fragmentShader, fragmentSource);

        if (!program.link(EShMsgDefault))
        {
            Console::Error(program.getInfoDebugLog());
            throw std::exception();
        }

        std::string vertexGLSL(vertexSource.data(), vertexSource.size());
        auto vertexCompiler = CompileShader(program, EShLangVertex, vertexGLSL);

        std::string fragmentGLSL(fragmentSource.data(), fragmentSource.size());
        auto fragmentCompiler = CompileShader(program, EShLangFragment, fragmentGLSL);

        uint8_t* strVertex = (uint8_t*)vertexGLSL.data();
        uint8_t* strFragment = (uint8_t*)fragmentGLSL.data();
        onCompiled
        (
            { std::move(vertexCompiler), gsl::make_span(strVertex, vertexGLSL.size()) },
            { std::move(fragmentCompiler), gsl::make_span(strFragment, fragmentGLSL.size()) }
        );
    }
}
