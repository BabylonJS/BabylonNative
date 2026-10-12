#pragma once

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include <Babylon/Graphics/BgfxShaderInfo.h>
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

        // True when any recorded instance source is a GPU storage buffer, in which case the CPU
        // BuildInstanceDataBuffer path is skipped and NativeEngine binds a GPU-repacked buffer.
        bool HasStorageInstances() const;

        // Vertex inputs of a transform feedback program (locations at or above
        // TRANSFORM_FEEDBACK_ATTRIBUTE_LOCATION_BASE), keyed by input index. The emulating compute
        // shader reads and decodes them from the vertex buffers bound as raw compute buffers.
        struct TransformFeedbackInput
        {
            VertexBuffer* Buffer{};
            uint32_t ByteOffset{};
            uint32_t ByteStride{};
            uint32_t NumElements{};
            Graphics::TransformFeedbackInputType Type{};
            bool Normalized{};
        };
        const std::map<uint32_t, TransformFeedbackInput>& GetTransformFeedbackInputs() const { return m_transformFeedbackInputs; }
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
        std::map<uint32_t, TransformFeedbackInput> m_transformFeedbackInputs;

        bool m_disposed{};
    };
}
