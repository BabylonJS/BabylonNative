#include "VertexBuffer.h"
#include "Babylon/Graphics/DeviceContext.h"
#include <cassert>

namespace Babylon
{
    VertexBuffer::VertexBuffer(Graphics::DeviceContext& deviceContext, gsl::span<const uint8_t> bytes, bool dynamic)
        : m_deviceContext{deviceContext}
        , m_deviceId{m_deviceContext.GetDeviceId()}
        , m_bytes{bytes.data(), bytes.data() + bytes.size()}
        , m_dynamic{dynamic}
    {
    }

    VertexBuffer::~VertexBuffer()
    {
        Dispose();
    }

    void VertexBuffer::Dispose()
    {
        if (m_disposed)
        {
            return;
        }

        if (bgfx::isValid(m_handle) && m_deviceId == m_deviceContext.GetDeviceId())
        {
            if (m_dynamic)
            {
                bgfx::destroy(m_dynamicHandle);
            }
            else
            {
                bgfx::destroy(m_handle);
            }
        }

        m_bytes.clear();

        m_disposed = true;
    }

    void VertexBuffer::Update(gsl::span<const uint8_t> bytes, size_t byteOffset)
    {
        if (!m_dynamic)
        {
            throw std::runtime_error{"Cannot update non-dynamic vertex buffer"};
        }

        if (bgfx::isValid(m_dynamicHandle))
        {
            const uint32_t startVertex = static_cast<uint32_t>(byteOffset / m_byteStride);

            if (byteOffset % m_byteStride != 0)
            {
                // bgfx only supports vertex start index and not arbitrary byte offsets.
                throw std::runtime_error{"Cannot update dynamic vertex buffer with a byte offset not divisible by its byte stride"};
            }

            bgfx::update(m_dynamicHandle, startVertex, bgfx::copy(bytes.data(), static_cast<uint32_t>(bytes.size())));
        }
        else
        {
            if (byteOffset + bytes.size() > m_bytes.size())
            {
                throw std::runtime_error{"Cannot update vertex buffer due to buffer overflow"};
            }

            std::memcpy(m_bytes.data() + byteOffset, bytes.data(), bytes.size());
        }
    }

    void VertexBuffer::Build(uint32_t byteStride)
    {
        if (m_byteStride == 0)
        {
            m_byteStride = byteStride;
        }
        else if (m_byteStride != byteStride)
        {
            throw std::runtime_error{"Attributes of a vertex buffer must have the same byte stride"};
        }

        if (!bgfx::isValid(m_handle))
        {
            auto releaseFn = [](void*, void* userData) {
                delete reinterpret_cast<decltype(m_bytes)*>(userData);
            };

            auto* bytesPtr = new decltype(m_bytes){std::move(m_bytes)};
            const bgfx::Memory* memory = bgfx::makeRef(bytesPtr->data(), static_cast<uint32_t>(bytesPtr->size()), releaseFn, bytesPtr);

            bgfx::VertexLayout layout;
            layout.begin();
            layout.m_stride = static_cast<uint16_t>(byteStride);
            layout.end();

            if (m_dynamic)
            {
                m_dynamicHandle = bgfx::createDynamicVertexBuffer(memory, layout);
            }
            else
            {
                m_handle = bgfx::createVertexBuffer(memory, layout);
            }

            if (!bgfx::isValid(m_handle))
            {
                throw std::runtime_error{"Failed to create vertex buffer"};
            }
        }
    }

    void VertexBuffer::Set(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layout)
    {
        if (m_dynamic)
        {
            encoder->setVertexBuffer(stream, m_dynamicHandle, startVertex, numVertices, layout);
        }
        else
        {
            encoder->setVertexBuffer(stream, m_handle, startVertex, numVertices, layout);
        }
    }

    void VertexBuffer::BuildInstanceDataBuffer(bgfx::InstanceDataBuffer& instanceDataBuffer, const std::map<bgfx::Attrib::Enum, InstanceInfo>& instances, uint32_t instanceCount)
    {
        // bgfx expects that each instance attribute occupies exactly one 16-byte slot.
        static constexpr uint16_t kSlotSize = 16;

        if (instances.empty())
        {
            return;
        }

        if (instanceCount == 0)
        {
            const auto& first{instances.begin()->second};
            instanceCount = static_cast<uint32_t>(first.Buffer->m_bytes.size()) / first.Stride;
        }

        if (instanceCount == 0)
        {
            return;
        }

        const uint16_t instanceStride = static_cast<uint16_t>(instances.size() * kSlotSize);

        // Create instance datas. Instance Data Buffer is transient.
        bgfx::allocInstanceDataBuffer(&instanceDataBuffer, instanceCount, instanceStride);

        uint8_t* data{instanceDataBuffer.data};

        // Zero the buffer so any unused bytes within a 16-byte slot (when ElementSize < 16) read as
        // zero in the shader instead of leaking transient ring-buffer garbage.
        std::memset(data, 0, static_cast<size_t>(instanceStride) * instanceCount);

        // Reverse because bgfx maps instance data in reverse attrib order:
        // i_data0 == the highest instance-data TEXCOORD semantic (TEXCOORD31 on D3D11),
        // i_data1 == TEXCOORD30, etc. OpenGL/Metal expect this layout too since bgfx
        // abstracts the mapping (binding by i_data name).
        uint32_t slotOffset{};
        for (auto iter = instances.rbegin(); iter != instances.rend(); ++iter)
        {
            const auto& element{iter->second};
            assert(element.ElementSize <= kSlotSize);
            const auto* source{element.Buffer->m_bytes.data()};
            for (uint32_t instance = 0; instance < instanceCount; instance++)
            {
                std::memcpy(data + instance * instanceStride + slotOffset, source + instance * element.Stride + element.Offset, element.ElementSize);
            }
            slotOffset += kSlotSize;
        }
    }
}
