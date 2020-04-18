#include "ShaderCompiler.h"
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
            shader.setEnvInput(glslang::EShSourceGlsl, shader.getStage(), glslang::EShClientVulkan, 100);
            shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
            shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

            if (!shader.parse(&DefaultTBuiltInResource, 450, false, EShMsgDefault))
            {
                throw std::exception();
            }

            program.addShader(&shader);
        }

        std::unique_ptr<spirv_cross::Compiler> CompileShader(glslang::TProgram& program, EShLanguage stage, std::string& glsl)
        {
            std::vector<uint32_t> spirv;
            glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

            spirv_cross::Parser parser{std::move(spirv)};
            parser.parse();

            auto compiler = std::make_unique<spirv_cross::CompilerGLSL>(parser.get_parsed_ir());

            compiler->build_combined_image_samplers();

            spirv_cross::CompilerGLSL::Options options = compiler->get_common_options();
#if defined(ANDROID)  //|| defined(__linux__) 
            options.version = 300;
            options.es = true;
#else
            options.version = 140;
            options.es = false;
#endif
            compiler->set_common_options(options);

            // rebuild uniform lists with the correct type
            program.buildReflection();
            int numUniforms = program.getNumUniformVariables();

            for (int i = 0; i < numUniforms; i++)
            {
                const auto& uniform = program.getUniform(i);
                std::string uniformString = "uniform highp ";
                switch (uniform.glDefineType)
                {
                    case 0x8B5C: //GL_FLOAT_MAT4
                        uniformString += "mat4 ";
                        break;
                    case 0:      // no type : skip
                    case 0x8B5D: // GL_SAMPLER_1D
                    case 0x8B5E: // GL_SAMPLER_2D
                    case 0x8B5F: // GL_SAMPLER_3D
                    case 0x8B60: // GL_SAMPLER_CUBE
                        continue;
                    default:
                        uniformString += "vec4 ";
                        break;
                }
                uniformString += uniform.name;
                uniformString += ";";
                compiler->add_header_line(uniformString);
            }

            std::string compiled = compiler->compile();

            // rename "uniform Frame { .... }" . Keep the uniforms
            const std::string frameNewName = ((stage == EShLangVertex) ? "FrameVS" : "FrameFS");
            const std::string frame = "Frame";
            size_t pos = compiled.find(frame);
            if (pos != std::string::npos)
            {
                compiled.replace(pos, frame.size(), frameNewName);
            }

            spirv_cross::ShaderResources resources = compiler->get_shader_resources();
            for (auto& resource : resources.uniform_buffers)
            {
                auto str = resource.name;
                auto fbn = compiler->get_fallback_name(resource.id);
                std::string rep = fbn + ".";
                size_t pos = compiled.find(rep);
                while (pos != std::string::npos)
                {
                    compiled.replace(pos, rep.size(), "");
                    pos = compiled.find(rep);
                }
            }

            auto combined = compiler->get_combined_image_samplers();
            for (auto resource : resources.separate_samplers)
            {
                auto& samplerName = resource.name;
                auto id = resource.id;
                for (auto combine : combined)
                {
                    if (combine.sampler_id == id)
                    {
                        id = combine.combined_id;
                        break;
                    }
                }
                auto& fbn = compiler->get_fallback_name(id);
                size_t pos = compiled.find(fbn);
                while (pos != std::string::npos)
                {
                    compiled.replace(pos, fbn.size(), samplerName);
                    pos = compiled.find(fbn);
                }
            }

#if defined(ANDROID) //|| defined(__linux__) 
            glsl = compiled.substr(strlen("#version 300 es\n"));

            // frag def
            static const std::string fragDef = "layout(location = 0) out highp vec4 glFragColor;";
            pos = glsl.find(fragDef);
            if (pos != std::string::npos)
            {
                glsl.replace(pos, fragDef.size(), "");
            }

            // frag
            static const std::string fragColor = "glFragColor";
            pos = glsl.find(fragColor);
            if (pos != std::string::npos)
            {
                glsl.replace(pos, fragColor.size(), "gl_FragColor");
            }
#else
{
const std::string version = "#version 140";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "");
            }
            glsl = compiled;
}
{
const std::string version = "layout(binding = 0";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "/*layout(binding = 0");
            }
            glsl = compiled;
}
{
const std::string version = "} _22;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _22;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _17;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _17;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _205;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _205;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _292;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _292;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _28;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _28;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _97;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _97;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _664;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _664;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _210;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _210;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _513;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _210;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _25;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _210;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _575;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _210;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _33;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _210;*/");
            }
            glsl = compiled;
}
{
const std::string version = "} _680;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _210;*/");
            }
            glsl = compiled;
}
{
const std::string version = "#extension";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "//#extension");
            }
            glsl = compiled;
}


   {
const std::string version = "} _69;";
            size_t posv = compiled.find(version);
            if (posv != std::string::npos)
            {
                compiled.replace(posv, version.size(), "} _69;*/");
            }
            glsl = compiled;
}     

#endif
//printf("%s\n", glsl.c_str());
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

        glslang::TShader vertexShader{EShLangVertex};
        AddShader(program, vertexShader, vertexSource);

        glslang::TShader fragmentShader{EShLangFragment};
        AddShader(program, fragmentShader, fragmentSource);

        if (!program.link(EShMsgDefault))
        {
            throw std::exception();
        }

        std::string vertexGLSL(vertexSource.data(), vertexSource.size());
        auto vertexCompiler = CompileShader(program, EShLangVertex, vertexGLSL);

        std::string fragmentGLSL(fragmentSource.data(), fragmentSource.size());
        auto fragmentCompiler = CompileShader(program, EShLangFragment, fragmentGLSL);

        uint8_t* strVertex = (uint8_t*)vertexGLSL.data();
        uint8_t* strFragment = (uint8_t*)fragmentGLSL.data();
        onCompiled(
            {std::move(vertexCompiler), gsl::make_span(strVertex, vertexGLSL.size())},
            {std::move(fragmentCompiler), gsl::make_span(strFragment, fragmentGLSL.size())});
    }
}
