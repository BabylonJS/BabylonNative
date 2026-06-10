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

    private:
        Graphics::DeviceContext& m_deviceContext;
        uintptr_t m_deviceID;
        bgfx::ProgramHandle m_handle;
        std::map<uint16_t, UniformValue> m_uniforms;
        std::map<std::string, uint16_t> m_uniformNameToIndex;
        std::map<uint16_t, UniformInfo> m_uniformInfos;
        std::map<std::string, uint32_t> m_vertexAttributeLocations;
        std::string m_vertexSource;
        std::string m_fragmentSource;
        std::map<std::map<std::string, uint32_t>, bgfx::ProgramHandle> m_instancedVariants;
    };
}
