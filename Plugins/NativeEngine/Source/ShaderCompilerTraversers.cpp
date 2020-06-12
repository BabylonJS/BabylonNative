#include "ShaderCompilerTraversers.h"

#include <glslang/Include/intermediate.h>
#include <glslang/MachineIndependent/localintermediate.h>
#include <glslang/MachineIndependent/RemoveTree.h>

#include <bgfx/bgfx.h>

#include <arcana/experimental/array.h>

#include <gsl/gsl>

#include <stdexcept>

namespace Babylon::ShaderCompilerTraversers
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
                    for (size_t idx = 0; idx < sequence.size(); ++idx)
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
            class AllocationsScope : AllocationsScopeBase
            {
            public:
                ~AllocationsScope() override
                {
                }
            
            private:
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

            static ScopeT Traverse(TProgram& program, IdGenerator& ids)
            {
                auto* scope = new AllocationsScope();
                Traverse(program.getIntermediate(EShLangVertex), ids, *scope);
                Traverse(program.getIntermediate(EShLangFragment), ids, *scope);
                return std::unique_ptr<AllocationsScopeBase>(scope);
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
                for (int idx = gsl::narrow_cast<int>(sequence.size()) - 1; idx >= 0; --idx)
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

        class UniformTypeChangeTraverser final : private TIntermTraverser
        {
        public:
            class AllocationsScope : AllocationsScopeBase
            {
            public:
                ~AllocationsScope() override
                {
                }

            private:
                friend UniformTypeChangeTraverser;
                std::vector<std::unique_ptr<TArraySizes>> ArraySizes{};
            };

            ~UniformTypeChangeTraverser() override
            {
            }

            void visitSymbol(TIntermSymbol* symbol) override
            {
                auto& type = symbol->getType();
                if (type.getQualifier().isUniformOrBuffer() && type.getBasicType() != EbtSampler && !type.isMatrix())
                {
                    // So what we've got here is actually fine...for the symbol. If the symbol's on its own (i.e., in 
                    // a linker object, this is enough. However, if the symbol has a parent that has a type that isn't
                    // the same as its NEW type, we're going to have to create a new binary and swizzle to down-project
                    // the vector into the smaller type that's expected by the hierarchy. Note that if done naively this
                    // may end up creating layered swizzles; i.e., if a vec3 was already being projected down a la 
                    // vec3.x, greedily adding a swizzle operator to deal with the new type mismatch may create 
                    // something like (vec3.xyz).x. Down the line, this shouldn't be extraordinarily difficult to check
                    // and preempt. Right now, though, I couldn't possibly care less.

                    auto* oldType = type.clone();
                    
                    TPublicType publicType{};
                    publicType.qualifier = type.getQualifier();

                    publicType.basicType = EbtFloat;
                    publicType.setVector(4);

                    if (type.getArraySizes())
                    {
                        m_scope.ArraySizes.emplace_back(std::make_unique<TArraySizes>());
                        publicType.arraySizes = m_scope.ArraySizes.back().get();
                        *publicType.arraySizes = *type.getArraySizes();
                    }
                    else
                    {
                        publicType.arraySizes = nullptr;
                    }

                    TType newType{publicType};
                    symbol->setType(newType);

                    if (!isLinkerObject(this->path))
                    {
                        auto shapeConversion = m_intermediate->addShapeConversion(*oldType, symbol);
                        auto* parent = this->getParentNode();
                        if (auto* aggregate = parent->getAsAggregate())
                        {
                            auto& sequence = aggregate->getSequence();
                            for (size_t idx = 0; idx < sequence.size(); ++idx)
                            {
                                if (sequence[idx] == symbol)
                                {
                                    sequence[idx] = shapeConversion;
                                }
                            }
                        }
                        else if (auto* binary = parent->getAsBinaryNode())
                        {
                            if (binary->getLeft() == symbol)
                            {
                                binary->setLeft(shapeConversion);
                            }
                            else
                            {
                                binary->setRight(shapeConversion);
                            }
                        }
                        else if (auto* unary = parent->getAsUnaryNode())
                        {
                            unary->setOperand(shapeConversion);
                        }
                        else
                        {
                            throw std::runtime_error{"Cannot replace symbol: node type handler unimplemented"};
                        }
                    }

                    delete oldType;
                }
            }

            static ScopeT Traverse(TProgram& program, IdGenerator& ids)
            {
                auto* scope = new AllocationsScope();
                Traverse(program.getIntermediate(EShLangVertex), ids, *scope);
                Traverse(program.getIntermediate(EShLangFragment), ids, *scope);
                return std::unique_ptr<AllocationsScopeBase>(scope);
            }

        private:
            UniformTypeChangeTraverser(TIntermediate* intermediate, AllocationsScope& scope)
                : TIntermTraverser{}
                , m_intermediate{intermediate}
                , m_scope{scope}
            {
            }

            static void Traverse(glslang::TIntermediate* intermediate, IdGenerator&, AllocationsScope& scope)
            {
                UniformTypeChangeTraverser traverser{intermediate, scope};
                intermediate->getTreeRoot()->traverse(&traverser);
            }

            TIntermediate* m_intermediate{};
            AllocationsScope& m_scope;
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
            std::pair<unsigned int, const char*> GetVaryingLocationAndNewNameForName(const char* name)
            {
#define IF_NAME_RETURN_ATTRIB(varyingName, attrib, newName)  \
    if (std::strcmp(name, varyingName) == 0)                 \
    {                                                        \
        return {static_cast<unsigned int>(attrib), newName}; \
    }
                IF_NAME_RETURN_ATTRIB("position", bgfx::Attrib::Position, "a_position")
                IF_NAME_RETURN_ATTRIB("normal", bgfx::Attrib::Normal, "a_normal")
                IF_NAME_RETURN_ATTRIB("tangent", bgfx::Attrib::Tangent, "a_tangent")
                IF_NAME_RETURN_ATTRIB("uv", bgfx::Attrib::TexCoord0, "a_texcoord0")
                IF_NAME_RETURN_ATTRIB("uv2", bgfx::Attrib::TexCoord1, "a_texcoord1")
                IF_NAME_RETURN_ATTRIB("uv3", bgfx::Attrib::TexCoord2, "a_texcoord2")
                IF_NAME_RETURN_ATTRIB("uv4", bgfx::Attrib::TexCoord3, "a_texcoord3")
                IF_NAME_RETURN_ATTRIB("color", bgfx::Attrib::Color0, "a_color0")
                IF_NAME_RETURN_ATTRIB("matricesIndices", bgfx::Attrib::Indices, "a_indices")
                IF_NAME_RETURN_ATTRIB("matricesWeights", bgfx::Attrib::Weight, "a_weight")
#undef IF_NAME_RETURN_ATTRIB
                return {FIRST_GENERIC_ATTRIBUTE_LOCATION + m_genericAttributesRunningCount++, name};
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
                    auto [location, newName] = traverser.GetVaryingLocationAndNewNameForName(name.c_str());
                    publicType.qualifier.layoutLocation = location;

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
                    auto* newSymbol = intermediate->addSymbol(TIntermSymbol{ids.Next(), newName, newType});
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
                for (int idx = gsl::narrow_cast<int>(sequence.size()) - 1; idx >= 0; --idx)
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

    ScopeT MoveNonSamplerUniformsIntoStruct(TProgram& program, IdGenerator& ids)
    {
        return UniformToStructTraverser::Traverse(program, ids);
    }

    ScopeT ChangeUniformTypes(TProgram& program, IdGenerator& ids)
    {
        return UniformTypeChangeTraverser::Traverse(program, ids);
    }

    void AssignLocationsAndNamesToVertexVaryings(TProgram& program, IdGenerator& ids)
    {
        VertexVaryingInTraverser::Traverse(program, ids);
    }

    void SplitSamplersIntoSamplersAndTextures(TProgram& program, IdGenerator& ids)
    {
        SamplerSplitterTraverser::Traverse(program, ids);
    }
}
