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

    void PromoteToFloats(std::vector<uint8_t>& bytes, bgfx::AttribType::Enum attribType, uint32_t numElements, uint32_t byteOffset, uint32_t byteStride)
    {
        switch (attribType)
        {
            case bgfx::AttribType::Int8:
            {
                PromoteToFloats<int8_t>(bytes, numElements, byteOffset, byteStride);
                break;
            }
            case bgfx::AttribType::Uint8:
            {
                PromoteToFloats<uint8_t>(bytes, numElements, byteOffset, byteStride);
                break;
            }
            case bgfx::AttribType::Int16:
            {
                PromoteToFloats<int16_t>(bytes, numElements, byteOffset, byteStride);
                break;
            }
            case bgfx::AttribType::Uint16:
            {
                PromoteToFloats<uint16_t>(bytes, numElements, byteOffset, byteStride);
                break;
            }
            case bgfx::AttribType::Uint10: // is supported by any format ?
            default:
            {
                throw std::runtime_error("Unable to promote vertex stream to a float array.");
            }
        }
    }
}

namespace Babylon
{
    VertexBuffer::VertexBuffer(gsl::span<uint8_t> bytes, bool dynamic)
        : m_bytes{bytes.data(), bytes.data() + bytes.size()}
        , m_dynamic{dynamic}
    {
    }

    VertexBuffer::~VertexBuffer()
    {
        if (bgfx::isValid(m_layoutHandle))
        {
            bgfx::destroy(m_layoutHandle);
        }

        if (m_dynamic)
        {
            if (bgfx::isValid(m_dynamicHandle))
            {
                bgfx::destroy(m_dynamicHandle);
            }
        }
        else
        {
            if (bgfx::isValid(m_handle))
            {
                bgfx::destroy(m_handle);
            }
        }
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

    void VertexBuffer::Record(uint32_t location, uint32_t byteOffset, uint32_t byteStride, uint32_t numElements, uint32_t type, bool normalized)
    {
        m_attrib = static_cast<bgfx::Attrib::Enum>(location);
        m_vertexOffset = byteOffset / byteStride;

        bgfx::AttribType::Enum attribType{static_cast<bgfx::AttribType::Enum>(type)};

        bgfx::VertexLayout layout{};

        layout.begin();

        // clang-format off
        const bool promoteToFloats = !normalized
            && (bgfx::getCaps()->rendererType == bgfx::RendererType::Direct3D11 ||
                bgfx::getCaps()->rendererType == bgfx::RendererType::Direct3D12 ||
                bgfx::getCaps()->rendererType == bgfx::RendererType::Vulkan)
            && (attribType == bgfx::AttribType::Int8 ||
                attribType == bgfx::AttribType::Uint8 ||
                attribType == bgfx::AttribType::Uint10 ||
                attribType == bgfx::AttribType::Int16 ||
                attribType == bgfx::AttribType::Uint16);
        // clang-format on

        if (promoteToFloats)
        {
            layout.add(m_attrib, static_cast<uint8_t>(numElements), bgfx::AttribType::Float);
            layout.m_stride = static_cast<uint16_t>(sizeof(float) * numElements);
            PromoteToFloats(m_bytes, attribType, numElements, byteOffset, byteStride);
        }
        else
        {
            layout.add(m_attrib, static_cast<uint8_t>(numElements), attribType, normalized);
            layout.m_stride = static_cast<uint16_t>(byteStride);
            layout.m_offset[m_attrib] = static_cast<uint16_t>(byteOffset % byteStride);
        }

        layout.end();

        auto releaseFn = [](void*, void* userData)
        {
            auto* bytes = reinterpret_cast<std::vector<uint8_t>*>(userData);
            bytes->clear();
        };

        const bgfx::Memory* memory = bgfx::makeRef(m_bytes.data(), static_cast<uint32_t>(m_bytes.size()), releaseFn, &m_bytes);

        if (m_dynamic)
        {
            assert(!bgfx::isValid(m_dynamicHandle));
            m_dynamicHandle = bgfx::createDynamicVertexBuffer(memory, layout);
        }
        else
        {
            assert(!bgfx::isValid(m_handle));
            m_handle = bgfx::createVertexBuffer(memory, layout);
        };

        m_layoutHandle = bgfx::createVertexLayout(layout);
    }

    void VertexBuffer::Set(bgfx::Encoder* encoder, uint32_t vertexStart, uint32_t numVertices)
    {
        uint8_t stream{static_cast<uint8_t>(m_attrib)};

        if (m_dynamic)
        {
            encoder->setVertexBuffer(stream, m_dynamicHandle, m_vertexOffset + vertexStart, numVertices, m_layoutHandle);
        }
        else
        {
            encoder->setVertexBuffer(stream, m_handle, m_vertexOffset + vertexStart, numVertices, m_layoutHandle);
        }
    }
}
