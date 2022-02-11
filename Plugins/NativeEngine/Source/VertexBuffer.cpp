#include "VertexBuffer.h"
#include <cassert>

namespace
{
    template<typename sourceType> void PromoteToFloats(std::vector<uint8_t>& bytes, uint32_t numElements, uint32_t byteOffset, uint32_t byteStride)
    {
        const size_t count = bytes.size() / byteStride;
        const size_t destinationSize = count * numElements * sizeof(float);
        if (destinationSize != bytes.size()) // ensure both vectors have different size
        {
            std::vector<uint8_t> newBytes(destinationSize);
            float* destination = reinterpret_cast<float*>(newBytes.data());
            for (size_t i = 0; i < count; ++i)
            {
                sourceType* source = reinterpret_cast<sourceType*>(bytes.data() + byteOffset + byteStride * i);
                for (size_t element = 0; element < numElements; element++)
                {
                    *destination++ = static_cast<float>(*source++);
                }
            }
            bytes.swap(newBytes);
        }
    }
}

namespace Babylon
{
    VertexBuffer::VertexBuffer(gsl::span<uint8_t> bytes, bool dynamic)
        : m_bytes{{bytes.data(), bytes.data() + bytes.size()}}
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

        if (bgfx::isValid(m_handle))
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

        m_bytes.reset();
        m_disposed = true;
    }

    void VertexBuffer::Update(Napi::Env env, gsl::span<uint8_t> bytes)
    {
        if (!m_dynamic)
        {
            throw Napi::Error::New(env, "Cannot update non-dynamic vertex buffer.");
        }

        if (bgfx::isValid(m_dynamicHandle))
        {
            // Buffer was already created, do a real update operation.
            bgfx::update(m_dynamicHandle, 0, bgfx::copy(bytes.data(), static_cast<uint32_t>(bytes.size())));
        }
        else
        {
            // Buffer hasn't been finalized yet, all that's necessary is to swap out the bytes.
            m_bytes = {bytes.data(), bytes.data() + bytes.size()};
        }
    }

    bool VertexBuffer::CreateHandle(const bgfx::VertexLayout& layout)
    {
        if (bgfx::isValid(m_handle))
        {
            // NOTE: This code is assuming that layout stride hasn't changed.
            return true;
        }

        auto releaseFn = [](void*, void* userData)
        {
            auto* bytes = reinterpret_cast<decltype(m_bytes)*>(userData);
            bytes->reset();
        };

        const bgfx::Memory* memory = bgfx::makeRef(m_bytes->data(), static_cast<uint32_t>(m_bytes->size()), releaseFn, &m_bytes);

        if (m_dynamic)
        {
            m_dynamicHandle = bgfx::createDynamicVertexBuffer(memory, layout);
        }
        else
        {
            m_handle = bgfx::createVertexBuffer(memory, layout);
        };

        return bgfx::isValid(m_handle);
    }

    void VertexBuffer::PromoteToFloats(bgfx::AttribType::Enum attribType, uint32_t numElements, uint32_t byteOffset, uint32_t byteStride)
    {
        switch (attribType)
        {
            case bgfx::AttribType::Int8:
            {
                ::PromoteToFloats<int8_t>(*m_bytes, numElements, byteOffset, byteStride);
                break;
            }
            case bgfx::AttribType::Uint8:
            {
                ::PromoteToFloats<uint8_t>(*m_bytes, numElements, byteOffset, byteStride);
                break;
            }
            case bgfx::AttribType::Int16:
            {
                ::PromoteToFloats<int16_t>(*m_bytes, numElements, byteOffset, byteStride);
                break;
            }
            case bgfx::AttribType::Uint16:
            {
                ::PromoteToFloats<uint16_t>(*m_bytes, numElements, byteOffset, byteStride);
                break;
            }
            case bgfx::AttribType::Uint10: // is supported by any format ?
            default:
            {
                throw std::runtime_error("Unable to promote vertex stream to a float array.");
            }
        }
    }

    void VertexBuffer::Set(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layoutHandle)
    {
        if (bgfx::isValid(m_handle))
        {
            if (m_dynamic)
            {
                encoder->setVertexBuffer(stream, m_dynamicHandle, startVertex, numVertices, layoutHandle);
            }
            else
            {
                encoder->setVertexBuffer(stream, m_handle, startVertex, numVertices, layoutHandle);
            }
        }
    }
}
