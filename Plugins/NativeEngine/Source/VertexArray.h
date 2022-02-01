#pragma once

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include <map>

namespace Babylon
{
    class VertexArray final
    {
    public:
        ~VertexArray();

        void Dispose();

        void RecordIndexBuffer(IndexBuffer* indexBuffer);
        void RecordVertexBuffer(VertexBuffer* vertexBuffer, uint32_t location, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized, uint32_t divisor);
        void SetIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices);
        void SetVertexBuffers(bgfx::Encoder* encoder, uint32_t startVertex, uint32_t numVertices);

    private:
        struct IndexBufferRecord
        {
            IndexBuffer* Buffer{};
        };

        IndexBufferRecord m_indexBufferRecord{};

        struct VertexBufferRecord
        {
            VertexBuffer* Buffer{};
            uint32_t Offset{};
            bgfx::VertexLayoutHandle LayoutHandle{};
        };

        struct InstanceVertexBufferRecord
        {
            VertexBuffer* Buffer{};
            uint32_t Offset{};
            uint32_t Stride{};
            uint32_t ElementSize{};
        };

        std::map<bgfx::Attrib::Enum, VertexBufferRecord> m_vertexBufferRecords{};
        std::map<bgfx::Attrib::Enum, InstanceVertexBufferRecord> m_vertexBufferInstanceRecords{};

        bool m_disposed{};
    };
}
