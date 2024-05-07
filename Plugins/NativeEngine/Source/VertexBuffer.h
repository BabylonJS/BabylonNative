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
        VertexBuffer(Graphics::DeviceContext& deviceContext, const gsl::span<uint8_t> bytes, bool dynamic);
        ~VertexBuffer();

        // No copy or move semantics
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) = delete;

        void Dispose();

        void Update(const gsl::span<uint8_t> bytes, size_t byteOffset);

        uint8_t Add(bgfx::Attrib::Enum attrib, bgfx::AttribType::Enum attribType, uint32_t byteOffset, uint16_t byteStride, uint8_t numElements, bool normalized);

        void Set(bgfx::Encoder* encoder, uint8_t streamIndex, uint8_t encoderStreamIndex, uint32_t startVertex, uint32_t numVertices);

        struct InstanceInfo
        {
            VertexBuffer* Buffer{};
            uint32_t Offset{};
            uint32_t Stride{};
            uint32_t ElementSize{};
        };

        static void BuildInstanceDataBuffer(bgfx::InstanceDataBuffer& instanceDataBuffer, const std::map<bgfx::Attrib::Enum, InstanceInfo>& instances, uint32_t instanceCount);

    private:
        class Handle final
        {
        public:
            Handle(Graphics::DeviceContext& deviceContext, std::vector<uint8_t> bytes, bool dynamic, uint16_t byteStride);
            ~Handle();

            // Copy semantics
            Handle(const Handle&) = delete;
            Handle& operator=(const Handle&) = delete;

            // Move semantics
            Handle(Handle&&) noexcept;
            Handle& operator=(Handle&&) noexcept;

            void Update(gsl::span<uint8_t> bytes, uint32_t startVertex);

            void Set(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layoutHandle);

        private:
            Graphics::DeviceContext& m_deviceContext;
            const uintptr_t m_deviceId{};
            bool m_dynamic{};

            union
            {
                bgfx::VertexBufferHandle m_handle{bgfx::kInvalidHandle};
                bgfx::DynamicVertexBufferHandle m_dynamicHandle;
            };
        };

        struct StreamInfo
        {
            const bgfx::Attrib::Enum Attrib{};
            const bgfx::AttribType::Enum AttribType{};
            const uint32_t ByteOffset{};
            const uint8_t NumElements{};
            const bool Normalized{};

            std::optional<Handle> PromoteToFloatsHandle{};
            uint32_t StartVertexOffset{};
            bgfx::VertexLayoutHandle LayoutHandle{bgfx::kInvalidHandle};

            explicit StreamInfo(bgfx::Attrib::Enum attrib, bgfx::AttribType::Enum attribType, uint32_t byteOffset, uint8_t numElements, bool normalized)
                : Attrib{attrib}
                , AttribType{attribType}
                , ByteOffset{byteOffset}
                , NumElements{numElements}
                , Normalized{normalized}
            {
            }
        };

        void Build(StreamInfo& stream, uint32_t numVertices);

        Graphics::DeviceContext& m_deviceContext;

        std::vector<uint8_t> m_bytes{};
        const bool m_dynamic{};

        uint16_t m_byteStride{};

        std::vector<StreamInfo> m_streams{};

        std::optional<Handle> m_handle{};

        bool m_disposed{};
    };
};
