#include "StorageBufferCopier.h"

#include "StorageBuffer.h"

#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>

#include <bgfx/embedded_shader.h>

#include <stdexcept>

#include "Shaders/dxbc/cs_buffer_copy.h"
#include "Shaders/dxil/cs_buffer_copy.h"
#include "Shaders/essl/cs_buffer_copy.h"
#include "Shaders/glsl/cs_buffer_copy.h"
#include "Shaders/metal/cs_buffer_copy.h"
#include "Shaders/spirv/cs_buffer_copy.h"

namespace Babylon
{
    namespace
    {
        const bgfx::EmbeddedShader CopyShaders[]{
            BGFX_EMBEDDED_SHADER(cs_buffer_copy),
            BGFX_EMBEDDED_SHADER_END()};

        constexpr uint8_t SOURCE_BINDING = 0;
        constexpr uint8_t DESTINATION_BINDING = 1;
        constexpr uint32_t COMPUTE_WORKGROUP_SIZE = 64;
    }

    StorageBufferCopier::StorageBufferCopier(Graphics::DeviceContext& deviceContext)
        : m_deviceContext{deviceContext}
        , m_deviceId{deviceContext.GetDeviceId()}
    {
    }

    StorageBufferCopier::~StorageBufferCopier()
    {
        if (m_deviceId != m_deviceContext.GetDeviceId())
        {
            return;
        }
        if (bgfx::isValid(m_program))
        {
            bgfx::destroy(m_program);
        }
        if (bgfx::isValid(m_copyCount))
        {
            bgfx::destroy(m_copyCount);
        }
    }

    void StorageBufferCopier::EnsureProgram()
    {
        if (bgfx::isValid(m_program))
        {
            return;
        }

        const auto shader = bgfx::createEmbeddedShader(CopyShaders, bgfx::getRendererType(), "cs_buffer_copy");
        if (!bgfx::isValid(shader))
        {
            throw std::runtime_error{"Failed to create embedded buffer copy shader"};
        }
        m_program = bgfx::createProgram(shader, true);
        if (!bgfx::isValid(m_program))
        {
            throw std::runtime_error{"Failed to create embedded buffer copy program"};
        }
        m_copyCount = bgfx::createUniform("u_copyCount", bgfx::UniformType::Vec4);
    }

    std::unique_ptr<StorageBuffer> StorageBufferCopier::CopyWithStride(bgfx::Encoder& encoder, Graphics::FrameBuffer& frameBuffer,
        StorageBuffer& source, uint32_t byteStride)
    {
        EnsureProgram();

        auto destination = std::make_unique<StorageBuffer>(m_deviceContext, source.ByteLength(), byteStride);
        const uint32_t wordCount = (source.ByteLength() + sizeof(uint32_t) - 1) / sizeof(uint32_t);
        const float copyCount[4]{static_cast<float>(wordCount), 0.0f, 0.0f, 0.0f};
        encoder.setUniform(m_copyCount, copyCount);
        source.SetCompute(&encoder, SOURCE_BINDING, bgfx::Access::Read);
        destination->SetCompute(&encoder, DESTINATION_BINDING, bgfx::Access::Write);
        frameBuffer.Compute(encoder, m_program, (wordCount + COMPUTE_WORKGROUP_SIZE - 1) / COMPUTE_WORKGROUP_SIZE, 1, 1);
        return destination;
    }
}
