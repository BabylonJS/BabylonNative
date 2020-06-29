#pragma once

#include <glslang/Public/ShaderLang.h>

#include <memory>

namespace Babylon::ShaderCompilerTraversers
{
    class AllocationsScopeBase
    {
    public:
        AllocationsScopeBase() = default;
        AllocationsScopeBase(const AllocationsScopeBase&) = delete;

        virtual ~AllocationsScopeBase()
        {
        }
    };
    using ScopeT = std::unique_ptr<AllocationsScopeBase>;

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

    ScopeT MoveNonSamplerUniformsIntoStruct(glslang::TProgram& program, IdGenerator& ids);
    ScopeT ChangeUniformTypes(glslang::TProgram& program, IdGenerator& ids);
    void AssignLocationsAndNamesToVertexVaryings(glslang::TProgram& program, IdGenerator& ids);
    void SplitSamplersIntoSamplersAndTextures(glslang::TProgram& program, IdGenerator& ids);
}
