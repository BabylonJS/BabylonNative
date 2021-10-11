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
        void Create(const bgfx::VertexLayout& layout);
        void PromoteToFloats(bgfx::AttribType::Enum attribType, uint32_t numElements, uint32_t byteOffset, uint32_t byteStride);
        void Set(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layoutHandle);

    private:
        std::vector<uint8_t> m_bytes{};
        bool m_dynamic{};

        union
        {
            bgfx::VertexBufferHandle m_handle{bgfx::kInvalidHandle};
            bgfx::DynamicVertexBufferHandle m_dynamicHandle;
        };
    };
}
