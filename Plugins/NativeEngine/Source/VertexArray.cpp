#include "VertexArray.h"
#include <cassert>
#include <string>
#include "Babylon/Graphics/BgfxShaderInfo.h"
#include "Babylon/Graphics/DeviceContext.h"

namespace Babylon
{
    VertexArray::VertexArray(Graphics::DeviceContext& deviceContext)
        : m_deviceContext{deviceContext}
        , m_deviceId{m_deviceContext.GetDeviceId()}
    {
    }

    VertexArray::~VertexArray()
    {
        Dispose();
    }

    void VertexArray::Dispose()
    {
        if (m_disposed)
        {
            return;
        }

        m_indexBuffer = nullptr;
        if (m_deviceId == m_deviceContext.GetDeviceId())
        {
            for (const auto& [attrib, record] : m_vertexBufferRecords)
            {
                static_cast<void>(attrib);
                if (bgfx::isValid(record.LayoutHandle))
                {
                    bgfx::destroy(record.LayoutHandle);
                }
            }
        }
        m_vertexBufferRecords.clear();
        m_vertexBufferInstances.clear();

        m_disposed = true;
    }

    void VertexArray::RecordIndexBuffer(IndexBuffer* indexBuffer)
    {
        m_indexBuffer = indexBuffer;
        m_indexBuffer->Build();
    }

    void VertexArray::RecordVertexBuffer(VertexBuffer* vertexBuffer, uint32_t location, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized, uint32_t divisor)
    {
        if (m_disposed)
        {
            throw std::runtime_error{"Cannot record a vertex buffer in a disposed vertex array"};
        }

        auto attribType = static_cast<bgfx::AttribType::Enum>(type);

        if (divisor == 1)
        {
            if (attribType != bgfx::AttribType::Float || normalized)
            {
                throw std::runtime_error{"Unsupported vertex buffer attribute type or normalized flag"};
            }

            const bgfx::Caps* caps = bgfx::getCaps();

            // Use the runtime cap, not MAX_INSTANCE_DATA_SLOT_COUNT: backends clamp maxInstanceData
            // to the device's maxVertexAttributes during init, so the compile-time value is a
            // ceiling a device need not honour. The constant stays for the shader compiler's
            // static_asserts, which need a compile-time bound.
            //
            // Only a new attribute can overflow -- re-recording an existing one overwrites its
            // entry -- and the check runs before the insert, so `size() >= max` is the overflow.
            const uint32_t maxInstanceData = caps->limits.maxInstanceData;
            if (m_vertexBufferInstances.find(location) == m_vertexBufferInstances.end() &&
                m_vertexBufferInstances.size() >= maxInstanceData)
            {
                throw std::runtime_error{"Number of vertex buffer instances greater than " + std::to_string(maxInstanceData) + " is not supported"};
            }

            m_vertexBufferInstances[location] = {vertexBuffer, byteOffset, byteStride, static_cast<uint16_t>(sizeof(float) * numElements)};
        }
        else
        {
            auto attrib = static_cast<bgfx::Attrib::Enum>(location);
            if (m_vertexBufferRecords.find(attrib) != m_vertexBufferRecords.end())
            {
                throw std::runtime_error{"Multiple vertex buffers with the same attribute cannot be recorded"};
            }
            vertexBuffer->Build(byteStride);

            bgfx::VertexLayout layout{};
            layout.begin();
            layout.add(attrib, static_cast<uint8_t>(numElements), attribType, normalized);
            layout.m_stride = static_cast<uint16_t>(byteStride);
            layout.m_offset[attrib] = static_cast<uint16_t>(byteOffset % byteStride);
            layout.end();

            const auto layoutHandle = bgfx::createVertexLayout(layout);
            if (!bgfx::isValid(layoutHandle))
            {
                throw std::runtime_error{"Failed to create vertex layout (attribute=" + std::to_string(location) +
                    ", stride=" + std::to_string(byteStride) + ", offset=" + std::to_string(byteOffset) +
                    "). The maxVertexLayouts limit of " + std::to_string(bgfx::getCaps()->limits.maxVertexLayouts) + " may be exhausted"};
            }
            try
            {
                if (!m_vertexBufferRecords.try_emplace(attrib, vertexBuffer, byteOffset / byteStride, layoutHandle).second)
                {
                    throw std::runtime_error{"Multiple vertex buffers with the same attribute cannot be recorded"};
                }
            }
            catch (...)
            {
                bgfx::destroy(layoutHandle);
                throw;
            }
        }
    }

    void VertexArray::SetIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices)
    {
        if (m_indexBuffer != nullptr)
        {
            m_indexBuffer->Set(encoder, firstIndex, numIndices);
        }
    }

    void VertexArray::SetVertexBuffers(bgfx::Encoder* encoder, uint32_t startVertex, uint32_t numVertices, uint32_t instanceCount, const VertexBuffer::InstanceDataLayout& instanceDataLayout)
    {
        if (!m_vertexBufferInstances.empty())
        {
            bgfx::InstanceDataBuffer instanceDataBuffer{};
            VertexBuffer::BuildInstanceDataBuffer(instanceDataBuffer, m_vertexBufferInstances, instanceCount, instanceDataLayout);
            encoder->setInstanceDataBuffer(&instanceDataBuffer);
        }

        uint8_t stream = 0;
        for (const auto& pair : m_vertexBufferRecords)
        {
            auto& record{pair.second};
            record.Buffer->Set(encoder, stream++, record.Offset + startVertex, numVertices, record.LayoutHandle);
        }
    }
}
