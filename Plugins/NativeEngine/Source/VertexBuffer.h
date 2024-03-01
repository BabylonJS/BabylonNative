#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <gsl/gsl>
#include <map>
#include <optional>

namespace Babylon
{
    class VertexBuffer final
    {
    public:
        VertexBuffer(gsl::span<uint8_t> bytes, bool dynamic);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        void Dispose();

        void Update(Napi::Env env, gsl::span<uint8_t> bytes, size_t byteOffset);
        bool CreateHandle(const bgfx::VertexLayout& layout);
        void PromoteToFloats(bgfx::AttribType::Enum attribType, uint32_t numElements, uint32_t byteOffset, uint32_t byteStride);
        void Set(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layoutHandle);

        struct InstanceVertexBufferRecord
        {
            VertexBuffer* Buffer{};
            uint32_t Offset{};
            uint32_t Stride{};
            uint32_t ElementSize{};
        };

        static void BuildInstanceDataBuffer(bgfx::InstanceDataBuffer& instanceDataBuffer, const std::map<bgfx::Attrib::Enum, InstanceVertexBufferRecord>& vertexBufferInstance, uint32_t instanceCount);

    private:
        std::optional<std::vector<uint8_t>> m_bytes{};
        bool m_dynamic{};

        union
        {
            bgfx::VertexBufferHandle m_handle{bgfx::kInvalidHandle};
            bgfx::DynamicVertexBufferHandle m_dynamicHandle;
        };

        bool m_disposed{};
        uintptr_t m_deviceID;
    };
}
