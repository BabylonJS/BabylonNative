#include "IndexBuffer.h"
#include "Babylon/Graphics/DeviceContext.h"

namespace Babylon
{
    IndexBuffer::IndexBuffer(Graphics::DeviceContext& deviceContext, const gsl::span<uint8_t> bytes, uint16_t flags, bool dynamic)
        : m_deviceContext{deviceContext}
        , m_deviceID{deviceContext.GetDeviceId()}
        , m_bytes{bytes.data(), bytes.data() + bytes.size()}
        , m_flags{flags}
        , m_dynamic{dynamic}
    {
    }

    IndexBuffer::~IndexBuffer()
    {
        Dispose();
    }

    void IndexBuffer::Dispose()
    {
        if (m_disposed)
        {
            return;
        }

        if (bgfx::isValid(m_handle) && m_deviceID == m_deviceContext.GetDeviceId())
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

    void IndexBuffer::Update(const gsl::span<uint8_t> bytes, uint32_t startIndex)
    {
        if (!m_dynamic)
        {
            throw std::runtime_error{"Cannot update non-dynamic index buffer"};
        }

        if (bgfx::isValid(m_dynamicHandle))
        {
            bgfx::update(m_dynamicHandle, startIndex, bgfx::copy(bytes.data(), static_cast<uint32_t>(bytes.size())));
        }
        else
        {
            const size_t byteStride = (m_flags & BGFX_BUFFER_INDEX32) ? 4 : 2;
            const size_t byteOffset = startIndex * byteStride;

            if (byteOffset + bytes.size() > m_bytes.size())
            {
                throw std::runtime_error{"Failed to update index buffer: buffer overflow"};
            }

            std::memcpy(m_bytes.data() + byteOffset, bytes.data(), bytes.size());
        }
    }

    void IndexBuffer::Set(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices)
    {
        if (!m_buildCalled)
        {
            m_buildCalled = true;
            Build();
        }

        if (m_dynamic)
        {
            encoder->setIndexBuffer(m_dynamicHandle, firstIndex, numIndices);
        }
        else
        {
            encoder->setIndexBuffer(m_handle, firstIndex, numIndices);
        }
    }

    void IndexBuffer::Build()
    {
        auto releaseFn = [](void*, void* userData) {
            delete reinterpret_cast<decltype(m_bytes)*>(userData);
        };

        auto* bytesPtr = new decltype(m_bytes){std::move(m_bytes)};
        const bgfx::Memory* memory = bgfx::makeRef(bytesPtr->data(), static_cast<uint32_t>(bytesPtr->size()), releaseFn, bytesPtr);

        if (m_dynamic)
        {
            m_dynamicHandle = bgfx::createDynamicIndexBuffer(memory, m_flags);
        }
        else
        {
            m_handle = bgfx::createIndexBuffer(memory, m_flags);
        }

        if (!bgfx::isValid(m_handle))
        {
            throw std::runtime_error{"Failed to create index buffer"};
        }
    }
}
