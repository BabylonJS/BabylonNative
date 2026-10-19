#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <gsl/gsl>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace Babylon
{
    namespace Graphics 
    {
        class DeviceContext;
    }

    class StorageBuffer;

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

        // Moves the contents into a GPU storage buffer so compute work (transform feedback
        // emulation) can read and write it; later updates, vertex binds and instance repacks use the
        // storage buffer. Must happen before the buffer is built as a regular vertex stream.
        StorageBuffer& PromoteToGpuStorage();
        StorageBuffer* GpuStorage() const { return m_gpuStorage.get(); }
        uint32_t ByteLength() const;

        // Binds the contents as a read-only raw compute buffer (transform feedback vertex input). A
        // buffer that was never built is promoted to GPU storage; a built one must be compute readable.
        void SetComputeRead(bgfx::Encoder* encoder, uint8_t stage);

        struct InstanceInfo
        {
            VertexBuffer* Buffer{};
            uint32_t Offset{};
            uint32_t Stride{};
            uint32_t ElementSize{};

            // The GPU storage holding this attribute when Buffer received transform feedback output.
            // Such instances are repacked on the GPU (see InstanceRepacker) instead of on the CPU.
            StorageBuffer* ResolveStorage() const;
        };

        struct InstanceDataLayout
        {
            uint32_t SlotCount{};
            std::map<uint32_t, uint32_t> Slots{};
        };

        static InstanceDataLayout CreateInstanceDataLayout(
            const std::map<uint32_t, InstanceInfo>& instances,
            const std::map<uint32_t, uint32_t>& builtInSlots,
            uint32_t maxSlotCount);

        static void BuildInstanceDataBuffer(
            bgfx::InstanceDataBuffer& instanceDataBuffer,
            const std::map<uint32_t, InstanceInfo>& instances,
            uint32_t instanceCount,
            const InstanceDataLayout& layout);

    private:
        Graphics::DeviceContext& m_deviceContext;
        const uintptr_t m_deviceId{};

        std::vector<uint8_t> m_bytes{};
        const bool m_dynamic{};
        uint32_t m_byteStride{};
        bool m_computeReadable{};

        union
        {
            bgfx::VertexBufferHandle m_handle{bgfx::kInvalidHandle};
            bgfx::DynamicVertexBufferHandle m_dynamicHandle;
        };

        std::unique_ptr<StorageBuffer> m_gpuStorage{};

        bool m_disposed{};
    };
};
