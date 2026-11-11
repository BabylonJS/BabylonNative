#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <gsl/gsl>
#include <map>
#include <memory>
#include <vector>

namespace Babylon
{
    namespace Graphics 
    {
        class DeviceContext;
    }

    class StorageBuffer;

    // Supplies per-draw instance data from dynamic vertex buffers that are kept alive and reused every frame,
    // rather than the fixed-size transient pool, which large instance counts drawn many times per frame (e.g.
    // Gaussian splat voxelization) exhaust. Draws within a frame take disjoint ranges of one buffer per stride,
    // so the draw count is not limited by bgfx's dynamic vertex buffer handles; a buffer that runs out of room
    // is replaced with a larger one, so allocation settles once the buffer covers a frame's instance data.
    class InstanceDataBufferPool final
    {
    public:
        explicit InstanceDataBufferPool(Graphics::DeviceContext& deviceContext);
        ~InstanceDataBufferPool();

        InstanceDataBufferPool(const InstanceDataBufferPool&) = delete;
        InstanceDataBufferPool& operator=(const InstanceDataBufferPool&) = delete;

        void Set(bgfx::Encoder* encoder, const bgfx::Memory* memory, uint16_t instanceStride, uint32_t instanceCount);

    private:
        struct Buffer
        {
            bgfx::DynamicVertexBufferHandle Handle{bgfx::kInvalidHandle};
            uint32_t Capacity{};
            uint32_t Used{};
        };

        Graphics::DeviceContext& m_deviceContext;
        uintptr_t m_deviceId{};
        uint32_t m_generation{};
        std::map<uint16_t, Buffer> m_buffers{};
    };

    class VertexBuffer final
    {
    public:
        VertexBuffer(Graphics::DeviceContext& deviceContext, gsl::span<const uint8_t> bytes, bool dynamic);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) = delete;

        void Dispose();

        void Update(gsl::span<const uint8_t> bytes, size_t byteOffset);

        void Build(uint32_t byteStride);

        void Set(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layout);

        // Moves the contents into a GPU storage buffer so compute work (transform feedback
        // emulation) can read and write it; later updates, vertex binds and instance repacks use the
        // storage buffer. Must happen before the buffer is built as a regular vertex stream. A word
        // aligned vertexByteStride becomes the storage element stride, so vertex binds with that
        // stride match it.
        StorageBuffer& PromoteToGpuStorage(uint32_t vertexByteStride = 0);
        StorageBuffer* GpuStorage() const { return m_gpuStorage.get(); }
        void ReplaceGpuStorage(std::unique_ptr<StorageBuffer> storage);
        uint32_t ByteLength() const;
        // Stride of the vertex layouts the buffer is drawn with, or 0 before it is first built.
        uint32_t ByteStride() const { return m_byteStride; }

        // Binds the contents as a read-only raw compute buffer (transform feedback vertex input). A
        // buffer that was never built is promoted to GPU storage; a built one must be compute readable.
        void SetComputeRead(bgfx::Encoder* encoder, uint8_t stage, uint32_t vertexByteStride);

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

        static void SetInstanceDataBuffer(
            bgfx::Encoder* encoder,
            InstanceDataBufferPool& pool,
            const std::map<uint32_t, InstanceInfo>& instances,
            uint32_t instanceCount,
            const InstanceDataLayout& layout);

        // Binds zeroed instance data for layouts whose declared instance inputs have no data.
        static void SetZeroInstanceDataBuffer(bgfx::Encoder* encoder, InstanceDataBufferPool& pool, uint32_t instanceCount, uint32_t slotCount);

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
