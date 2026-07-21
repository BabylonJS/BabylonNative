#include <Babylon/Plugins/ShaderCompiler.h>

#include "ShaderCompilerCommon.h"
#include "ShaderCompilerTraversers.h"
#include <bgfx/bgfx.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_hlsl.hpp>
#include <d3dcompiler.h>
#include <wrl/client.h>

namespace
{
    void AddShader(glslang::TProgram& program, glslang::TShader& shader, std::string_view source)
    {
        const std::array<const char*, 1> sources{source.data()};
        shader.setStrings(sources.data(), gsl::narrow_cast<int>(sources.size()));

        auto defaultTBuiltInResource = GetDefaultResources();

        if (!shader.parse(defaultTBuiltInResource, 310, EProfile::EEsProfile, true, true, EShMsgDefault))
        {
            throw std::runtime_error{shader.getInfoLog()};
        }

        program.addShader(&shader);
    }

    std::pair<std::unique_ptr<spirv_cross::Parser>, std::unique_ptr<spirv_cross::Compiler>> CompileShader(glslang::TProgram& program, EShLanguage stage, gsl::span<const spirv_cross::HLSLVertexAttributeRemap> attributes, ID3DBlob** blob)
    {
        std::vector<uint32_t> spirv;
        glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

        auto parser = std::make_unique<spirv_cross::Parser>(std::move(spirv));
        parser->parse();

        auto compiler = std::make_unique<spirv_cross::CompilerHLSL>(parser->get_parsed_ir());

        compiler->set_hlsl_options({40, true});

        for (const auto& attribute : attributes)
        {
            compiler->add_vertex_attribute_remap(attribute);
        }

        Babylon::ShaderCompilerCommon::AssignUniformBufferBindings(*compiler);

        std::string hlsl = compiler->compile();

        Microsoft::WRL::ComPtr<ID3DBlob> errorMsgs;
        const char* target = stage == EShLangVertex ? "vs_5_0" : "ps_5_0";

        UINT flags = 0;

#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif

        if (FAILED(D3DCompile(hlsl.data(), hlsl.size(), nullptr, nullptr, nullptr, "main", target, flags, 0, blob, &errorMsgs)))
        {
            throw std::runtime_error{static_cast<const char*>(errorMsgs->GetBufferPointer())};
        }

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
        ShaderCompilerTraversers::AssignLocationsAndNamesToVertexVaryingsD3D(program, ids, vertexAttributeRenaming, instancedAttributes);
        ShaderCompilerTraversers::SplitSamplersIntoSamplersAndTextures(program, ids);
        ShaderCompilerTraversers::SplitSamplerFunctionParameters(program, ids);
        ShaderCompilerTraversers::ZeroInitializeStructLocals(program);
        ShaderCompilerTraversers::InvertYDerivativeOperands(program);

        // clang-format off
        static const spirv_cross::HLSLVertexAttributeRemap attributes[] = {
            {bgfx::Attrib::Position,  "POSITION"    },
            {bgfx::Attrib::Normal,    "NORMAL"      },
            {bgfx::Attrib::Tangent,   "TANGENT"     },
            {bgfx::Attrib::Color0,    "COLOR"       },
            {bgfx::Attrib::Indices,   "BLENDINDICES"},
            {bgfx::Attrib::Weight,    "BLENDWEIGHT" },
            {bgfx::Attrib::TexCoord0, "TEXCOORD0"   },
            {bgfx::Attrib::TexCoord1, "TEXCOORD1"   },
            {bgfx::Attrib::TexCoord2, "TEXCOORD2"   },
            {bgfx::Attrib::TexCoord3, "TEXCOORD3"   },
            {bgfx::Attrib::TexCoord4, "TEXCOORD4"   },
            {bgfx::Attrib::TexCoord5, "TEXCOORD5"   },
            {bgfx::Attrib::TexCoord6, "TEXCOORD6"   },
            {bgfx::Attrib::TexCoord7, "TEXCOORD7"   },
            {bgfx::Attrib::TexCoord8, "TEXCOORD8"   },
            {bgfx::Attrib::TexCoord9, "TEXCOORD9"   },
            {bgfx::Attrib::TexCoord10, "TEXCOORD10" },
            {bgfx::Attrib::TexCoord11, "TEXCOORD11" },
            {bgfx::Attrib::TexCoord12, "TEXCOORD12" },
            {bgfx::Attrib::TexCoord13, "TEXCOORD13" },
            {bgfx::Attrib::TexCoord14, "TEXCOORD14" },
            {bgfx::Attrib::TexCoord15, "TEXCOORD15" },
            // Per-instance data (i_data0..i_data15) occupies the top TEXCOORD semantics
            // (TEXCOORD31..16), which have no bgfx::Attrib enum. The shader traversers assign
            // them synthetic locations of (bgfx::Attrib::TexCoord0 + semanticIndex); map those
            // back so bgfx's instance-data layout (TEXCOORD31 == i_data0, descending) binds them.
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 16, "TEXCOORD16"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 17, "TEXCOORD17"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 18, "TEXCOORD18"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 19, "TEXCOORD19"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 20, "TEXCOORD20"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 21, "TEXCOORD21"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 22, "TEXCOORD22"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 23, "TEXCOORD23"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 24, "TEXCOORD24"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 25, "TEXCOORD25"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 26, "TEXCOORD26"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 27, "TEXCOORD27"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 28, "TEXCOORD28"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 29, "TEXCOORD29"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 30, "TEXCOORD30"},
            {Babylon::Plugins::TEXCOORD0_ATTRIBUTE_LOCATION + 31, "TEXCOORD31"},
        };
        // clang-format on

        Microsoft::WRL::ComPtr<ID3DBlob> vertexBlob;
        auto [vertexParser, vertexCompiler] = CompileShader(program, EShLangVertex, attributes, &vertexBlob);
        ShaderInfo vertexShaderInfo{
            std::move(vertexParser),
            std::move(vertexCompiler),
            gsl::make_span(static_cast<uint8_t*>(vertexBlob->GetBufferPointer()), vertexBlob->GetBufferSize()),
            std::move(vertexAttributeRenaming)};

        Microsoft::WRL::ComPtr<ID3DBlob> fragmentBlob;
        auto [fragmentParser, fragmentCompiler] = CompileShader(program, EShLangFragment, {}, &fragmentBlob);
        ShaderInfo fragmentShaderInfo{
            std::move(fragmentParser),
            std::move(fragmentCompiler),
            gsl::make_span(static_cast<uint8_t*>(fragmentBlob->GetBufferPointer()), fragmentBlob->GetBufferSize()),
            {}};

        return CreateBgfxShader(std::move(vertexShaderInfo), std::move(fragmentShaderInfo));
    }
}
