#include "VertexArray.h"
#include <cassert>
#include <string>
#include <tuple>
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
            for (const auto& pair : m_vertexBufferRecords)
            {
                const auto& record{pair.second};
                if (bgfx::isValid(record.LayoutHandle))
                {
                    bgfx::destroy(record.LayoutHandle);
                }
            }
        }
        m_vertexBufferRecords.clear();
        m_vertexBufferInstances.clear();
        if (m_deviceId == m_deviceContext.GetDeviceId())
        {
            for (const auto& [key, buffer] : m_unindexedExpandedBuffers)
            {
                static_cast<void>(key);
                if (bgfx::isValid(buffer.Handle))
                {
                    bgfx::destroy(buffer.Handle);
                }
            }
        }
        m_unindexedExpandedBuffers.clear();

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
        if (m_deviceId != m_deviceContext.GetDeviceId())
        {
            throw std::runtime_error{"Cannot record a vertex buffer in a stale vertex array after device loss"};
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

            const auto layoutHandle = m_deviceContext.CreateVertexLayout(layout);
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

    bool VertexArray::SetExpandedIndexBuffer(bgfx::Encoder* encoder, PrimitiveModeExpansion::Mode mode, uint32_t firstIndex, uint32_t numIndices)
    {
        if (m_indexBuffer == nullptr)
        {
            throw std::runtime_error{"Indexed primitive expansion requires an index buffer"};
        }

        return m_indexBuffer->SetExpanded(encoder, mode, firstIndex, numIndices);
    }

    bool VertexArray::SetExpandedUnindexedBuffer(bgfx::Encoder* encoder, PrimitiveModeExpansion::Mode mode, uint32_t numVertices)
    {
        const UnindexedExpansionKey key{mode, numVertices};
        auto existing = m_unindexedExpandedBuffers.find(key);
        if (existing == m_unindexedExpandedBuffers.end())
        {
            auto expanded = PrimitiveModeExpansion::ExpandUnindexed(numVertices, mode);
            if (expanded.IndexCount == 0)
            {
                return false;
            }

            const uint16_t flags = expanded.Index32 ? BGFX_BUFFER_INDEX32 : 0;
            const bgfx::Memory* memory = bgfx::copy(expanded.Bytes.data(), static_cast<uint32_t>(expanded.Bytes.size()));
            const bgfx::IndexBufferHandle handle = bgfx::createIndexBuffer(memory, flags);
            if (!bgfx::isValid(handle))
            {
                throw std::runtime_error{"Failed to create expanded primitive index buffer"};
            }

            try
            {
                existing = m_unindexedExpandedBuffers.emplace(key, ExpandedBuffer{handle, expanded.IndexCount}).first;
            }
            catch (...)
            {
                bgfx::destroy(handle);
                throw;
            }
        }

        encoder->setIndexBuffer(existing->second.Handle, 0, existing->second.IndexCount);
        return true;
    }

    bool VertexArray::UnindexedExpansionKey::operator<(const UnindexedExpansionKey& other) const
    {
        return std::tie(Mode, VertexCount) < std::tie(other.Mode, other.VertexCount);
    }

    void VertexArray::SetVertexBuffers(bgfx::Encoder* encoder, uint32_t startVertex, uint32_t numVertices, uint32_t instanceCount, const VertexBuffer::InstanceDataLayout& instanceDataLayout)
    {
        if (!m_vertexBufferInstances.empty())
        {
            bgfx::InstanceDataBuffer instanceDataBuffer{};
            VertexBuffer::BuildInstanceDataBuffer(instanceDataBuffer, m_vertexBufferInstances, instanceCount, instanceDataLayout);
            encoder->setInstanceDataBuffer(&instanceDataBuffer);
        }
        else if (instanceCount > 0)
        {
            // Attribute-less instancing: the draw requests multiple instances but has no per-instance
            // vertex data (e.g. the clustered-light tile-mask proxies, which derive the light index,
            // vertical batch offset and bit mask purely from gl_InstanceID). Without an instance data
            // buffer bgfx would fall back to drawing a single instance, leaving the tile mask empty and
            // breaking clustered lighting. Tell bgfx the instance count explicitly so gl_InstanceID
            // iterates 0..instanceCount-1.
            encoder->setInstanceCount(instanceCount);
        }

        uint8_t stream = 0;
        for (const auto& pair : m_vertexBufferRecords)
        {
            auto& record{pair.second};
            record.Buffer->Set(encoder, stream++, record.Offset + startVertex, numVertices, record.LayoutHandle);
        }
    }
}
