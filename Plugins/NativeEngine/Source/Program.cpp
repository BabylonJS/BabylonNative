#include "Program.h"

#include <arcana/tracing/trace_region.h>

#include <cassert>

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
            uniformInfos.emplace(std::make_pair(handle.idx, Babylon::UniformInfo{itStage == uniformStages.end() ? uint8_t{} : itStage->second, handle, info.type, info.num}));
            uniformNameToIndex[info.name] = handleIndex;
        }
    }

    bgfx::ShaderHandle CreateShader(const std::shared_ptr<Babylon::Graphics::BgfxShaderInfo>& shaderInfo, gsl::span<const uint8_t> bytes)
    {
        using ShaderInfoPtr = std::shared_ptr<Babylon::Graphics::BgfxShaderInfo>;
        static auto ShaderInfoReleaseFn = [](void*, void* userData) {
            delete reinterpret_cast<ShaderInfoPtr*>(userData);
        };
        return bgfx::createShader(bgfx::makeRef(
            bytes.data(), static_cast<uint32_t>(bytes.size()),
            ShaderInfoReleaseFn, new ShaderInfoPtr{shaderInfo}));
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

        auto vertexShader = CreateShader(shaderInfo, shaderInfo->VertexBytes);
        InitUniformInfos(vertexShader, shaderInfo->UniformStages, m_uniformInfos, m_uniformNameToIndex);

        auto fragmentShader = CreateShader(shaderInfo, shaderInfo->FragmentBytes);
        InitUniformInfos(fragmentShader, shaderInfo->UniformStages, m_uniformInfos, m_uniformNameToIndex);

        m_handle = bgfx::createProgram(vertexShader, fragmentShader, true);
        m_vertexAttributeLocations = shaderInfo->VertexAttributeLocations;
    }

    void Program::SetSources(std::string vertexSource, std::string fragmentSource)
    {
        m_vertexSource = std::move(vertexSource);
        m_fragmentSource = std::move(fragmentSource);
    }

    bgfx::ProgramHandle Program::GetOrCreateInstancedVariant(const std::map<std::string, uint32_t>& instancedAttributes, ShaderProvider& shaderProvider)
    {
        if (instancedAttributes.empty())
        {
            return m_handle;
        }

        const auto it = m_instancedVariants.find(instancedAttributes);
        if (it != m_instancedVariants.end())
        {
            return it->second;
        }

        auto shaderInfo = shaderProvider.Get(m_vertexSource, m_fragmentSource, instancedAttributes);

        auto vertexShader = CreateShader(shaderInfo, shaderInfo->VertexBytes);
        auto fragmentShader = CreateShader(shaderInfo, shaderInfo->FragmentBytes);
        bgfx::ProgramHandle handle = bgfx::createProgram(vertexShader, fragmentShader, true);

        m_instancedVariants.emplace(instancedAttributes, handle);
        return handle;
    }

    void Program::Dispose()
    {
        const bool sameDevice = m_deviceID == m_deviceContext.GetDeviceId();

        if (bgfx::isValid(m_handle))
        {
            if (sameDevice)
            {
                bgfx::destroy(m_handle);
            }

            m_handle = BGFX_INVALID_HANDLE;
        }

        for (auto& [key, handle] : m_instancedVariants)
        {
            if (sameDevice && bgfx::isValid(handle))
            {
                bgfx::destroy(handle);
            }
        }
        m_instancedVariants.clear();

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

        // bgfx reads a type-dependent number of floats per array element from uniform data.
        // Must match bgfx g_uniformTypeSize (bgfx.cpp): Vec4=4, Mat3=9, Mat4=16.
        assert((itUniformInfo == m_uniformInfos.end() || [&]() {
            static_assert(bgfx::UniformType::Vec4 == 2 && bgfx::UniformType::Mat3 == 3 && bgfx::UniformType::Mat4 == 4);
            const size_t FloatsPerElement[] = {4, 9, 16};
            bgfx::UniformType::Enum type = itUniformInfo->second.Type;
            return (type >= bgfx::UniformType::Vec4 && type <= bgfx::UniformType::Mat4)
                       ? value.Data.size() == FloatsPerElement[type - bgfx::UniformType::Vec4] * elementLength
                       : false;
        }()));

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
