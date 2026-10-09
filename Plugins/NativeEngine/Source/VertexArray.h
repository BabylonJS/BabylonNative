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
        explicit VertexArray(Graphics::DeviceContext& deviceContext);
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        void Dispose();

        void RecordIndexBuffer(IndexBuffer* indexBuffer);
        void RecordVertexBuffer(VertexBuffer* vertexBuffer, uint32_t location, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized, uint32_t divisor);

        void SetIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices);
        bool SetExpandedIndexBuffer(bgfx::Encoder* encoder, PrimitiveModeExpansion::Mode mode, uint32_t firstIndex, uint32_t numIndices);
        bool SetExpandedUnindexedBuffer(bgfx::Encoder* encoder, PrimitiveModeExpansion::Mode mode, uint32_t numVertices);
        void SetVertexBuffers(bgfx::Encoder* encoder, uint32_t startVertex, uint32_t numVertices, uint32_t instanceCount, const VertexBuffer::InstanceDataLayout& instanceDataLayout);

        const std::map<uint32_t, VertexBuffer::InstanceInfo>& GetInstances() const { return m_vertexBufferInstances; }

    private:
        struct UnindexedExpansionKey final
        {
            PrimitiveModeExpansion::Mode Mode{};
            uint32_t VertexCount{};

            bool operator<(const UnindexedExpansionKey& other) const;
        };

        struct ExpandedBuffer final
        {
            bgfx::IndexBufferHandle Handle{bgfx::kInvalidHandle};
            uint32_t IndexCount{};
        };

        Graphics::DeviceContext& m_deviceContext;
        const uintptr_t m_deviceId{};
        IndexBuffer* m_indexBuffer{};
        std::map<UnindexedExpansionKey, ExpandedBuffer> m_unindexedExpandedBuffers{};

        struct VertexBufferRecord
        {
            VertexBuffer* Buffer{};
            const uint32_t Offset{};
            const bgfx::VertexLayoutHandle LayoutHandle{};

            VertexBufferRecord(VertexBuffer* buffer, const uint32_t offset, bgfx::VertexLayoutHandle layoutHandle)
                : Buffer{buffer}
                , Offset{offset}
                , LayoutHandle{layoutHandle}
            {
            }
        };

        std::map<bgfx::Attrib::Enum, VertexBufferRecord> m_vertexBufferRecords{};

        std::map<uint32_t, VertexBuffer::InstanceInfo> m_vertexBufferInstances;

        bool m_disposed{};
    };
}
