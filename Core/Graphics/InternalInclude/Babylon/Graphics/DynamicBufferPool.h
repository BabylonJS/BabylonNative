#pragma once

#include <bgfx/bgfx.h>
#include <cstdint>
#include <unordered_map>

namespace Babylon::Graphics
{
    class DeviceContext;

    // Supplies vertex and index data for draws submitted before the next bgfx::frame(), from dynamic buffers that
    // are kept alive and reused every frame. This replaces bgfx's transient buffers, whose size is fixed at init:
    // allocations within a frame take disjoint ranges of one buffer per vertex layout (and one 16-bit index buffer),
    // and a buffer that runs out of room is replaced by a larger one, so allocation settles once the buffers cover a
    // frame's data.
    // This class is not thread-safe. Callers must serialize all access.
    class DynamicBufferPool final
    {
    public:
        explicit DynamicBufferPool(DeviceContext& deviceContext);
        ~DynamicBufferPool();

        DynamicBufferPool(const DynamicBufferPool&) = delete;
        DynamicBufferPool& operator=(const DynamicBufferPool&) = delete;

        struct VertexRange
        {
            bgfx::DynamicVertexBufferHandle Handle{bgfx::kInvalidHandle};
            uint32_t StartVertex{};
        };

        struct IndexRange
        {
            bgfx::DynamicIndexBufferHandle Handle{bgfx::kInvalidHandle};
            uint32_t StartIndex{};
        };

        // Copies vertices (memory->size must be a multiple of the layout's stride) into the pool.
        VertexRange AllocVertices(const bgfx::VertexLayout& layout, const bgfx::Memory* memory);

        // Copies 16-bit indices into the pool.
        IndexRange AllocIndices(const bgfx::Memory* memory);

    private:
        template<typename HandleT>
        struct Buffer
        {
            HandleT Handle{bgfx::kInvalidHandle};
            uint32_t Capacity{};
            uint32_t Used{};
        };

        void BeginAllocation();

        DeviceContext& m_deviceContext;
        uintptr_t m_deviceId{};
        uint32_t m_generation{};
        std::unordered_map<uint64_t, Buffer<bgfx::DynamicVertexBufferHandle>> m_vertexBuffers{};
        Buffer<bgfx::DynamicIndexBufferHandle> m_indexBuffer{};
    };
}
