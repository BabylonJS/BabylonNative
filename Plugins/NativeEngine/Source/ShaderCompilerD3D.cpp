#include "ShaderCompiler.h"
#include "ResourceLimits.h"
#include <arcana/experimental/array.h>
#include <bgfx/bgfx.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <glslang/MachineIndependent/RemoveTree.h>
#include <spirv_parser.hpp>
#include <spirv_hlsl.hpp>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <gsl/gsl>

namespace Babylon
{
    namespace
    {
        using namespace glslang;

        void makeReplacements(
            std::map<std::string, TIntermTyped*> nameToReplacement,
            std::vector<std::pair<TIntermSymbol*, TIntermNode*>> symbolToParent)
        {
            for (const auto& [symbol, parent] : symbolToParent)
            {
                auto* replacement = nameToReplacement[symbol->getName().c_str()];
                if (auto* aggregate = parent->getAsAggregate())
                {
                    auto& sequence = aggregate->getSequence();
                    for (int idx = 0; idx < sequence.size(); ++idx)
                    {
                        if (sequence[idx] == symbol)
                        {
                            RemoveAllTreeNodes(sequence[idx]);
                            sequence[idx] = replacement;
                        }
                    }
                }
                else if (auto* binary = parent->getAsBinaryNode())
                {
                    if (binary->getLeft() == symbol)
                    {
                        RemoveAllTreeNodes(binary->getLeft());
                        binary->setLeft(replacement);
                    }
                    else
                    {
                        RemoveAllTreeNodes(binary->getRight());
                        binary->setRight(replacement);
                    }
                }
                else if (auto* unary = parent->getAsUnaryNode())
                {
                    RemoveAllTreeNodes(unary->getOperand());
                    unary->setOperand(replacement);
                }
                else
                {
                    throw std::runtime_error{"Cannot replace symbol: node type handler unimplemented"};
                }
            }
        }

        class UniformToStructTraverser final : private TIntermTraverser
        {
        public:
            class AllocationsScope
            {
                friend UniformToStructTraverser;
                std::vector<std::unique_ptr<TType>> Types{};
                std::vector<std::unique_ptr<TTypeList>> TypeLists{};
            };

            ~UniformToStructTraverser() override
            {
            }

            void visitSymbol(TIntermSymbol* symbol) override
            {
                if (symbol->getType().getQualifier().isUniformOrBuffer())
                {
                    m_uniformNameToSymbol[symbol->getName().c_str()] = symbol;
                    m_symbolsToParents.emplace_back(symbol, this->getParentNode());
                }
            }

            static AllocationsScope Traverse(TProgram& program)
            {
                AllocationsScope scope{};
                Traverse(program.getIntermediate(EShLangVertex), scope);
                Traverse(program.getIntermediate(EShLangFragment), scope);
                return scope;
            }

        private:
            static void Traverse(glslang::TIntermediate* intermediate, AllocationsScope& scope)
            {
                UniformToStructTraverser traverser{};
                intermediate->getTreeRoot()->traverse(&traverser);

                std::map<std::string, TIntermTyped*> originalNameToReplacement{};

                // Precursor types needed to create subtree replacements.
                TSourceLoc loc{};
                loc.init();
                TPublicType publicType{};
                publicType.qualifier.clearLayout();

                std::vector<std::string> originalNames{};
                scope.TypeLists.emplace_back(std::make_unique<TTypeList>());
                auto* structMembers = scope.TypeLists.back().get();

                for (const auto& [name, symbol] : traverser.m_uniformNameToSymbol)
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

                    scope.Types.emplace_back(std::make_unique<TType>(publicType));
                    auto* newType = scope.Types.back().get();
                    newType->setFieldName(name.c_str());
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

                for (size_t idx = 0; idx < structMembers->size(); ++idx)
                {
                    auto* left = structSymbol;
                    auto* right = intermediate->addConstantUnion(idx, loc);
                    auto* binary = intermediate->addBinaryNode(EOpIndexDirectStruct, left, right, loc);
                    originalNameToReplacement[(*structMembers)[idx].type->getFieldName().c_str()] = binary;
                }

                makeReplacements(originalNameToReplacement, traverser.m_symbolsToParents);
            }

            std::map<std::string, TIntermSymbol*> m_uniformNameToSymbol{};
            std::vector<std::pair<TIntermSymbol*, TIntermNode*>> m_symbolsToParents{};
        };

        class VertexVaryingInTraverser final : private TIntermTraverser
        {
        public:
            ~VertexVaryingInTraverser() override
            {
            }

            void visitSymbol(TIntermSymbol* symbol) override
            {
                if (symbol->getType().getQualifier().storage == EvqVaryingIn)
                {
                    m_varyingNameToSymbol[symbol->getName().c_str()] = symbol;
                    m_symbolsToParents.emplace_back(symbol, this->getParentNode());
                }
            }

            static void Traverse(TProgram& program)
            {
                Traverse(program.getIntermediate(EShLangVertex));
            }

        private:
            unsigned int GetVaryingLocationForName(const char* name)
            {
#define IF_NAME_RETURN_ATTRIB(varyingName, attrib)             \
                if (std::strcmp(name, varyingName) == 0)       \
                {                                              \
                    return static_cast<unsigned int>(attrib);  \
                }
                IF_NAME_RETURN_ATTRIB("position", bgfx::Attrib::Position)
                IF_NAME_RETURN_ATTRIB("normal", bgfx::Attrib::Normal)
                IF_NAME_RETURN_ATTRIB("tangent", bgfx::Attrib::Tangent)
                IF_NAME_RETURN_ATTRIB("uv", bgfx::Attrib::TexCoord0)
                IF_NAME_RETURN_ATTRIB("uv2", bgfx::Attrib::TexCoord1)
                IF_NAME_RETURN_ATTRIB("uv3", bgfx::Attrib::TexCoord2)
                IF_NAME_RETURN_ATTRIB("uv4", bgfx::Attrib::TexCoord3)
                IF_NAME_RETURN_ATTRIB("color", bgfx::Attrib::Color0)
                IF_NAME_RETURN_ATTRIB("matricesIndices", bgfx::Attrib::Indices)
                IF_NAME_RETURN_ATTRIB("matricesWeights", bgfx::Attrib::Weight)
#undef IF_NAME_RETURN_ATTRIB
                return FIRST_GENERIC_ATTRIBUTE_LOCATION + m_genericAttributesRunningCount++;
            }

            static void Traverse(glslang::TIntermediate* intermediate)
            {
                VertexVaryingInTraverser traverser{};
                intermediate->getTreeRoot()->traverse(&traverser);

                std::map<std::string, TIntermTyped*> originalNameToReplacement{};

                // Precursor types needed to create subtree replacements.
                TSourceLoc loc{};
                loc.init();
                TPublicType publicType{};
                publicType.qualifier.clearLayout();

                for (const auto& [name, symbol] : traverser.m_varyingNameToSymbol)
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

                    TType newType{publicType};
                    newType.setBasicType(symbol->getType().getBasicType());
                    auto* newSymbol = intermediate->addSymbol(TIntermSymbol{symbol->getId(), symbol->getName(), newType});
                    originalNameToReplacement[name] = newSymbol;
                }

                makeReplacements(originalNameToReplacement, traverser.m_symbolsToParents);
            }

            const unsigned int FIRST_GENERIC_ATTRIBUTE_LOCATION{10}; // TODO: Is this right? There are returnable values in the list that are larger than 10.
            unsigned int m_genericAttributesRunningCount{0};
            std::map<std::string, TIntermSymbol*> m_varyingNameToSymbol{};
            std::vector<std::pair<TIntermSymbol*, TIntermNode*>> m_symbolsToParents{};
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

        glslang::SpvVersion spv{};
        spv.spv = 0x10000;
        vertexShader.getIntermediate()->setSpv(spv);
        fragmentShader.getIntermediate()->setSpv(spv);

        if (!program.link(EShMsgDefault))
        {
            throw std::exception(program.getInfoDebugLog());
        }

        auto utstScope = UniformToStructTraverser::Traverse(program);
        VertexVaryingInTraverser::Traverse(program);

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
