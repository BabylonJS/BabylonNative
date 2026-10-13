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

        constexpr uint32_t kSlotSize = 16;
        constexpr uint32_t kParamsHeaderU = 4;
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
        if (attributeCount > kMaxAttributes || layout.SlotCount > kMaxAttributes)
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
        const uint16_t instanceStride = static_cast<uint16_t>(layout.SlotCount * kSlotSize);
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

        // Build params blob: header + one uvec4 per attribute, written into the shader's layout slot.
        const uint32_t paramsU = kParamsHeaderU + attributeCount * 4;
        std::vector<uint32_t> params(paramsU, 0);
        params[0] = instanceCount;
        params[1] = attributeCount;
        params[2] = instanceStride / sizeof(float);
        params[3] = 0;

        uint32_t attr = 0;
        for (const auto& [location, element] : instances)
        {
            const uint32_t slot = layout.Slots.at(location);
            const uint32_t base = kParamsHeaderU + attr++ * 4;
            params[base + 0] = element.Offset / sizeof(float);
            params[base + 1] = element.Stride / sizeof(float);
            params[base + 2] = element.ElementSize / sizeof(float);
            params[base + 3] = (slot * kSlotSize) / sizeof(float);
        }

        bgfx::Encoder* computeEncoder = m_deviceContext.GetActiveEncoder();
        if (computeEncoder == nullptr)
        {
            return BGFX_INVALID_HANDLE;
        }

        // bgfx applies all CPU updates of a frame before rendering it, so each dispatch needs its
        // own params. Destruction is deferred until the frame that uses them has rendered.
        const uint32_t paramsBytes = paramsU * sizeof(uint32_t);
        StorageBuffer paramsBuffer{m_deviceContext, paramsBytes, /*byteStride*/ 16, /*computeWrite*/ false};
        paramsBuffer.Update(gsl::make_span(reinterpret_cast<const uint8_t*>(params.data()), paramsBytes), 0);
        paramsBuffer.SetCompute(computeEncoder, 0, bgfx::Access::Read);
        source->SetCompute(computeEncoder, 1, bgfx::Access::Read);
        state.DestStorage->SetCompute(computeEncoder, 2, bgfx::Access::ReadWrite);

        const uint32_t numGroups = (instanceCount + 63u) / 64u;
        frameBuffer.Compute(*computeEncoder, m_program, numGroups, 1, 1);

        return state.DestStorage->Handle();
    }
}