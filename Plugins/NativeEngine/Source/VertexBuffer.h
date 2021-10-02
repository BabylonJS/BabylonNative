#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <gsl/gsl>

namespace Babylon
{
    class VertexBuffer final
    {
    public:
        VertexBuffer(gsl::span<uint8_t> bytes, bool dynamic);
        ~VertexBuffer();

        void Update(Napi::Env env, gsl::span<uint8_t> bytes);
        void Record(uint32_t index, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized);
        void Set(bgfx::Encoder* encoder, uint32_t vertexStart, uint32_t vertexCount);

    private:
        std::vector<uint8_t> m_bytes{};
        bool m_dynamic{};

        bgfx::Attrib::Enum m_attrib{};
        uint32_t m_vertexOffset{};
        bgfx::VertexLayoutHandle m_layoutHandle{bgfx::kInvalidHandle};

        union
        {
            bgfx::VertexBufferHandle m_handle{bgfx::kInvalidHandle};
            bgfx::DynamicVertexBufferHandle m_dynamicHandle;
        };
    };
}
