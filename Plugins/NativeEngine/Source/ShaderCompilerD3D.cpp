#include "ShaderCompiler.h"
#include "ResourceLimits.h"
#include <arcana/experimental/array.h>
#include <bgfx/bgfx.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_hlsl.hpp>
#include <d3dcompiler.h>
#include <wrl/client.h>

#include <sstream>

namespace Babylon
{
    namespace
    {
        template<typename ...Ts> struct types { using type = types<Ts...>; };
        template<typename T> struct _pointer_vector : std::vector<std::unique_ptr<T>> {};
        template<typename ...> struct _pointer_scope_builder;
        template<typename T, typename ...Ts> struct _pointer_scope_builder<_pointer_vector<T>, Ts...> { using type = types<_pointer_vector<T>, Ts...>; };
        template<typename T, typename... Ts> struct _pointer_scope_builder<T, Ts...> : _pointer_scope_builder<Ts..., _pointer_vector<T>> {};
        template<typename... Ts> struct pointer_scope : pointer_scope<typename _pointer_scope_builder<Ts...>::type> {};
        template<typename... Ts> struct pointer_scope<types<Ts...>> : private Ts...
        {
            template<typename T, typename ...ArgsT>
            T* make_new(ArgsT... args)
            {
                auto& vector = *static_cast<std::vector<std::unique_ptr<T>>*>(this);
                vector.emplace_back(std::make_unique<T>(args...));
                return vector.back().get();
            }
        };

        using namespace glslang;

        struct UniformToStructTraverser final : TIntermTraverser
        {
            UniformToStructTraverser()
                : TIntermTraverser()
            {
            }

            ~UniformToStructTraverser() override
            {
            }

            void visitSymbol(TIntermSymbol* symbol) override
            {
                const auto addToSymbolsToParents = [symbol, this]() { SymbolsToParents.emplace_back(symbol, this->getParentNode()); };
                if (symbol->getType().getQualifier().isUniformOrBuffer())
                {
                    UniformNameToSymbol[symbol->getName().c_str()] = symbol;
                    addToSymbolsToParents();
                }
                else if (IsVertex && symbol->getType().getQualifier().storage == EvqVaryingIn)
                {
                    VaryingNameToSymbol[symbol->getName().c_str()] = symbol;
                    addToSymbolsToParents();
                }
            }

            uint32_t texCoordIdx = 10;
            unsigned int GetVaryingLocationForName(const char* name)
            {
                if (std::strcmp(name, "position") == 0)
                {
                    return 0;
                }
                else if (std::strcmp(name, "normal") == 0)
                {
                    return 1;
                }
                else
                {
                    return texCoordIdx++;
                }
            }

            template<typename ScopeT>
            static UniformToStructTraverser Traverse(glslang::TIntermediate* intermediate, ScopeT& scope, bool isVertex = false)
            {
                UniformToStructTraverser traverser{};
                traverser.IsVertex = isVertex;
                intermediate->getTreeRoot()->traverse(&traverser);

                // Default loc we don't care about.
                TSourceLoc loc{};
                loc.init();

                // Build the struct
                std::vector<std::string> originalNames{};
                auto* structMembers = scope.make_new<TTypeList>();

                TPublicType publicType{};
                publicType.qualifier.clearLayout();

                for (const auto& [name, symbol] : traverser.UniformNameToSymbol)
                {
                    const auto& type = symbol->getType();
                    if (type.isMatrix())
                    {
                        publicType.setMatrix(type.getMatrixCols(), type.getMatrixRows());
                    }
                    else if (type.isVector())
                    {
                        publicType.setVector(type.getVectorSize());
                    }

                    originalNames.emplace_back(name);

                    auto* newType = scope.make_new<TType>(publicType);
                    newType->setFieldName(name.c_str()); // TODO: Replace names here as necessary
                    newType->setBasicType(symbol->getType().getBasicType());
                    structMembers->emplace_back();
                    structMembers->back().type = newType;
                    structMembers->back().loc.init();
                }

                TQualifier qualifier{};
                qualifier.clearLayout();
                qualifier.storage = EvqUniform;
                qualifier.layoutMatrix = ElmColumnMajor;
                qualifier.layoutPacking = ElpStd140;
                qualifier.layoutBinding = 0; // Determines which cbuffer it's bounds to (b0, b1, b2, etc.)

                TType structType(structMembers, "Frame", qualifier);

                TIntermSymbol* structSymbol = intermediate->addSymbol(structType, loc);

                // Build the struct indexer operations.
                std::map<std::string, TIntermTyped*> originalNameToReplacement{};
                for (size_t idx = 0; idx < structMembers->size(); ++idx)
                {
                    auto* left = structSymbol;
                    auto* right = intermediate->addConstantUnion(idx, loc);
                    auto* binary = intermediate->addBinaryNode(EOpIndexDirectStruct, left, right, loc);
                    originalNameToReplacement[originalNames[idx]] = binary;
                }

                // Add replacers for varyings.
                for (const auto& [name, symbol] : traverser.VaryingNameToSymbol)
                {
                    publicType.qualifier = symbol->getType().getQualifier();
                    publicType.qualifier.layoutLocation = traverser.GetVaryingLocationForName(name.c_str());

                    const auto& type = symbol->getType();
                    if (type.isMatrix())
                    {
                        publicType.setMatrix(type.getMatrixCols(), type.getMatrixRows());
                    }
                    else if (type.isVector())
                    {
                        publicType.setVector(type.getVectorSize());
                    }

                    originalNames.emplace_back(name);

                    TType newType{publicType};
                    newType.setBasicType(symbol->getType().getBasicType());
                    auto* newSymbol = intermediate->addSymbol(TIntermSymbol{symbol->getId(), symbol->getName(), newType});
                    originalNameToReplacement[name] = newSymbol;
                }

                for (const auto& [symbol, parent] : traverser.SymbolsToParents)
                {
                    auto* replacement = originalNameToReplacement[symbol->getName().c_str()];
                    if (auto* aggregate = parent->getAsAggregate())
                    {
                        auto& sequence = aggregate->getSequence();
                        for (int idx = 0; idx < sequence.size(); ++idx)
                        {
                            if (sequence[idx] == symbol)
                            {
                                sequence[idx] = replacement;
                            }
                        }
                    }
                    else if (auto* binary = parent->getAsBinaryNode())
                    {
                        if (binary->getLeft() == symbol)
                        {
                            binary->setLeft(replacement);
                        }
                        else
                        {
                            binary->setRight(replacement);
                        }
                    }
                    else if (auto* unary = parent->getAsUnaryNode())
                    {
                        unary->setOperand(replacement);
                    }
                    else
                    {
                        throw std::runtime_error{"Cannot replace symbol: node type handler unimplemented"};
                    }
                }

                return traverser;
            }

            bool IsVertex{};
            std::map<std::string, TIntermSymbol*> UniformNameToSymbol{};
            std::map<std::string, TIntermSymbol*> VaryingNameToSymbol{};
            std::vector<std::pair<TIntermSymbol*, TIntermNode*>> SymbolsToParents{};
        };
    }

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

        std::unique_ptr<spirv_cross::Compiler> CompileShader(glslang::TProgram& program, EShLanguage stage, gsl::span<const spirv_cross::HLSLVertexAttributeRemap> attributes, ID3DBlob** blob)
        {
            std::vector<uint32_t> spirv;
            glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

            spirv_cross::Parser parser{std::move(spirv)};
            parser.parse();

            auto compiler = std::make_unique<spirv_cross::CompilerHLSL>(parser.get_parsed_ir());
            compiler->set_hlsl_options({40});

            for (const auto& attribute : attributes)
            {
                compiler->add_vertex_attribute_remap(attribute);
            }

            std::string hlsl = compiler->compile();

            Microsoft::WRL::ComPtr<ID3DBlob> errorMsgs;
            const char* target = stage == EShLangVertex ? "vs_4_0" : "ps_4_0";

            UINT flags = 0;

#ifdef _DEBUG
            flags |= D3DCOMPILE_DEBUG;
#endif

            if (FAILED(D3DCompile(hlsl.data(), hlsl.size(), nullptr, nullptr, nullptr, "main", target, flags, 0, blob, &errorMsgs)))
            {
                throw std::exception(static_cast<const char*>(errorMsgs->GetBufferPointer()));
            }

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
        InvertYDerivativeOperands(fragmentShader);
        
        // TODO: Do stuff here.
        glslang::SpvVersion spv{};
        spv.spv = 0x10000;
        vertexShader.getIntermediate()->setSpv(spv);
        fragmentShader.getIntermediate()->setSpv(spv);

        if (!program.link(EShMsgDefault))
        {
            throw std::exception(program.getInfoDebugLog());
        }

        pointer_scope<TType, TTypeList> scope{};
        UniformToStructTraverser::Traverse(program.getIntermediate(EShLangVertex), scope, true);
        UniformToStructTraverser::Traverse(program.getIntermediate(EShLangFragment), scope);

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

        Microsoft::WRL::ComPtr<ID3DBlob> vertexBlob;
        auto vertexCompiler = CompileShader(program, EShLangVertex, attributes, &vertexBlob);
        ShaderInfo vertexShaderInfo{
            std::move(vertexCompiler),
            gsl::make_span(static_cast<uint8_t*>(vertexBlob->GetBufferPointer()), vertexBlob->GetBufferSize())};

        Microsoft::WRL::ComPtr<ID3DBlob> fragmentBlob;
        auto fragmentCompiler = CompileShader(program, EShLangFragment, {}, &fragmentBlob);
        ShaderInfo fragmentShaderInfo{
            std::move(fragmentCompiler),
            gsl::make_span(static_cast<uint8_t*>(fragmentBlob->GetBufferPointer()), fragmentBlob->GetBufferSize())};

        onCompiled(std::move(vertexShaderInfo), std::move(fragmentShaderInfo));
    }
}
