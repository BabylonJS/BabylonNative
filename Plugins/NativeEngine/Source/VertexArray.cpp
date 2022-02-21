#include "VertexArray.h"
#include <cassert>

namespace Babylon
{
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

        m_indexBufferRecord.Buffer = nullptr;

        for (auto& pair : m_vertexBufferRecords)
        {
            bgfx::destroy(pair.second.LayoutHandle);
        }

        m_vertexBufferRecords.clear();

        m_disposed = true;
    }

 

    bool VertexArray::RecordIndexBuffer(IndexBuffer* indexBuffer)
    {
        if (!indexBuffer->CreateHandle())
        {
            return false;
        }

        assert(m_indexBufferRecord.Buffer == nullptr);
        m_indexBufferRecord.Buffer = indexBuffer;

        return true;
    }

    bool VertexArray::RecordVertexBuffer(VertexBuffer* vertexBuffer, uint32_t location, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized, uint32_t divisor)
    {
        auto attrib{static_cast<bgfx::Attrib::Enum>(location)};
        if (divisor == 1)
        {
            const bgfx::Caps* caps = bgfx::getCaps();

            // Check if instancing is supported.
            const bool instancingSupported = 0 != (BGFX_CAPS_INSTANCING & caps->supported);
            if (!instancingSupported)
            {
                return false;
            }
            // bgfx allows instancing on 5 vec4 attributes
            if (m_vertexBufferInstanceRecords.size() < 5)
            {
                m_vertexBufferInstanceRecords[attrib] = {vertexBuffer, byteOffset, byteStride, static_cast<uint16_t>(sizeof(float) * numElements)};
            }
            else
            {
                return false;
            }
        }
        else
        {
            bgfx::VertexLayout layout{};
            layout.begin();

            auto attribType{static_cast<bgfx::AttribType::Enum>(type)};

            // clang-format off
            const bool promoteToFloats = !normalized
                && (bgfx::getCaps()->rendererType == bgfx::RendererType::Direct3D11 ||
                    bgfx::getCaps()->rendererType == bgfx::RendererType::Direct3D12 ||
                    bgfx::getCaps()->rendererType == bgfx::RendererType::Vulkan)
                && (attribType == bgfx::AttribType::Int8 ||
                    attribType == bgfx::AttribType::Uint8 ||
                    attribType == bgfx::AttribType::Uint10 ||
                    attribType == bgfx::AttribType::Int16 ||
                    attribType == bgfx::AttribType::Uint16);
            // clang-format on

            if (promoteToFloats)
            {
                layout.add(attrib, static_cast<uint8_t>(numElements), bgfx::AttribType::Float);
                layout.m_stride = static_cast<uint16_t>(sizeof(float) * numElements);
                vertexBuffer->PromoteToFloats(attribType, numElements, byteOffset, byteStride);
            }
            else
            {
                layout.add(attrib, static_cast<uint8_t>(numElements), attribType, normalized);
                layout.m_stride = static_cast<uint16_t>(byteStride);
                layout.m_offset[attrib] = static_cast<uint16_t>(byteOffset % byteStride);
            }

            layout.end();

            if (!vertexBuffer->CreateHandle(layout))
            {
                return false;
            }

            m_vertexBufferRecords[attrib] = {vertexBuffer, byteOffset / byteStride, bgfx::createVertexLayout(layout)};
        }
        return true;
    }

    void VertexArray::SetIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices)
    {
        if (m_indexBufferRecord.Buffer != nullptr)
        {
            m_indexBufferRecord.Buffer->Set(encoder, firstIndex, numIndices);
        }
    }

    void VertexArray::SetVertexBuffers(bgfx::Encoder* encoder, uint32_t startVertex, uint32_t numVertices)
    {
        const bgfx::Caps* caps = bgfx::getCaps();

        // Check if instancing is supported.
        const bool instancingSupported = 0 != (BGFX_CAPS_INSTANCING & caps->supported);
        if (!m_vertexBufferInstanceRecords.empty() && instancingSupported)
        {
            bgfx::InstanceDataBuffer instanceDataBuffer{};
            VertexBuffer::BuildInstanceDataBuffer(instanceDataBuffer, m_vertexBufferInstanceRecords);
            encoder->setInstanceDataBuffer(&instanceDataBuffer);
        }

        for (const auto& pair : m_vertexBufferRecords)
        {
            auto stream{static_cast<uint8_t>(pair.first)};
            auto& record{pair.second};
            record.Buffer->Set(encoder, stream, record.Offset + startVertex, numVertices, record.LayoutHandle);
        }
    }
}
