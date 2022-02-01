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

    void VertexArray::RecordIndexBuffer(IndexBuffer* indexBuffer)
    {
        indexBuffer->CreateHandle();

        assert(m_indexBufferRecord.Buffer == nullptr);
        m_indexBufferRecord.Buffer = indexBuffer;
    }

    void VertexArray::RecordVertexBuffer(VertexBuffer* vertexBuffer, uint32_t location, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized, uint32_t divisor)
    {
        auto attrib{static_cast<bgfx::Attrib::Enum>(location)};
        if (divisor)
        {
            // bgfx allows instancing on TexCoord3 - TexCoord7 only
            if (attrib >= bgfx::Attrib::TexCoord3 && attrib <= bgfx::Attrib::TexCoord7)
            {
                m_vertexBufferInstanceRecords[attrib] = {vertexBuffer, byteOffset, byteStride, static_cast<uint16_t>(sizeof(float) * numElements)};
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
            vertexBuffer->CreateHandle(layout);

            m_vertexBufferRecords[attrib] = {vertexBuffer, byteOffset / byteStride, bgfx::createVertexLayout(layout)};
        }
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

            // instance stride
            uint16_t instanceStride{};
            uint32_t instanceCount{};
            for (auto& pair : m_vertexBufferInstanceRecords)
            {
                const auto vertexBuffer{pair.second.Buffer};
                instanceCount = static_cast<uint32_t>(vertexBuffer->GetBytes().size()) / pair.second.Stride;
                instanceStride += static_cast<uint16_t>(pair.second.ElementSize);
            }

            // create instance datas. Instance Data Buffer is transient.
            bgfx::allocInstanceDataBuffer(&instanceDataBuffer, instanceCount, instanceStride);

            // copy instance datas
            uint8_t* data{instanceDataBuffer.data};
            uint32_t offset{};

            // reverse because of bgfx also reverting : https://github.com/bkaradzic/bgfx/blob/4581f14cd481bad1e0d6292f0dd0a6e298c2ee18/src/renderer_d3d11.cpp#L2701
            for (auto iter = m_vertexBufferInstanceRecords.rbegin(); iter != m_vertexBufferInstanceRecords.rend(); ++iter)
            {
                const auto& element{iter->second};
                const auto* source{element.Buffer->GetBytes().data()};
                for (uint32_t instance = 0; instance < instanceCount; instance++)
                {
                    memcpy(data + instance * instanceStride + offset, source + instance * element.Stride + element.Offset, element.ElementSize);
                }
                offset += iter->second.ElementSize;
            }
            encoder->setInstanceDataBuffer(&instanceDataBuffer);
        }

        for (auto& pair : m_vertexBufferRecords)
        {
            auto stream{static_cast<uint8_t>(pair.first)};
            auto& record{pair.second};
            record.Buffer->Set(encoder, stream, record.Offset + startVertex, numVertices, record.LayoutHandle);
        }
    }
}
