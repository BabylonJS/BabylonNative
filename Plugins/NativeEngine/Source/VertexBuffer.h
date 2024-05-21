#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <gsl/gsl>
#include <list>
#include <map>
#include <optional>

namespace Babylon
{
    namespace Graphics 
    {
        class DeviceContext;
    }

    class VertexBuffer final
    {
    public:
        VertexBuffer(Graphics::DeviceContext& deviceContext, gsl::span<const uint8_t> bytes, bool dynamic);
        ~VertexBuffer();

        // No copy or move semantics
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) = delete;

        void Dispose();

        void Update(gsl::span<const uint8_t> bytes, size_t byteOffset);

        void Build(uint32_t byteStride);

        void Set(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layout);

        struct InstanceInfo
        {
            VertexBuffer* Buffer{};
            uint32_t Offset{};
            uint32_t Stride{};
            uint32_t ElementSize{};
        };

        static void BuildInstanceDataBuffer(bgfx::InstanceDataBuffer& instanceDataBuffer, const std::map<bgfx::Attrib::Enum, InstanceInfo>& instances, uint32_t instanceCount);

    private:
        Graphics::DeviceContext& m_deviceContext;
        const uintptr_t m_deviceId{};

        std::vector<uint8_t> m_bytes{};
        const bool m_dynamic{};
        uint32_t m_byteStride{};

        union
        {
            bgfx::VertexBufferHandle m_handle{bgfx::kInvalidHandle};
            bgfx::DynamicVertexBufferHandle m_dynamicHandle;
        };

        bool m_disposed{};
    };
};
