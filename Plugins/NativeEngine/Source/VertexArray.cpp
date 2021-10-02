#include "VertexArray.h"
#include <cassert>

namespace Babylon
{
    void VertexArray::RecordIndexBuffer(IndexBuffer& indexBuffer)
    {
        indexBuffer.Record();
        assert(m_indexBuffer == nullptr);
        m_indexBuffer = &indexBuffer;
    }

    void VertexArray::RecordVertexBuffer(VertexBuffer& vertexBuffer, uint32_t location, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized)
    {
        vertexBuffer.Record(location, byteOffset, byteStride, numElements, type, normalized);
        m_vertexBuffers.push_back(&vertexBuffer);
    }

    void VertexArray::SetIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices)
    {
        if (m_indexBuffer != nullptr)
        {
            m_indexBuffer->Set(encoder, firstIndex, numIndices);
        }
    }

    void VertexArray::SetVertexBuffers(bgfx::Encoder* encoder, uint32_t vertexStart, uint32_t vertexCount)
    {
        for (VertexBuffer* vertexBuffer : m_vertexBuffers)
        {
            vertexBuffer->Set(encoder, vertexStart, vertexCount);
        }
    }
}
