#if _MSC_VER 
#pragma warning( disable : 4100 ) // unreferenced formal parameter in glslang header
#endif

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

namespace Babylon
{
    namespace
    {
        using namespace glslang;

        class IdGenerator
        {
        public:
            IdGenerator() = default;
            IdGenerator(const IdGenerator&) = delete;

            int Next()
            {
                return ++m_lastId;
            }

        private:
            int m_lastId{0};
        };

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

        bool isLinkerObject(const TIntermSequence& path)
        {
            auto* agg = path.size() > 1 ? path[1]->getAsAggregate() : nullptr;
            return agg && agg->getOp() == EOpLinkerObjects;
        }

        class UniformToStructTraverser final : private TIntermTraverser
        {
        public:
            class AllocationsScope
            {
                friend UniformToStructTraverser;
                std::vector<std::unique_ptr<TType>> Types{};
                std::vector<std::unique_ptr<TTypeList>> TypeLists{};
                std::vector<std::unique_ptr<TArraySizes>> ArraySizes{};
            };

            ~UniformToStructTraverser() override
            {
            }

            void visitSymbol(TIntermSymbol* symbol) override
            {
                if (symbol->getType().getQualifier().isUniformOrBuffer() && symbol->getType().getBasicType() != EbtSampler)
                {
                    if (isLinkerObject(this->path))
                    {
                        m_uniformNameToSymbol[symbol->getName().c_str()] = symbol;
                    }
                    else
                    {
                        m_symbolsToParents.emplace_back(symbol, this->getParentNode());
                    }
                }
            }

            static AllocationsScope Traverse(TProgram& program, IdGenerator& ids)
            {
                AllocationsScope scope{};
                Traverse(program.getIntermediate(EShLangVertex), ids, scope);
                Traverse(program.getIntermediate(EShLangFragment), ids, scope);
                return scope;
            }

        private:
            static void Traverse(glslang::TIntermediate* intermediate, IdGenerator& ids, AllocationsScope& scope)
            {
                UniformToStructTraverser traverser{};
                intermediate->getTreeRoot()->traverse(&traverser);

                std::map<std::string, TIntermTyped*> originalNameToReplacement{};

                // Precursor types needed to create subtree replacements.
                TSourceLoc loc{};
                loc.init();
                TPublicType publicType{};
                publicType.qualifier.clearLayout();
                publicType.qualifier.storage = EvqUniform;
                publicType.qualifier.precision = EpqHigh;
                publicType.qualifier.layoutMatrix = ElmColumnMajor;
                publicType.qualifier.layoutPacking = ElpStd140;

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

                    if (type.getArraySizes())
                    {
                        scope.ArraySizes.emplace_back(std::make_unique<TArraySizes>());
                        publicType.arraySizes = scope.ArraySizes.back().get();
                        *publicType.arraySizes = *type.getArraySizes();
                    }
                    else
                    {
                        publicType.arraySizes = nullptr;
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

                TIntermSymbol* structSymbol = intermediate->addSymbol(TIntermSymbol{ids.Next(), "anon@0", structType});

                for (unsigned int idx = 0; idx < structMembers->size(); ++idx)
                {
                    auto& memberType = (*structMembers)[idx].type;

                    auto* left = structSymbol;
                    auto* right = intermediate->addConstantUnion(idx, loc);
                    auto* binary = intermediate->addBinaryNode(EOpIndexDirectStruct, left, right, loc);
                    binary->setType(*memberType);
                    originalNameToReplacement[memberType->getFieldName().c_str()] = binary;
                }

                // Perform linker object replacements
                auto* linkerObjectAggregate = intermediate->getTreeRoot()->getAsAggregate()->getSequence().back()->getAsAggregate();
                assert(linkerObjectAggregate->getOp() == EOpLinkerObjects);
                auto& sequence = linkerObjectAggregate->getSequence();
                for (int idx = static_cast<int>(sequence.size()) - 1; idx >= 0; --idx)
                {
                    auto* symbol = sequence[idx]->getAsSymbolNode();
                    if (symbol)
                    {
                        auto found = traverser.m_uniformNameToSymbol.find(symbol->getName().c_str());
                        if (found != traverser.m_uniformNameToSymbol.end())
                        {
                            RemoveAllTreeNodes(symbol);
                            sequence.erase(sequence.begin() + idx);
                        }
                    }
                }
                sequence.insert(sequence.begin(), structSymbol);

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
                    if (isLinkerObject(this->path))
                    {
                        m_varyingNameToSymbol[symbol->getName().c_str()] = symbol;
                    }

                    // Because the symbol replacement for varyings is just a new symbol with the
                    // correct parameters, we can just do the linker object replacement alongside
                    // the other replacements, so we add the occurrence here regardless of whether
                    // we're in a linker object.
                    m_symbolsToParents.emplace_back(symbol, this->getParentNode());
                }
            }

            static void Traverse(TProgram& program, IdGenerator& ids)
            {
                Traverse(program.getIntermediate(EShLangVertex), ids);
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

            static void Traverse(glslang::TIntermediate* intermediate, IdGenerator& ids)
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
                    const auto& type = symbol->getType();
                    publicType.qualifier = type.getQualifier();
                    publicType.qualifier.layoutLocation = traverser.GetVaryingLocationForName(name.c_str());

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
                    auto* newSymbol = intermediate->addSymbol(TIntermSymbol{ids.Next(), symbol->getName(), newType});
                    originalNameToReplacement[name] = newSymbol;
                }

                makeReplacements(originalNameToReplacement, traverser.m_symbolsToParents);
            }

            const unsigned int FIRST_GENERIC_ATTRIBUTE_LOCATION{10}; // TODO: Is this right? There are returnable values in the list that are larger than 10.
            unsigned int m_genericAttributesRunningCount{0};
            std::map<std::string, TIntermSymbol*> m_varyingNameToSymbol{};
            std::vector<std::pair<TIntermSymbol*, TIntermNode*>> m_symbolsToParents{};
        };

        class SamplerSplitterTraverser : TIntermTraverser
        {
        public:
            ~SamplerSplitterTraverser() override
            {
            }

            void visitSymbol(TIntermSymbol* symbol) override
            {
                if (symbol->getType().getQualifier().storage == EvqUniform && symbol->getType().getBasicType() == EbtSampler)
                {
                    if (isLinkerObject(this->path))
                    {
                        m_samplerNameToSymbol[symbol->getName().c_str()] = symbol;
                    }
                    else
                    {
                        m_symbolsToParents.emplace_back(symbol, this->getParentNode());
                    }
                }
            }

            static void Traverse(TProgram& program, IdGenerator& ids)
            {
                Traverse(program.getIntermediate(EShLangVertex), ids);
                Traverse(program.getIntermediate(EShLangFragment), ids);
            }

        private:
            static void Traverse(TIntermediate* intermediate, IdGenerator& ids)
            {
                SamplerSplitterTraverser traverser{};
                intermediate->getTreeRoot()->traverse(&traverser);

                TSourceLoc loc{};
                loc.init();

                std::map<std::string, TIntermTyped*> nameToReplacement{};
                std::map<std::string, std::pair<TIntermSymbol*, TIntermSymbol*>> nameToNewTextureAndSampler{};

                unsigned int layoutBinding = 0;
                for (const auto& [name, symbol] : traverser.m_samplerNameToSymbol)
                {
                    // For each name and symbol, create a replacer.
                    const auto& type = symbol->getType();

                    // Create the new texture symbol.
                    TIntermSymbol* newTexture;
                    {
                        TPublicType publicType{};
                        publicType.qualifier.clearLayout();
                        publicType.basicType = type.getBasicType();
                        publicType.qualifier = type.getQualifier();
                        publicType.qualifier.precision = EpqHigh;
                        publicType.qualifier.layoutBinding = layoutBinding;
                        publicType.sampler = type.getSampler();
                        publicType.sampler.combined = false;

                        TType newType{publicType};
                        std::string newName = name + "Texture";
                        newTexture = intermediate->addSymbol(TIntermSymbol{ids.Next(), newName.c_str(), newType});
                    }

                    // Create the new sampler symbol.
                    TIntermSymbol* newSampler;
                    {
                        TPublicType publicType{};
                        publicType.qualifier.clearLayout();
                        publicType.basicType = type.getBasicType();
                        publicType.qualifier = type.getQualifier();
                        publicType.qualifier.precision = EpqHigh;
                        publicType.qualifier.layoutBinding = layoutBinding;
                        publicType.sampler.sampler = true;

                        TType newType{publicType};
                        newSampler = intermediate->addSymbol(TIntermSymbol{ids.Next(), name.c_str(), newType});
                    }

                    nameToNewTextureAndSampler[name] = std::pair<TIntermSymbol*, TIntermSymbol*>{newTexture, newSampler};

                    // Create the aggregate.
                    auto* aggregate = intermediate->growAggregate(newTexture, newSampler);
                    {
                        aggregate->setOperator(EOpConstructTextureSampler);

                        TPublicType publicType{};
                        publicType.basicType = type.getBasicType();
                        publicType.qualifier.clearLayout();
                        publicType.qualifier.storage = EvqTemporary;
                        publicType.sampler = type.getSampler();
                        publicType.sampler.combined = true;
                        aggregate->setType(TType{publicType});
                    }

                    nameToReplacement[name] = aggregate;
                    ++layoutBinding;
                }

                // Perform linker object replacements
                auto* linkerObjectAggregate = intermediate->getTreeRoot()->getAsAggregate()->getSequence().back()->getAsAggregate();
                assert(linkerObjectAggregate->getOp() == EOpLinkerObjects);
                auto& sequence = linkerObjectAggregate->getSequence();
                for (int idx = static_cast<int>(sequence.size()) - 1; idx >= 0; --idx)
                {
                    auto* symbol = sequence[idx]->getAsSymbolNode();
                    if (symbol)
                    {
                        auto found = nameToNewTextureAndSampler.find(symbol->getName().c_str());
                        if (found != nameToNewTextureAndSampler.end())
                        {
                            RemoveAllTreeNodes(symbol);
                            sequence[idx] = found->second.first;
                            sequence.insert(sequence.begin() + idx + 1, found->second.second);
                        }
                    }
                }

                makeReplacements(nameToReplacement, traverser.m_symbolsToParents);
            }

            std::map<std::string, TIntermSymbol*> m_samplerNameToSymbol{};
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

        IdGenerator ids{};
        auto utstScope = UniformToStructTraverser::Traverse(program, ids);
        VertexVaryingInTraverser::Traverse(program, ids);
        SamplerSplitterTraverser::Traverse(program, ids);

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
