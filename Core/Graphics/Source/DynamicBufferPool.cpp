#include <Babylon/Graphics/DynamicBufferPool.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace Babylon::Graphics
{
    namespace
    {
        // Smallest buffer, in bytes, so small allocations do not cause a series of tiny reallocations.
        constexpr uint64_t MIN_CAPACITY_BYTES = 64 * 1024;

        constexpr uint16_t INDEX_SIZE = sizeof(uint16_t);

        // Makes sure the buffer has room for count more elements of elementSize bytes, replacing it with a larger one
        // if needed. Draws earlier in this frame still read the old buffer; bgfx defers its destruction until after
        // the frame has been rendered.
        template<typename BufferT, typename CreateT>
        void Reserve(BufferT& buffer, uint32_t count, uint16_t elementSize, CreateT&& create)
        {
            if (count <= buffer.Capacity - buffer.Used)
            {
                return;
            }

            const uint64_t capacity = std::max<uint64_t>({count, uint64_t{buffer.Capacity} * 2, MIN_CAPACITY_BYTES / elementSize});
            if (capacity * elementSize > UINT32_MAX)
            {
                throw std::runtime_error{"Dynamic buffer data is too large"};
            }

            const auto handle = create(static_cast<uint32_t>(capacity));
            if (!bgfx::isValid(handle))
            {
                throw std::runtime_error{"Failed to create a dynamic buffer"};
            }

            if (bgfx::isValid(buffer.Handle))
            {
                bgfx::destroy(buffer.Handle);
            }
            buffer = {handle, static_cast<uint32_t>(capacity), 0};
        }
    }

    DynamicBufferPool::DynamicBufferPool(DeviceContext& deviceContext, CreationFunctions creationFunctions)
        : m_deviceContext{deviceContext}
        , m_deviceId{deviceContext.GetDeviceId()}
        , m_generation{deviceContext.ViewIdGeneration()}
        , m_creationFunctions{std::move(creationFunctions)}
    {
        if (!m_creationFunctions.CreateVertexBuffer)
        {
            m_creationFunctions.CreateVertexBuffer = [](uint32_t capacity, const bgfx::VertexLayout& layout) {
                return bgfx::createDynamicVertexBuffer(capacity, layout);
            };
        }
        if (!m_creationFunctions.CreateIndexBuffer)
        {
            m_creationFunctions.CreateIndexBuffer = [](uint32_t capacity) {
                return bgfx::createDynamicIndexBuffer(capacity);
            };
        }
    }

    DynamicBufferPool::~DynamicBufferPool()
    {
        if (m_deviceId != m_deviceContext.GetDeviceId())
        {
            return;
        }

        for (const auto& [key, buffer] : m_vertexBuffers)
        {
            if (bgfx::isValid(buffer.Handle))
            {
                bgfx::destroy(buffer.Handle);
            }
        }
        if (bgfx::isValid(m_indexBuffer.Handle))
        {
            bgfx::destroy(m_indexBuffer.Handle);
        }
    }

    DynamicBufferPool::VertexRange DynamicBufferPool::AllocVertices(const bgfx::VertexLayout& layout, const void* data, uint32_t size)
    {
        const uint16_t stride = layout.getStride();
        if (stride == 0 || size % stride != 0)
        {
            throw std::runtime_error{"Vertex data does not match the vertex layout"};
        }

        BeginAllocation();

        const uint32_t count = size / stride;
        const uint64_t key = (uint64_t{layout.m_hash} << 16) | stride;
        auto [it, inserted] = m_vertexBuffers.try_emplace(key);
        try
        {
            Reserve(it->second, count, stride, [this, &layout](uint32_t capacity) {
                return m_creationFunctions.CreateVertexBuffer(capacity, layout);
            });
        }
        catch (...)
        {
            if (inserted)
            {
                m_vertexBuffers.erase(it);
            }
            throw;
        }

        auto& buffer = it->second;
        const VertexRange range{buffer.Handle, buffer.Used};
        bgfx::update(buffer.Handle, buffer.Used, bgfx::copy(data, size));
        buffer.Used += count;
        return range;
    }

    DynamicBufferPool::IndexRange DynamicBufferPool::AllocIndices(const void* data, uint32_t size)
    {
        if (size % INDEX_SIZE != 0)
        {
            throw std::runtime_error{"Index data must hold 16-bit indices"};
        }

        BeginAllocation();

        const uint32_t count = size / INDEX_SIZE;
        Reserve(m_indexBuffer, count, INDEX_SIZE, [this](uint32_t capacity) {
            return m_creationFunctions.CreateIndexBuffer(capacity);
        });

        const IndexRange range{m_indexBuffer.Handle, m_indexBuffer.Used};
        bgfx::update(m_indexBuffer.Handle, m_indexBuffer.Used, bgfx::copy(data, size));
        m_indexBuffer.Used += count;
        return range;
    }

    void DynamicBufferPool::BeginAllocation()
    {
        if (m_deviceId != m_deviceContext.GetDeviceId())
        {
            // The handles belonged to the previous device, which already released them.
            m_vertexBuffers.clear();
            m_indexBuffer = {};
            m_deviceId = m_deviceContext.GetDeviceId();
        }

        // Every bgfx::frame() (including mid-frame flushes) bumps the generation. All updates submitted before it
        // have then been applied ahead of the draws that read them, so the ranges can be handed out again.
        const uint32_t generation = m_deviceContext.ViewIdGeneration();
        if (generation != m_generation)
        {
            m_generation = generation;
            for (auto& [key, buffer] : m_vertexBuffers)
            {
                buffer.Used = 0;
            }
            m_indexBuffer.Used = 0;
        }
    }
}
