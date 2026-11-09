#pragma once

#include <bgfx/bgfx.h>

#include <memory>

namespace Babylon
{
    namespace Graphics
    {
        class DeviceContext;
        class FrameBuffer;
    }

    class StorageBuffer;

    // Copies StorageBuffer contents on the GPU, where transform feedback may have written them.
    class StorageBufferCopier final
    {
    public:
        explicit StorageBufferCopier(Graphics::DeviceContext& deviceContext);
        ~StorageBufferCopier();

        StorageBufferCopier(const StorageBufferCopier&) = delete;
        StorageBufferCopier& operator=(const StorageBufferCopier&) = delete;

        // The frame buffer view orders the compute dispatch before the subsequent draw.
        std::unique_ptr<StorageBuffer> CopyWithStride(bgfx::Encoder& encoder, Graphics::FrameBuffer& frameBuffer,
            StorageBuffer& source, uint32_t byteStride);

    private:
        void EnsureProgram();

        Graphics::DeviceContext& m_deviceContext;
        const uintptr_t m_deviceId;
        bgfx::ProgramHandle m_program{bgfx::kInvalidHandle};
        bgfx::UniformHandle m_copyCount{bgfx::kInvalidHandle};
    };
}
