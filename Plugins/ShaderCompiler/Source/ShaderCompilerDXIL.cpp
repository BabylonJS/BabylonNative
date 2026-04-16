#include <Babylon/Plugins/ShaderCompiler.h>

#include "ShaderCompilerCommon.h"
#include "ShaderCompilerTraversers.h"
#include <bgfx/bgfx.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_hlsl.hpp>
#include <wrl/client.h>
#include <dxcapi.h>

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

    struct DxcCompilerState
    {
        // Order matters: ComPtrs must be destroyed before FreeLibrary so COM
        // object vtables (which live inside dxcompiler.dll) remain valid during
        // Release(). Members are destroyed in reverse declaration order.
        struct ModuleDeleter
        {
            void operator()(HMODULE m) const noexcept
            {
                if (m != nullptr)
                {
                    FreeLibrary(m);
                }
            }
        };
        std::unique_ptr<std::remove_pointer_t<HMODULE>, ModuleDeleter> Module;
        Microsoft::WRL::ComPtr<IDxcCompiler3> Compiler;
    };

    // Thread-safe lazy initialization via C++11 magic statics.
    DxcCompilerState& GetDxcCompiler()
    {
        static DxcCompilerState state = []() {
            DxcCompilerState s;

            s.Module.reset(LoadLibraryExW(L"dxcompiler.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS));
            if (!s.Module)
            {
                throw std::runtime_error{"Failed to load dxcompiler.dll"};
            }

            auto createInstance = reinterpret_cast<DxcCreateInstanceProc>(
                GetProcAddress(s.Module.get(), "DxcCreateInstance"));
            if (!createInstance)
            {
                throw std::runtime_error{"Failed to find DxcCreateInstance in dxcompiler.dll"};
            }

            if (FAILED(createInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&s.Compiler))))
            {
                throw std::runtime_error{"Failed to create IDxcCompiler3"};
            }

            return s;
        }();
        return state;
    }

    std::pair<std::unique_ptr<spirv_cross::Parser>, std::unique_ptr<spirv_cross::Compiler>> CompileShader(glslang::TProgram& program, EShLanguage stage, gsl::span<const spirv_cross::HLSLVertexAttributeRemap> attributes, IDxcBlob** blob)
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

        std::string hlsl = compiler->compile();

        auto& dxc = GetDxcCompiler();

        const wchar_t* target = stage == EShLangVertex ? L"vs_6_0" : L"ps_6_0";

        std::vector<LPCWSTR> args = {L"-E", L"main", L"-T", target};
#ifdef _DEBUG
        args.push_back(L"-Zi");
        args.push_back(L"-Od");
#endif

        DxcBuffer sourceBuffer{};
        sourceBuffer.Ptr = hlsl.data();
        sourceBuffer.Size = hlsl.size();
        sourceBuffer.Encoding = DXC_CP_UTF8;

        Microsoft::WRL::ComPtr<IDxcResult> result;
        if (FAILED(dxc.Compiler->Compile(
            &sourceBuffer,
            args.data(),
            static_cast<UINT32>(args.size()),
            nullptr,
            IID_PPV_ARGS(&result))))
        {
            throw std::runtime_error{"DXC compilation call failed"};
        }

        HRESULT status;
        result->GetStatus(&status);
        if (FAILED(status))
        {
            Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors;
            result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
            throw std::runtime_error{errors && errors->GetStringLength() > 0
                ? errors->GetStringPointer()
                : "DXC compilation failed"};
        }

        if (FAILED(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(blob), nullptr)) || *blob == nullptr)
        {
            throw std::runtime_error{"DXC did not produce a shader object"};
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
        };
        // clang-format on

        Microsoft::WRL::ComPtr<IDxcBlob> vertexBlob;
        auto [vertexParser, vertexCompiler] = CompileShader(program, EShLangVertex, attributes, &vertexBlob);
        ShaderInfo vertexShaderInfo{
            std::move(vertexParser),
            std::move(vertexCompiler),
            gsl::make_span(static_cast<uint8_t*>(vertexBlob->GetBufferPointer()), vertexBlob->GetBufferSize()),
            std::move(vertexAttributeRenaming)};

        Microsoft::WRL::ComPtr<IDxcBlob> fragmentBlob;
        auto [fragmentParser, fragmentCompiler] = CompileShader(program, EShLangFragment, {}, &fragmentBlob);
        ShaderInfo fragmentShaderInfo{
            std::move(fragmentParser),
            std::move(fragmentCompiler),
            gsl::make_span(static_cast<uint8_t*>(fragmentBlob->GetBufferPointer()), fragmentBlob->GetBufferSize()),
            {}};

        return CreateBgfxShader(std::move(vertexShaderInfo), std::move(fragmentShaderInfo));
    }
}
