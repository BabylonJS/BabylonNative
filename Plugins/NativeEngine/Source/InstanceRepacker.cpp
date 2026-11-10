#include "InstanceRepacker.h"

#include "StorageBuffer.h"

#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>

#include <stdexcept>
#include <vector>

#include "Shaders/dxbc/cs_instance_repack.h"
#include "Shaders/dxil/cs_instance_repack.h"
#include "Shaders/essl/cs_instance_repack.h"
#include "Shaders/glsl/cs_instance_repack.h"
#include "Shaders/metal/cs_instance_repack.h"
#include "Shaders/spirv/cs_instance_repack.h"

namespace Babylon
{
    namespace
    {
        const bgfx::EmbeddedShader RepackShaders[]{
            BGFX_EMBEDDED_SHADER(cs_instance_repack),
            BGFX_EMBEDDED_SHADER_END()};

        constexpr uint32_t INSTANCE_SLOT_SIZE = 16;
        constexpr uint32_t PARAMETER_HEADER_WORD_COUNT = 4;
        constexpr uint32_t ATTRIBUTE_PARAMETER_WORD_COUNT = 4;
        constexpr uint32_t INSTANCE_COUNT_PARAMETER = 0;
        constexpr uint32_t ATTRIBUTE_COUNT_PARAMETER = 1;
        constexpr uint32_t INSTANCE_STRIDE_PARAMETER = 2;
        constexpr uint32_t SOURCE_OFFSET_PARAMETER = 0;
        constexpr uint32_t SOURCE_STRIDE_PARAMETER = 1;
        constexpr uint32_t ELEMENT_SIZE_PARAMETER = 2;
        constexpr uint32_t DESTINATION_OFFSET_PARAMETER = 3;
        constexpr uint8_t PARAMETERS_BINDING = 0;
        constexpr uint8_t SOURCE_BINDING = 1;
        constexpr uint8_t DESTINATION_BINDING = 2;
        constexpr uint32_t COMPUTE_WORKGROUP_SIZE = 64;
    }

    InstanceRepacker::InstanceRepacker(Graphics::DeviceContext& deviceContext)
        : m_deviceContext{deviceContext}
        , m_deviceId{deviceContext.GetDeviceId()}
    {
    }

    InstanceRepacker::~InstanceRepacker()
    {
        for (auto& pair : m_perVertexArray)
        {
            DestroyDest(pair.second);
        }
        m_perVertexArray.clear();
        if (bgfx::isValid(m_program) && m_deviceId == m_deviceContext.GetDeviceId())
        {
            bgfx::destroy(m_program);
        }
    }

    void InstanceRepacker::EnsureProgram()
    {
        if (bgfx::isValid(m_program))
        {
            return;
        }

        const auto shader = bgfx::createEmbeddedShader(RepackShaders, bgfx::getRendererType(), "cs_instance_repack");
        if (!bgfx::isValid(shader))
        {
            throw std::runtime_error{"Failed to create embedded instance repack shader"};
        }
        m_program = bgfx::createProgram(shader, true);
        if (!bgfx::isValid(m_program))
        {
            throw std::runtime_error{"Failed to create embedded instance repack program"};
        }
    }

    void InstanceRepacker::DestroyDest(PerVertexArray& state)
    {
        if (state.DestStorage != nullptr)
        {
            state.DestStorage->Dispose();
            state.DestStorage.reset();
        }
        state.Capacity = 0;
        state.Stride = 0;
    }

    void InstanceRepacker::Forget(VertexArray* vertexArray)
    {
        auto it = m_perVertexArray.find(vertexArray);
        if (it != m_perVertexArray.end())
        {
            DestroyDest(it->second);
            m_perVertexArray.erase(it);
        }
    }

    bgfx::DynamicVertexBufferHandle InstanceRepacker::Repack(Graphics::FrameBuffer& frameBuffer, VertexArray* vertexArray,
        const std::map<uint32_t, VertexBuffer::InstanceInfo>& instances, const VertexBuffer::InstanceDataLayout& layout,
        uint32_t instanceCount)
    {
        if (instances.empty() || instanceCount == 0)
        {
            return BGFX_INVALID_HANDLE;
        }

        const uint32_t attributeCount = static_cast<uint32_t>(instances.size());
        if (attributeCount > MAX_ATTRIBUTES || layout.SlotCount > MAX_ATTRIBUTES)
        {
            return BGFX_INVALID_HANDLE;
        }
        if (layout.Slots.size() != instances.size())
        {
            throw std::runtime_error{"Instance-data layout does not cover every recorded attribute."};
        }

        StorageBuffer* source = instances.begin()->second.ResolveStorage();
        if (source == nullptr)
        {
            return BGFX_INVALID_HANDLE;
        }
        for (const auto& [location, instance] : instances)
        {
            if (instance.ResolveStorage() != source)
            {
                throw std::runtime_error{"GPU instance attributes must share a single storage buffer"};
            }
        }

        EnsureProgram();
        const uint16_t instanceStride = static_cast<uint16_t>(layout.SlotCount * INSTANCE_SLOT_SIZE);
        if (instanceCount > UINT32_MAX / instanceStride)
        {
            throw std::runtime_error{"GPU instance count is too large"};
        }
        const uint32_t destBytes = instanceCount * instanceStride;

        PerVertexArray& state = m_perVertexArray[vertexArray];

        if (state.DestStorage == nullptr || state.Capacity < instanceCount || state.Stride != instanceStride
            || state.DestStorage->ByteLength() < destBytes)
        {
            DestroyDest(state);
            state.DestStorage = std::make_shared<StorageBuffer>(
                m_deviceContext, destBytes, /*byteStride*/ instanceStride, /*computeWrite*/ true);
            state.Capacity = instanceCount;
            state.Stride = instanceStride;
        }

        const uint32_t paramsU = PARAMETER_HEADER_WORD_COUNT + attributeCount * ATTRIBUTE_PARAMETER_WORD_COUNT;
        std::vector<uint32_t> params(paramsU, 0);
        params[INSTANCE_COUNT_PARAMETER] = instanceCount;
        params[ATTRIBUTE_COUNT_PARAMETER] = attributeCount;
        params[INSTANCE_STRIDE_PARAMETER] = instanceStride / sizeof(float);

        uint32_t attr = 0;
        for (const auto& [location, element] : instances)
        {
            const uint32_t slot = layout.Slots.at(location);
            const uint32_t base = PARAMETER_HEADER_WORD_COUNT + attr++ * ATTRIBUTE_PARAMETER_WORD_COUNT;
            params[base + SOURCE_OFFSET_PARAMETER] = element.Offset / sizeof(float);
            params[base + SOURCE_STRIDE_PARAMETER] = element.Stride / sizeof(float);
            params[base + ELEMENT_SIZE_PARAMETER] = element.ElementSize / sizeof(float);
            params[base + DESTINATION_OFFSET_PARAMETER] = (slot * INSTANCE_SLOT_SIZE) / sizeof(float);
        }

        bgfx::Encoder* computeEncoder = m_deviceContext.GetActiveEncoder();
        if (computeEncoder == nullptr)
        {
            return BGFX_INVALID_HANDLE;
        }

        // bgfx applies all CPU updates of a frame before rendering it, so each dispatch needs its
        // own params. Destruction is deferred until the frame that uses them has rendered.
        const uint32_t paramsBytes = paramsU * sizeof(uint32_t);
        StorageBuffer paramsBuffer{m_deviceContext, paramsBytes, /*byteStride*/ INSTANCE_SLOT_SIZE, /*computeWrite*/ false};
        paramsBuffer.Update(gsl::make_span(reinterpret_cast<const uint8_t*>(params.data()), paramsBytes), 0);
        paramsBuffer.SetCompute(computeEncoder, PARAMETERS_BINDING, bgfx::Access::Read);
        source->SetCompute(computeEncoder, SOURCE_BINDING, bgfx::Access::Read);
        state.DestStorage->SetCompute(computeEncoder, DESTINATION_BINDING, bgfx::Access::ReadWrite);

        const uint32_t numGroups = (instanceCount + COMPUTE_WORKGROUP_SIZE - 1) / COMPUTE_WORKGROUP_SIZE;
        frameBuffer.Compute(*computeEncoder, m_program, numGroups, 1, 1);

        return state.DestStorage->Handle();
    }
}