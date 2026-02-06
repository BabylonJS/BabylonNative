#include "Program.h"

#include <arcana/tracing/trace_region.h>

namespace
{
    void InitUniformInfos(
        bgfx::ShaderHandle shader,
        const std::map<std::string, uint8_t>& uniformStages,
        std::map<uint16_t, Babylon::UniformInfo>& uniformInfos,
        std::map<std::string, uint16_t>& uniformNameToIndex)
    {
        uint16_t numUniforms = bgfx::getShaderUniforms(shader);
        std::vector<bgfx::UniformHandle> uniforms{numUniforms};
        bgfx::getShaderUniforms(shader, uniforms.data(), gsl::narrow_cast<uint16_t>(uniforms.size()));

        for (uint16_t index = 0; index < numUniforms; index++)
        {
            bgfx::UniformInfo info{};
            uint16_t handleIndex = uniforms[index].idx;
            bgfx::getUniformInfo(uniforms[index], info);
            auto itStage = uniformStages.find(info.name);
            auto& handle = uniforms[index];
            uniformInfos.emplace(std::make_pair(handle.idx, Babylon::UniformInfo{itStage == uniformStages.end() ? uint8_t{} : itStage->second, handle, info.num}));
            uniformNameToIndex[info.name] = handleIndex;
        }
    }
}

namespace Babylon
{
    Program::Program(Graphics::DeviceContext& deviceContext)
        : m_deviceContext{deviceContext}
        , m_deviceID{deviceContext.GetDeviceId()}
        , m_handle{bgfx::kInvalidHandle}
    {
    }

    Program::~Program()
    {
        Dispose();
    }

    void Program::Initialize(std::shared_ptr<Graphics::BgfxShaderInfo> shaderInfo)
    {
        arcana::trace_region region{"Program::Initialize"};

        using ShaderInfoPtr = std::shared_ptr<Graphics::BgfxShaderInfo>;

        static auto ShaderInfoReleaseFn = [](void*, void* userData) {
            delete reinterpret_cast<ShaderInfoPtr*>(userData);
        };

        auto vertexShader = bgfx::createShader(bgfx::makeRef(
            shaderInfo->VertexBytes.data(), static_cast<uint32_t>(shaderInfo->VertexBytes.size()),
            ShaderInfoReleaseFn, new ShaderInfoPtr{shaderInfo}));
        InitUniformInfos(vertexShader, shaderInfo->UniformStages, m_uniformInfos, m_uniformNameToIndex);

        auto fragmentShader = bgfx::createShader(bgfx::makeRef(
            shaderInfo->FragmentBytes.data(), static_cast<uint32_t>(shaderInfo->FragmentBytes.size()),
            ShaderInfoReleaseFn, new ShaderInfoPtr{shaderInfo}));
        InitUniformInfos(fragmentShader, shaderInfo->UniformStages, m_uniformInfos, m_uniformNameToIndex);

        m_handle = bgfx::createProgram(vertexShader, fragmentShader, true);
        m_vertexAttributeLocations = shaderInfo->VertexAttributeLocations;
    }

    void Program::Dispose()
    {
        if (bgfx::isValid(m_handle))
        {
            if (m_deviceID == m_deviceContext.GetDeviceId())
            {
                bgfx::destroy(m_handle);
            }

            m_handle = BGFX_INVALID_HANDLE;
        }

        m_uniforms.clear();
        m_uniformNameToIndex.clear();
        m_uniformInfos.clear();
        m_vertexAttributeLocations.clear();
    }

    void Program::SetUniform(bgfx::UniformHandle handle, gsl::span<const float> data, size_t elementLength)
    {
        UniformValue& value = m_uniforms[handle.idx];

        const auto itUniformInfo{m_uniformInfos.find(handle.idx)};

        if (itUniformInfo != m_uniformInfos.end())
        {
            elementLength = std::min(itUniformInfo->second.MaxElementLength, elementLength);
        }

        value.Data.assign(data.begin(), data.end());
        value.ElementLength = static_cast<uint16_t>(elementLength);
    }

    const UniformInfo* Program::GetUniformInfo(const std::string& name) const
    {
        const auto itUniformIndex = m_uniformNameToIndex.find(name);
        if (itUniformIndex == m_uniformNameToIndex.end())
        {
            return nullptr;
        }

        const auto itUniformInfo = m_uniformInfos.find(itUniformIndex->second);
        if (itUniformInfo == m_uniformInfos.end())
        {
            return nullptr;
        }

        return &itUniformInfo->second;
    }
}
