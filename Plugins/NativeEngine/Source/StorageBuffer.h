#pragma once

#include <bgfx/bgfx.h>
#include <gsl/gsl>
#include <vector>

namespace Babylon
{
    namespace Graphics
    {
        class DeviceContext;
    }

    // Wraps a bgfx dynamic vertex buffer that compute shaders bind as a raw (ByteAddressBuffer)
    // buffer and draws bind as a vertex or instance stream. It is created lazily and seeded from a
    // CPU shadow, so a buffer can be filled before its first use.
    class StorageBuffer final
    {
    public:
        // byteStride is the bgfx vertex-layout stride used when the buffer is bound as a vertex or
        // instance stream; instance data destinations pass (numSlots * 16). computeWrite = false
        // omits BGFX_BUFFER_COMPUTE_WRITE so the buffer stays CPU-updatable.
        StorageBuffer(Graphics::DeviceContext& deviceContext, uint32_t byteLength, uint32_t byteStride = 16, bool computeWrite = true);
        ~StorageBuffer();

        // No copy or move semantics.
        StorageBuffer(const StorageBuffer&) = delete;
        StorageBuffer(StorageBuffer&&) = delete;

        void Dispose();

        // Writes bytes into the CPU shadow and the GPU buffer at byteOffset, which must be a multiple of the stride.
        void Update(gsl::span<const uint8_t> bytes, uint32_t byteOffset);

        // Binds the buffer as a compute resource on the given stage.
        void SetCompute(bgfx::Encoder* encoder, uint8_t stage, bgfx::Access::Enum access);

        // Binds the buffer as a vertex stream.
        void SetVertex(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layout);

        uint32_t ByteLength() const { return m_byteLength; }
        gsl::span<const uint8_t> ShadowBytes() const { return gsl::make_span(m_shadow.data(), m_shadow.size()); }
        bgfx::DynamicVertexBufferHandle Handle();

    private:
        void ValidateUpdate(size_t byteLength, uint32_t byteOffset) const;
        void EnsureCreated();
        void Upload(gsl::span<const uint8_t> bytes, uint32_t byteOffset);

        Graphics::DeviceContext& m_deviceContext;
        const uintptr_t m_deviceId{};

        const uint32_t m_byteLength{};
        const bool m_computeWrite{true};
        const uint32_t m_byteStride{};

        std::vector<uint8_t> m_shadow{};
        bgfx::DynamicVertexBufferHandle m_handle{bgfx::kInvalidHandle};

        bool m_disposed{};
    };
}
