#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <gsl/gsl>

namespace Babylon
{
    class IndexBuffer final
    {
    public:
        IndexBuffer(gsl::span<uint8_t> bytes, uint16_t flags, bool dynamic);
        ~IndexBuffer();

        void Update(Napi::Env env, gsl::span<uint8_t> bytes, uint32_t startIndex);
        void Create();
        void Set(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices);

    private:
        std::vector<uint8_t> m_bytes{};
        uint16_t m_flags{};
        bool m_dynamic{};

        union
        {
            bgfx::IndexBufferHandle m_handle{bgfx::kInvalidHandle};
            bgfx::DynamicIndexBufferHandle m_dynamicHandle;
        };
    };
}
