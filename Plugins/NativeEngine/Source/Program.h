#pragma once

#include "ShaderProvider.h"

#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/BgfxShaderInfo.h>

#include <bgfx/bgfx.h>
#include <gsl/gsl>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Babylon
{
    struct UniformInfo final
    {
        UniformInfo(uint8_t stage, bgfx::UniformHandle handle, bgfx::UniformType::Enum type, size_t maxElementLength)
            : Stage{stage}
            , Handle{handle}
            , Type{type}
            , MaxElementLength{maxElementLength}
        {
        }

        uint8_t Stage{};
        bgfx::UniformHandle Handle{bgfx::kInvalidHandle};
        bgfx::UniformType::Enum Type{bgfx::UniformType::Count};
        size_t MaxElementLength{};
    };

    struct UniformValue
    {
        std::vector<float> Data{};
        uint16_t ElementLength{};
    };

    class Program final
    {
    public:
        Program(Graphics::DeviceContext& deviceContext);

        // Copy semantics
        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;

        // Move semantics
        Program(Program&&) = delete;
        Program& operator=(Program&&) = delete;

        ~Program();

        void Initialize(std::shared_ptr<Graphics::BgfxShaderInfo> shaderInfo);

        // Creates a bgfx compute program (single CSH shader) from shaderInfo->ComputeBytes.
        void InitializeCompute(std::shared_ptr<Graphics::BgfxShaderInfo> shaderInfo);

        // Creates the compute program that emulates a transform feedback vertex shader (see
        // ShaderCompiler::CompileTransformFeedback). Vertex inputs keep their reported locations.
        void InitializeTransformFeedback(std::shared_ptr<Graphics::BgfxShaderInfo> shaderInfo);
        bool IsTransformFeedback() const { return m_transformFeedbackParams != nullptr; }
        const UniformInfo* TransformFeedbackParams() const { return m_transformFeedbackParams; }
        // Null when the shader has no vertex inputs (glslang strips the unused array).
        const UniformInfo* TransformFeedbackInputLayout() const { return m_transformFeedbackInputLayout; }
        uint32_t TransformFeedbackInputCount() const { return m_transformFeedbackInputCount; }

        void Dispose();

        // Stores the original GLSL sources so divisor-driven instanced variants can be
        // recompiled lazily on the first instanced draw (see GetOrCreateInstancedVariant).
        void SetSources(std::string vertexSource, std::string fragmentSource);

        // Returns a program handle whose vertex shader routes the given consumer-instanced
        // attributes (name -> bgfx per-instance location) to bgfx i_data slots. Compiled and
        // cached on first use. An empty map returns the base handle.
        bgfx::ProgramHandle GetOrCreateInstancedVariant(const std::map<std::string, uint32_t>& instancedAttributes, ShaderProvider& shaderProvider);

        void SetUniform(bgfx::UniformHandle handle, gsl::span<const float> data, size_t elementLength = 1);
        const UniformInfo* GetUniformInfo(const std::string& name) const;
        bgfx::ProgramHandle Handle() const { return m_handle; }
        const std::map<uint16_t, UniformValue>& Uniforms() const { return m_uniforms; }
        const std::map<std::string, uint32_t>& VertexAttributeLocations() const { return m_vertexAttributeLocations; }
        // The render target orientation uniform the shader compiler injects on top-left-origin
        // backends (see RENDER_TARGET_TRANSFORM_UNIFORM_NAME). Null on OpenGL and for compute.
        // Resolved once at initialization because it is consulted on every draw.
        const UniformInfo* RenderTargetTransformUniform() const { return m_renderTargetTransformUniform; }
        const std::map<uint8_t, bgfx::UniformHandle>& SamplerStateUniforms() const { return m_samplerStateUniforms; }
        const std::map<uint8_t, bgfx::UniformHandle>& SamplerUniforms() const { return m_samplerUniforms; }

        // Compiler-assigned i_data slot for each built-in attribute location.
        const std::map<uint32_t, uint32_t>& BuiltInInstanceDataSlots() const { return m_builtInInstanceDataSlots; }

    private:
        Graphics::DeviceContext& m_deviceContext;
        uintptr_t m_deviceID;
        bgfx::ProgramHandle m_handle;
        std::map<uint16_t, UniformValue> m_uniforms;
        std::map<std::string, uint16_t> m_uniformNameToIndex;
        std::map<uint16_t, UniformInfo> m_uniformInfos;
        std::map<std::string, uint32_t> m_vertexAttributeLocations;
        const UniformInfo* m_renderTargetTransformUniform{nullptr};
        std::map<uint8_t, bgfx::UniformHandle> m_samplerStateUniforms;
        std::map<uint8_t, bgfx::UniformHandle> m_samplerUniforms;
        std::map<uint32_t, uint32_t> m_builtInInstanceDataSlots;
        std::string m_vertexSource;
        std::string m_fragmentSource;
        std::map<std::map<std::string, uint32_t>, bgfx::ProgramHandle> m_instancedVariants;
        const UniformInfo* m_transformFeedbackParams{nullptr};
        const UniformInfo* m_transformFeedbackInputLayout{nullptr};
        uint32_t m_transformFeedbackInputCount{};
    };
}
