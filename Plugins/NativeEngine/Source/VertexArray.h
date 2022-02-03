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

        bool RecordIndexBuffer(IndexBuffer* indexBuffer);
        bool RecordVertexBuffer(VertexBuffer* vertexBuffer, uint32_t location, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized);
        void SetIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices);
        void SetVertexBuffers(bgfx::Encoder* encoder, uint32_t startVertex, uint32_t numVertices);
        bool IsValid();

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

        std::map<bgfx::Attrib::Enum, VertexBufferRecord> m_vertexBufferRecords{};

        bool m_disposed{};
        bool m_isValid{true};
    };
}
