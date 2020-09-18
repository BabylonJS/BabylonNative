#pragma once

#include <glslang/Public/ShaderLang.h>

#include <memory>

namespace Babylon::ShaderCompilerTraversers
{
    /// <summary>
    /// Helper class to manage the lifecycle of objects allocated by shader program
    /// modification. Some allocations must persist long enough for the shader
    /// program to be recompiled, which will always happen after the scope of the
    /// modification which performed the allocation has exited. This helper class
    /// provides an opaque mechanism for placing those allocations in a transient
    /// "scope" which can persist as long as it needs to.
    /// </summary>
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

    /// <summary>
    /// Helper class to allow consistency amongst various calls to modify shaders.
    /// In the course of modifying programs, various objects must be given IDs, 
    /// and those IDs must not be allowed to conflict with one another, so this
    /// class provides a way for many modification techniques to generate new
    /// IDs that will not collide.
    /// </summary>
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

    /// <summary>
    /// Modify the shader program by moving all uniforms other than samplers into a struct.
    /// Thus, if the input shader has uniforms 
    /// 
    ///     vec3 position;
    ///     mat4 viewMatrix;
    /// 
    /// then the shader will be modified to have the following struct instead.
    /// 
    ///     struct Frame
    ///     {
    ///         vec3 position;
    ///         mat4 viewMatrix;
    ///     }
    /// </summary>
    /// <param name="program">The shader program to be modified.</param>
    /// <param name="ids">The IdGenerator from which to generate IDs that don't collide with other modifications.</param>
    /// <returns></returns>
    ScopeT MoveNonSamplerUniformsIntoStruct(glslang::TProgram& program, IdGenerator& ids);

    /// <summary>
    /// Performs all changes to uniform types required by platforms that need changes.
    /// This is needed for Metal and OpenGL (not DirectX) to match bgfx's expectations 
    /// that all uniforms, even scalars, are implemented as vec4 uniforms.
    /// </summary>
    /// <param name="program">The shader program to be modified.</param>
    /// <param name="ids">The IdGenerator from which to generate IDs that don't collide with other modifications.</param>
    /// <returns></returns>
    ScopeT ChangeUniformTypes(glslang::TProgram& program, IdGenerator& ids);

    /// <summary>
    /// Changes the names and locations of varying attributes in the vertex shader to
    /// match bgfx's expectations. 
    /// </summary>
    /// <param name="program">The shader program to be modified.</param>
    /// <param name="ids">The IdGenerator from which to generate IDs that don't collide with other modifications.</param>
    /// <returns></returns>
    void AssignLocationsAndNamesToVertexVaryings(glslang::TProgram& program, IdGenerator& ids);

    /// <summary>
    /// WebGL (and therefore Babylon.js) treats texture samplers as a single variable. 
    /// Native platforms expect them to be two separate variables -- a texture and a 
    /// sampler -- used together, so this function splits all texture samplers to match
    /// the expectations of native platforms.
    /// </summary>
    /// <param name="program">The shader program to be modified.</param>
    /// <param name="ids">The IdGenerator from which to generate IDs that don't collide with other modifications.</param>
    /// <returns></returns>
    void SplitSamplersIntoSamplersAndTextures(glslang::TProgram& program, IdGenerator& ids);
}
