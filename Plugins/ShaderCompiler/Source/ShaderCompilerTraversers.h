#pragma once

#include <glslang/Public/ShaderLang.h>

#include <memory>
#include <map>

namespace Babylon::ShaderCompilerTraversers
{
    /// Helper class to manage the lifecycle of objects allocated by shader program
    /// modification. Some allocations must persist long enough for the shader
    /// program to be recompiled, which will always happen after the scope of the
    /// modification which performed the allocation has exited. This helper class
    /// provides an opaque mechanism for placing those allocations in a transient
    /// "scope" which can persist as long as it needs to.
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

    /// Helper class to allow consistency amongst various calls to modify shaders.
    /// In the course of modifying programs, various objects must be given IDs,
    /// and those IDs must not be allowed to conflict with one another, so this
    /// class provides a way for many modification techniques to generate new
    /// IDs that will not collide.
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
    ScopeT MoveNonSamplerUniformsIntoStruct(glslang::TProgram& program, IdGenerator& ids);

    /// Performs all changes to uniform types required by platforms that need changes.
    /// This is needed for Metal and OpenGL and Direct3D to match bgfx's expectations
    /// that all uniforms, even scalars, are implemented as vec4 uniforms.
    /// It's not mandatory for D3D11 but it is for D3D12.
    ScopeT ChangeUniformTypes(glslang::TProgram& program, IdGenerator& ids);

    /// Changes the names and locations of varying attributes in the vertex shader to
    /// match bgfx's expectations.
    void AssignLocationsAndNamesToVertexVaryingsOpenGL(glslang::TProgram& program, IdGenerator& ids, std::map<std::string, std::string>& vertexAttributeRenaming);
    void AssignLocationsAndNamesToVertexVaryingsMetal(glslang::TProgram& program, IdGenerator& ids, std::map<std::string, std::string>& vertexAttributeRenaming);
    void AssignLocationsAndNamesToVertexVaryingsD3D(glslang::TProgram& program, IdGenerator& ids, std::map<std::string, std::string>& vertexAttributeRenaming);

    /// WebGL (and therefore Babylon.js) treats texture samplers as a single variable.
    /// Native platforms expect them to be two separate variables -- a texture and a
    /// sampler -- used together, so this function splits all texture samplers to match
    /// the expectations of native platforms.
    void SplitSamplersIntoSamplersAndTextures(glslang::TProgram& program, IdGenerator& ids);

    /// Split combined `sampler2D`/`samplerCube`/etc. function parameters of user-defined
    /// functions into separate texture and sampler parameters. Must be called AFTER
    /// SplitSamplersIntoSamplersAndTextures.
    ///
    /// The GLSL emitted by Babylon.js contains user functions that take combined
    /// samplers as parameters and pass uniform samplers to them at the call site, e.g.
    ///
    ///     vec3 fetch(sampler2D s, vec2 uv) { return texture(s, uv).rgb; }
    ///     void main() { ... fetch(myUniformSampler, uv); }
    ///
    /// glslang's SPIR-V emitter requires opaque (sampler) function arguments to be
    /// l-values (it calls `accessChainGetLValue()` on them). After
    /// SplitSamplersIntoSamplersAndTextures has rewritten the call-site sampler argument
    /// into an `EOpConstructTextureSampler(t, s)` aggregate (an r-value), glslang
    /// asserts at `SpvBuilder.cpp:accessChainGetLValue()`. See TPC #1584 for details.
    ///
    /// This pass fixes the assert by:
    ///   1. Rewriting each user function with sampler parameters: each `sampler2D s`
    ///      parameter becomes two parameters `texture2D sTexture, sampler s`, and every
    ///      reference to the original `s` in the body becomes an
    ///      `EOpConstructTextureSampler(sTexture, s)` aggregate.
    ///   2. Rewriting every call site to such functions: the single
    ///      `EOpConstructTextureSampler(t, s)` argument is replaced with two separate
    ///      arguments (t and s).
    void SplitSamplerFunctionParameters(glslang::TProgram& program, IdGenerator& ids);

    /// Prepends a zero-initialization assignment at the start of every function body
    /// for each struct-typed local variable referenced inside the body. Works around
    /// Babylon.js shaders that read from uninitialized struct fields (e.g.
    /// `lightingInfo result; result.diffuse += ...;` inside `computeAreaLighting`),
    /// which compile successfully under WebGL but trip D3DCompile's `error X4000:
    /// variable used without having been completely initialized` once SPIRV-Cross
    /// emits the corresponding HLSL.
    ///
    /// Locals of array, scalar, vector and matrix type are left unchanged; only
    /// struct-typed locals (the observed X4000 trigger) are initialized.
    void ZeroInitializeStructLocals(glslang::TProgram& program);

    /// Invert dFdy operands similar to bgfx_shader.sh
    /// https://github.com/bkaradzic/bgfx/blob/7be225bf490bb1cd231cfb4abf7e617bf35b59cb/src/bgfx_shader.sh#L44-L45
    /// https://github.com/bkaradzic/bgfx/blob/7be225bf490bb1cd231cfb4abf7e617bf35b59cb/src/bgfx_shader.sh#L62-L65
    void InvertYDerivativeOperands(glslang::TProgram& program);
}
