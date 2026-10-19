#pragma once

#include "PrimitiveModeExpansion.h"

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <gsl/gsl>

#include <map>

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
        bool SetExpanded(bgfx::Encoder* encoder, PrimitiveModeExpansion::Mode mode, uint32_t firstIndex, uint32_t numIndices);

    private:
        struct ExpansionKey final
        {
            PrimitiveModeExpansion::Mode Mode{};
            uint32_t FirstIndex{};
            uint32_t IndexCount{};

            bool operator<(const ExpansionKey& other) const;
        };

        struct ExpandedBuffer final
        {
            bgfx::IndexBufferHandle Handle{bgfx::kInvalidHandle};
            uint32_t IndexCount{};
        };

        void DestroyExpandedBuffers();

        Graphics::DeviceContext& m_deviceContext;
        const uintptr_t m_deviceID{};

        PrimitiveModeExpansion::IndexData m_data;
        const uint16_t m_flags{};
        const bool m_dynamic{};
        std::map<ExpansionKey, ExpandedBuffer> m_expandedBuffers{};

        union
        {
            bgfx::IndexBufferHandle m_handle{bgfx::kInvalidHandle};
            bgfx::DynamicIndexBufferHandle m_dynamicHandle;
        };

        bool m_disposed{};
    };
}
