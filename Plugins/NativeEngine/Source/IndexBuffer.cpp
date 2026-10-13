#include "IndexBuffer.h"
#include "Babylon/Graphics/DeviceContext.h"

#include <limits>
#include <tuple>

namespace Babylon
{
    IndexBuffer::IndexBuffer(Graphics::DeviceContext& deviceContext, gsl::span<const uint8_t> bytes, uint16_t flags, bool dynamic)
        : m_deviceContext{deviceContext}
        , m_deviceID{deviceContext.GetDeviceId()}
        , m_data{bytes, (flags & BGFX_BUFFER_INDEX32) != 0}
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

        DestroyExpandedBuffers();
        m_data.Clear();

        m_disposed = true;
    }

    void IndexBuffer::Update(gsl::span<const uint8_t> bytes, uint32_t startIndex)
    {
        if (!m_dynamic)
        {
            throw std::runtime_error{"Cannot update non-dynamic index buffer"};
        }

        if (m_disposed)
        {
            throw std::runtime_error{"Cannot update disposed index buffer"};
        }

        if (bytes.empty())
        {
            return;
        }

        const bool hasGpuBuffer = bgfx::isValid(m_dynamicHandle);
        if (hasGpuBuffer && bytes.size() > std::numeric_limits<uint32_t>::max())
        {
            throw std::length_error{"Index buffer update is too large"};
        }

        m_data.Update(bytes, startIndex);

        DestroyExpandedBuffers();

        if (hasGpuBuffer)
        {
            bgfx::update(m_dynamicHandle, startIndex, bgfx::copy(bytes.data(), static_cast<uint32_t>(bytes.size())));
        }
    }

    void IndexBuffer::Build()
    {
        if (m_disposed)
        {
            throw std::runtime_error{"Cannot build disposed index buffer"};
        }

        if (!bgfx::isValid(m_handle))
        {
            const auto bytes = m_data.Bytes();
            if (bytes.size() > std::numeric_limits<uint32_t>::max())
            {
                throw std::length_error{"Index buffer data is too large"};
            }
            const bgfx::Memory* memory = bgfx::copy(bytes.data(), static_cast<uint32_t>(bytes.size()));

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

    void IndexBuffer::Set(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices)
    {
        if (m_disposed)
        {
            throw std::runtime_error{"Cannot bind disposed index buffer"};
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

    bool IndexBuffer::SetExpanded(bgfx::Encoder* encoder, PrimitiveModeExpansion::Mode mode, uint32_t firstIndex, uint32_t numIndices)
    {
        if (m_disposed)
        {
            throw std::runtime_error{"Cannot bind disposed index buffer"};
        }

        const ExpansionKey key{mode, firstIndex, numIndices};
        auto existing = m_expandedBuffers.find(key);
        if (existing == m_expandedBuffers.end())
        {
            auto expanded = PrimitiveModeExpansion::ExpandIndexed(m_data, firstIndex, numIndices, mode);
            if (expanded.IndexCount == 0)
            {
                return false;
            }

            const uint16_t flags = expanded.Index32 ? BGFX_BUFFER_INDEX32 : 0;
            const bgfx::Memory* memory = bgfx::copy(expanded.Bytes.data(), static_cast<uint32_t>(expanded.Bytes.size()));
            const bgfx::IndexBufferHandle handle = bgfx::createIndexBuffer(memory, flags);
            if (!bgfx::isValid(handle))
            {
                throw std::runtime_error{"Failed to create expanded primitive index buffer"};
            }

            try
            {
                existing = m_expandedBuffers.emplace(key, ExpandedBuffer{handle, expanded.IndexCount}).first;
            }
            catch (...)
            {
                bgfx::destroy(handle);
                throw;
            }
        }

        encoder->setIndexBuffer(existing->second.Handle, 0, existing->second.IndexCount);
        return true;
    }

    bool IndexBuffer::ExpansionKey::operator<(const ExpansionKey& other) const
    {
        return std::tie(Mode, FirstIndex, IndexCount) < std::tie(other.Mode, other.FirstIndex, other.IndexCount);
    }

    void IndexBuffer::DestroyExpandedBuffers()
    {
        if (m_deviceID == m_deviceContext.GetDeviceId())
        {
            for (const auto& [key, buffer] : m_expandedBuffers)
            {
                static_cast<void>(key);
                if (bgfx::isValid(buffer.Handle))
                {
                    bgfx::destroy(buffer.Handle);
                }
            }
        }
        m_expandedBuffers.clear();
    }
}
