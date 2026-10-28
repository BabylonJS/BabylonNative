#pragma once

#include "VertexBuffer.h"

#include <bgfx/bgfx.h>

#include <map>
#include <memory>

namespace Babylon
{
    namespace Graphics
    {
        class DeviceContext;
        class FrameBuffer;
    }

    class StorageBuffer;
    class VertexArray;

    // Repacks GPU-written instance data into bgfx's 16-byte i_data slots on the GPU.
    class InstanceRepacker final
    {
    public:
        explicit InstanceRepacker(Graphics::DeviceContext& deviceContext);
        ~InstanceRepacker();

        InstanceRepacker(const InstanceRepacker&) = delete;
        InstanceRepacker& operator=(const InstanceRepacker&) = delete;

        // The frame buffer view orders the compute dispatch before the subsequent draw.
        bgfx::DynamicVertexBufferHandle Repack(Graphics::FrameBuffer& frameBuffer, VertexArray* vertexArray,
            const std::map<uint32_t, VertexBuffer::InstanceInfo>& instances, const VertexBuffer::InstanceDataLayout& layout,
            uint32_t instanceCount);

        void Forget(VertexArray* vertexArray);

    private:
        static constexpr uint32_t kMaxAttributes = 32;

        struct PerVertexArray
        {
            // StorageBuffer allows both raw compute writes and instance-data binding.
            std::shared_ptr<StorageBuffer> DestStorage;
            uint32_t Capacity{}; // Instances
            uint16_t Stride{};   // Bytes per instance
        };

        void EnsureProgram();
        void DestroyDest(PerVertexArray& state);

        Graphics::DeviceContext& m_deviceContext;
        const uintptr_t m_deviceId;
        bgfx::ProgramHandle m_program{bgfx::kInvalidHandle};
        std::map<VertexArray*, PerVertexArray> m_perVertexArray;
    };
}
