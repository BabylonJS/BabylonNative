#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <gsl/gsl>

#include <optional>

namespace Babylon
{
    namespace Graphics 
    {
        class DeviceContext;
    }

    class IndexBuffer final
    {
    public:
        IndexBuffer(Graphics::DeviceContext& deviceContext, gsl::span<const uint8_t> bytes, uint16_t flags, bool dynamic);
        ~IndexBuffer();

        // No copy or move semantics
        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer(IndexBuffer&&) = delete;

        void Dispose();

        void Update(gsl::span<const uint8_t> bytes, uint32_t startIndex);

        void Build();

        void Set(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices);

    private:
        Graphics::DeviceContext& m_deviceContext;
        const uintptr_t m_deviceID{};

        std::vector<uint8_t> m_bytes{};
        const uint16_t m_flags{};
        const bool m_dynamic{};

        union
        {
            bgfx::IndexBufferHandle m_handle{bgfx::kInvalidHandle};
            bgfx::DynamicIndexBufferHandle m_dynamicHandle;
        };

        bool m_disposed{};
    };
}
