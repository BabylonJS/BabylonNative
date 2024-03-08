#pragma once

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include <set>
#include <map>

namespace Babylon
{
    class VertexArray final
    {
    public:
        VertexArray() = default;
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        void Dispose();

        void RecordIndexBuffer(IndexBuffer* indexBuffer);
        void RecordVertexBuffer(VertexBuffer* vertexBuffer, uint32_t location, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized, uint32_t divisor);

        void SetIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices);
        void SetVertexBuffers(bgfx::Encoder* encoder, uint32_t startVertex, uint32_t numVertices, uint32_t instanceCount = 0);

    private:
        IndexBuffer* m_indexBuffer{};
        std::set<VertexBuffer*> m_vertexBuffers;
        std::map<bgfx::Attrib::Enum, VertexBuffer::InstanceInfo> m_vertexBufferInstances;

        bool m_disposed{};
    };
}
