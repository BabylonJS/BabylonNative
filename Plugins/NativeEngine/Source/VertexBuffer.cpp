#include "VertexBuffer.h"
#include "Babylon/Graphics/DeviceContext.h"
#include <cassert>

namespace
{
    template<typename T>
    std::vector<uint8_t> PromoteToFloats(const gsl::span<uint8_t> bytes, uint8_t numElements, uint32_t byteOffset, uint16_t byteStride, uint32_t numVertices)
    {
        assert(numElements > 0);
        const uint32_t maxNumVertices = static_cast<uint32_t>((bytes.size() - byteOffset - sizeof(T) * numElements) / byteStride) + 1;
        if (numVertices > maxNumVertices)
        {
            numVertices = maxNumVertices;
        }

        std::vector<uint8_t> destinationBytes(numVertices * numElements * sizeof(float));
        float* destinationFloats = reinterpret_cast<float*>(destinationBytes.data());
        for (size_t index = 0; index < numVertices; ++index)
        {
            const T* source = reinterpret_cast<const T*>(bytes.data() + byteOffset + index * byteStride);
            for (size_t element = 0; element < numElements; ++element)
            {
                *destinationFloats++ = static_cast<float>(*source++);
            }
        }

        return destinationBytes;
    }

    std::vector<uint8_t> PromoteToFloats(const gsl::span<uint8_t> bytes, bgfx::AttribType::Enum attribType, uint8_t numElements, uint32_t byteOffset, uint16_t byteStride, uint32_t numVertices = std::numeric_limits<uint32_t>::max())
    {
        switch (attribType)
        {
            case bgfx::AttribType::Int8:
                return PromoteToFloats<int8_t>(bytes, numElements, byteOffset, byteStride, numVertices);
            case bgfx::AttribType::Uint8:
                return PromoteToFloats<uint8_t>(bytes, numElements, byteOffset, byteStride, numVertices);
            case bgfx::AttribType::Int16:
                return PromoteToFloats<int16_t>(bytes, numElements, byteOffset, byteStride, numVertices);
            case bgfx::AttribType::Uint16:
                return PromoteToFloats<uint16_t>(bytes, numElements, byteOffset, byteStride, numVertices);
            default:
                throw std::runtime_error{"Unable to promote vertex stream to a float array."};
        }
    }
}

namespace Babylon
{
    VertexBuffer::VertexBuffer(Graphics::DeviceContext& deviceContext, const gsl::span<uint8_t> bytes, bool dynamic)
        : m_deviceContext{deviceContext}
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

        m_streams.clear();
        m_attributes.clear();
        m_handle.reset();
        m_bytes.clear();

        m_disposed = true;
    }

    void VertexBuffer::Update(const gsl::span<uint8_t> bytes, size_t byteOffset)
    {
        if (!m_dynamic)
        {
            throw std::runtime_error{"Cannot update non-dynamic vertex buffer"};
        }

        if (m_handle.has_value())
        {
            const uint32_t startVertex = static_cast<uint32_t>(byteOffset / m_byteStride);

            if (byteOffset % m_byteStride != 0)
            {
                // bgfx only supports vertex start index and not arbitrary byte offsets.
                throw std::runtime_error{"Cannot update dynamic vertex buffer with a byte offset not divisible by its byte stride"};
            }

            for (auto& stream : m_streams)
            {
                if (stream.PromoteToFloatsHandle.has_value())
                {
                    const AttributeInfo& attribute = stream.Attribute;

                    std::vector<uint8_t> promoteToFloatsBytes = PromoteToFloats(
                        bytes,
                        attribute.AttribType,
                        attribute.NumElements,
                        attribute.ByteOffset,
                        m_byteStride);

                    stream.PromoteToFloatsHandle->Update(promoteToFloatsBytes, startVertex);
                }
                else
                {
                    m_handle->Update(bytes, startVertex);
                }
            }
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

    void VertexBuffer::Add(bgfx::Attrib::Enum attrib, bgfx::AttribType::Enum attribType, uint32_t byteOffset, uint16_t byteStride, uint8_t numElements, bool normalized)
    {
        if (m_byteStride == 0)
        {
            m_byteStride = byteStride;
        }
        else if (byteStride != m_byteStride)
        {
            throw std::runtime_error{"All attributes added to a vertex buffer must have the same byte stride"};
        }

        m_attributes[attrib] = {attribType, byteOffset, numElements, normalized};
    }

    void VertexBuffer::Set(bgfx::Encoder* encoder, uint8_t& streamCount, uint32_t startVertex, uint32_t numVertices)
    {
        if (!m_buildCalled)
        {
            m_buildCalled = true;
            Build(numVertices);
        }

        for (auto& stream : m_streams)
        {
            if (stream.PromoteToFloatsHandle.has_value())
            {
                stream.PromoteToFloatsHandle->Set(encoder, streamCount++, stream.StartVertexOffset + startVertex, numVertices, stream.LayoutHandle);
            }
            else
            {
                m_handle->Set(encoder, streamCount++, stream.StartVertexOffset + startVertex, numVertices, stream.LayoutHandle);
            }
        }
    }

    void VertexBuffer::Build(uint32_t numVertices)
    {
        // WebGL 1 and thus Babylon.js expects non-normalized integer attributes to automatically convert to a float if
        // the shader expects a float. This does not happen automatically for DirectX and Vulkan. To support this, this
        // function is promoting the attribute to floats by creating a new vertex buffer with its own handle and vertex
        // layout, otherwise known as a stream. This class must keep track of these new streams so that they can be set
        // and updated later.

        for (const auto& pair : m_attributes)
        {
            const bgfx::Attrib::Enum attrib = pair.first;
            const AttributeInfo& info = pair.second;

            // clang-format off
            const bool promoteToFloats = !info.Normalized
                && (bgfx::getCaps()->rendererType == bgfx::RendererType::Direct3D11 ||
                    bgfx::getCaps()->rendererType == bgfx::RendererType::Direct3D12 ||
                    bgfx::getCaps()->rendererType == bgfx::RendererType::Vulkan)
                && (info.AttribType == bgfx::AttribType::Int8 ||
                    info.AttribType == bgfx::AttribType::Uint8 ||
                    info.AttribType == bgfx::AttribType::Uint10 ||
                    info.AttribType == bgfx::AttribType::Int16 ||
                    info.AttribType == bgfx::AttribType::Uint16);
            // clang-format on

            if (promoteToFloats)
            {
                std::vector<uint8_t> bytes = PromoteToFloats(m_bytes, info.AttribType, info.NumElements, info.ByteOffset, m_byteStride, numVertices);

                bgfx::VertexLayout layout;
                layout.begin().add(attrib, info.NumElements, bgfx::AttribType::Float).end();

                m_streams.push_back({info,
                    std::optional<Handle>{std::in_place, m_deviceContext, std::move(bytes), m_dynamic, layout.getStride()},
                    0,
                    bgfx::createVertexLayout(layout)});
            }
            else
            {
                bgfx::VertexLayout layout;
                layout.begin();
                layout.add(attrib, info.NumElements, info.AttribType, info.Normalized);
                layout.m_offset[attrib] = static_cast<uint16_t>(info.ByteOffset % m_byteStride);
                layout.m_stride = m_byteStride;
                layout.end();

                m_streams.push_back({info,
                    std::nullopt,
                    info.ByteOffset / m_byteStride,
                    bgfx::createVertexLayout(layout)});
            }
        }

        m_handle.emplace(m_deviceContext, std::move(m_bytes), m_dynamic, m_byteStride);
    }

    void VertexBuffer::BuildInstanceDataBuffer(bgfx::InstanceDataBuffer& instanceDataBuffer, const std::map<bgfx::Attrib::Enum, InstanceInfo>& instances, uint32_t instanceCount)
    {
        uint16_t instanceStride{};
        for (auto& pair : instances)
        {
            if (instanceCount == 0)
            {
                const auto* vertexBuffer{pair.second.Buffer};
                instanceCount = static_cast<uint32_t>(vertexBuffer->m_bytes.size()) / pair.second.Stride;
            }

            instanceStride += static_cast<uint16_t>(pair.second.ElementSize);
        }

        // Create instance datas. Instance Data Buffer is transient.
        bgfx::allocInstanceDataBuffer(&instanceDataBuffer, instanceCount, instanceStride);

        // Copy instance data.
        uint8_t* data{instanceDataBuffer.data};
        uint32_t offset{};

        // Reverse because bgfx is also reversed: https://github.com/bkaradzic/bgfx/blob/4581f14cd481bad1e0d6292f0dd0a6e298c2ee18/src/renderer_d3d11.cpp#L2701
#if D3D11 || D3D12
        for (auto iter = instances.rbegin(); iter != instances.rend(); ++iter)
#else
        for (auto iter = instances.cbegin(); iter != instances.cend(); ++iter)
#endif
        {
            const auto& element{iter->second};
            const auto* source{element.Buffer->m_bytes.data()};
            for (uint32_t instance = 0; instance < instanceCount; instance++)
            {
                std::memcpy(data + instance * instanceStride + offset, source + instance * element.Stride + element.Offset, element.ElementSize);
            }
            offset += iter->second.ElementSize;
        }
    }

    VertexBuffer::Handle::Handle(Graphics::DeviceContext& deviceContext, std::vector<uint8_t> bytes, bool dynamic, uint16_t byteStride)
        : m_deviceContext{deviceContext}
        , m_deviceId{deviceContext.GetDeviceId()}
        , m_dynamic{dynamic}
    {
        auto releaseFn = [](void*, void* userData) {
            delete reinterpret_cast<decltype(bytes)*>(userData);
        };

        auto* bytesPtr = new decltype(bytes){std::move(bytes)};
        const bgfx::Memory* memory = bgfx::makeRef(bytesPtr->data(), static_cast<uint32_t>(bytesPtr->size()), releaseFn, bytesPtr);

        bgfx::VertexLayout layout;
        layout.begin();
        layout.m_stride = byteStride;
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

    VertexBuffer::Handle::~Handle()
    {
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
    }

    VertexBuffer::Handle::Handle(Handle&& other) noexcept
        : m_deviceContext{other.m_deviceContext}
        , m_dynamic{other.m_dynamic}
        , m_handle{other.m_handle}
    {
        other.m_handle = BGFX_INVALID_HANDLE;
    }

    VertexBuffer::Handle& VertexBuffer::Handle::operator=(Handle&& other) noexcept
    {
        this->~Handle();

        m_dynamic = other.m_dynamic;

        m_handle = other.m_handle;
        other.m_handle = BGFX_INVALID_HANDLE;

        return *this;
    }

    void VertexBuffer::Handle::Update(const gsl::span<uint8_t> bytes, uint32_t startVertex)
    {
        assert(m_dynamic);

        if (bgfx::isValid(m_handle))
        {
            bgfx::update(m_dynamicHandle, startVertex, bgfx::copy(bytes.data(), static_cast<uint32_t>(bytes.size())));
        }
    }

    void VertexBuffer::Handle::Set(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layoutHandle)
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
