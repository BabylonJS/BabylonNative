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
                if (symbol->getType().getQualifier().isUniformOrBuffer())
                {
                    NameToSymbol[symbol->getName().c_str()] = symbol;
                    SymbolsToParents.emplace_back(symbol, this->getParentNode());
                }
            }

            static UniformToStructTraverser Traverse(glslang::TIntermediate* intermediate)
            {
                UniformToStructTraverser traverser{};
                intermediate->getTreeRoot()->traverse(&traverser);

                // Default loc we don't care about.
                TSourceLoc loc{};

                // Build the struct
                std::vector<std::string> originalNames{};
                // TODO: Leak less.
                auto* structMembers = new TTypeList();

                TPublicType publicType{};
                publicType.qualifier.clearLayout();

                for (const auto& [name, symbol] : traverser.NameToSymbol)
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

                    // TODO: Leak less.
                    auto* newType = new TType(publicType);
                    newType->setFieldName(name.c_str()); // TODO: Replace names here as necessary
                    newType->setBasicType(symbol->getType().getBasicType());
                    structMembers->emplace_back();
                    structMembers->back().type = newType;
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
                std::map<std::string, TIntermBinary*> originalNameToReplacement{};
                for (size_t idx = 0; idx < structMembers->size(); ++idx)
                {
                    auto* left = structSymbol;
                    auto* right = intermediate->addConstantUnion(idx, loc);
                    auto* binary = intermediate->addBinaryNode(EOpIndexDirectStruct, left, right, loc);
                    originalNameToReplacement[originalNames[idx]] = binary;
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
                    else
                    {
                        throw std::runtime_error{"Cannot replace symbol: node type handler unimplemented"};
                    }
                }

                return traverser;
            }

            std::map<std::string, TIntermSymbol*> NameToSymbol{};
            std::vector<std::pair<TIntermSymbol*, TIntermNode*>> SymbolsToParents{};
        };

        class DebugTraverser : public TIntermTraverser
        {
        public:
            DebugTraverser()
                : TIntermTraverser{}
            {
            }

            ~DebugTraverser() override
            {
            }

            int doStuffNext = 0;
            void visitSymbol(TIntermSymbol* symbol) override
            {
                if (doStuffNext == 0 && std::strcmp(symbol->getName().c_str(), "finalWorld") == 0)
                {
                    doStuffNext = 1;
                }

                Indent();
                ss << "Symbol: " << symbol->getName() << std::endl;
            }

            void visitConstantUnion(TIntermConstantUnion* constUnion) override
            {
                Indent();
                ss << "Constant Union: " << constUnion->getCompleteString().c_str() << std::endl;
            }

            bool visitBinary(TVisit, TIntermBinary* binary) override
            {
                if (binary->getOp() == glslang::EOpIndexDirectStruct && doStuffNext == 1)
                {
                    binary->setLeft(symbol);
                    doStuffNext = 2;
                }

                Indent();
                ss << "Binary: " << binary->getOp() << ", " << binary->getCompleteString().c_str() << std::endl;
                return true;
            }

            bool visitUnary(TVisit, TIntermUnary* unary) override
            {
                Indent();
                ss << "Unary: " << unary->getCompleteString().c_str() << std::endl;
                return true;
            }

            bool visitSelection(TVisit, TIntermSelection* selection) override
            {
                Indent();
                ss << "Selection: " << selection->getCompleteString().c_str() << std::endl;
                return true;
            }

            bool visitAggregate(TVisit, TIntermAggregate* aggregate) override
            {
                Indent();
                ss << "Aggregate: " << aggregate->getCompleteString().c_str() << std::endl;
                return true;
            }

            bool visitLoop(TVisit, TIntermLoop* loop) override
            {
                Indent();
                ss << "Loop: " << loop->getTest()->getCompleteString().c_str() << std::endl;
                return true;
            }

            bool visitBranch(TVisit, TIntermBranch* branch) override
            {
                Indent();
                ss << "Branch: " << branch->getExpression()->getCompleteString().c_str() << std::endl;
                return true;
            }

            bool visitSwitch(TVisit, TIntermSwitch* _switch) override
            {
                Indent();
                ss << "Switch: " << std::endl;
                return true;
            }

            static std::string Traverse(glslang::TIntermediate* intermediate)
            {
                DebugTraverser traverser{};
                traverser.DoStuff(intermediate);
                intermediate->getTreeRoot()->traverse(&traverser);
                return traverser.ss.str();
            }

        private:
            std::stringstream ss{};

            TIntermSymbol* symbol;
            void DoStuff(TIntermediate* intermediate)
            {
                auto* structMembers = new TTypeList();

                // Precursor type required for setting things that are otherwise inaccessible for some reason.
                TPublicType pubMatrix{};
                pubMatrix.qualifier.clearLayout();

                pubMatrix.setMatrix(4, 4);
                auto* matrix = new TType(pubMatrix);
                matrix->setFieldName("fourByFour");
                matrix->setBasicType(TBasicType::EbtFloat);
                structMembers->emplace_back();
                structMembers->back().type = matrix;

                pubMatrix.setMatrix(3, 3);
                auto* otherMatrix = new TType(pubMatrix);
                otherMatrix->setFieldName("threeByThree");
                otherMatrix->setBasicType(TBasicType::EbtFloat);
                structMembers->emplace_back();
                structMembers->back().type = otherMatrix;

                pubMatrix.setVector(3);
                auto* notMatrix = new TType(pubMatrix);
                notMatrix->setFieldName("vector");
                notMatrix->setBasicType(TBasicType::EbtFloat);
                structMembers->emplace_back();
                structMembers->back().type = notMatrix;

                TQualifier qualifier{};
                qualifier.clearLayout();
                qualifier.storage = EvqUniform;
                qualifier.layoutMatrix = ElmColumnMajor;
                qualifier.layoutPacking = ElpStd140;
                qualifier.layoutBinding = 0; // Determines which cbuffer it's bounds to (b0, b1, b2, etc.)

                TType structType(structMembers, "Stuff", qualifier);

                TSourceLoc loc{};
                symbol = intermediate->addSymbol(structType, loc);
            }

            void Indent()
            {
                for (int idx = 0; idx < this->depth; ++idx)
                {
                    ss << "    ";
                }
            }
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

        // auto vrtx = DebugTraverser::Traverse(program.getIntermediate(EShLangVertex));
        // auto fgmt = DebugTraverser::Traverse(program.getIntermediate(EShLangFragment));

        UniformToStructTraverser::Traverse(program.getIntermediate(EShLangVertex));
        UniformToStructTraverser::Traverse(program.getIntermediate(EShLangFragment));

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
