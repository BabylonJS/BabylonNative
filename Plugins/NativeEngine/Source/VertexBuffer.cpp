#include "VertexBuffer.h"
#include "StorageBuffer.h"
#include "Babylon/Graphics/DeviceContext.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>

namespace Babylon
{
    namespace
    {
        // bgfx expects that each instance attribute occupies exactly one 16-byte slot.
        constexpr uint16_t INSTANCE_SLOT_SIZE = 16;

        // Allocates zeroed instance data. Zeroing makes any unused bytes within a slot (when ElementSize < 16),
        // and any slot the draw supplied no data for at all, read as zero in the shader.
        const bgfx::Memory* AllocInstanceData(uint32_t instanceCount, uint32_t slotCount, uint16_t& instanceStride)
        {
            instanceStride = static_cast<uint16_t>(slotCount * INSTANCE_SLOT_SIZE);
            if (instanceStride == 0)
            {
                throw std::runtime_error{"Instance data layout has no slots"};
            }
            if (instanceCount > UINT32_MAX / instanceStride)
            {
                throw std::runtime_error{"Instance count is too large"};
            }

            const bgfx::Memory* memory{bgfx::alloc(static_cast<uint32_t>(instanceStride) * instanceCount)};
            std::memset(memory->data, 0, memory->size);
            return memory;
        }

        // Smallest pool buffer, in instances, so small draws do not cause a series of tiny reallocations.
        constexpr uint32_t MIN_INSTANCE_POOL_CAPACITY = 1024;
    }

    InstanceDataBufferPool::InstanceDataBufferPool(Graphics::DeviceContext& deviceContext)
        : m_deviceContext{deviceContext}
        , m_deviceId{deviceContext.GetDeviceId()}
        , m_generation{deviceContext.ViewIdGeneration()}
    {
    }

    InstanceDataBufferPool::~InstanceDataBufferPool()
    {
        if (m_deviceId != m_deviceContext.GetDeviceId())
        {
            return;
        }

        for (const auto& [stride, buffer] : m_buffers)
        {
            bgfx::destroy(buffer.Handle);
        }
    }

    void InstanceDataBufferPool::Set(bgfx::Encoder* encoder, const bgfx::Memory* memory, uint16_t instanceStride, uint32_t instanceCount)
    {
        if (m_deviceId != m_deviceContext.GetDeviceId())
        {
            // The handles belonged to the previous device, which already released them.
            m_buffers.clear();
            m_deviceId = m_deviceContext.GetDeviceId();
        }

        // Every bgfx::frame() (including mid-frame flushes) bumps the generation. All updates submitted before
        // it have then been applied ahead of the draws that read them, so the ranges can be handed out again.
        const uint32_t generation = m_deviceContext.ViewIdGeneration();
        if (generation != m_generation)
        {
            m_generation = generation;
            for (auto& [stride, buffer] : m_buffers)
            {
                buffer.Used = 0;
            }
        }

        Buffer& buffer = m_buffers[instanceStride];
        if (instanceCount > buffer.Capacity - buffer.Used)
        {
            // Draws earlier in this frame still read the old buffer; bgfx defers its destruction until after
            // the frame has been rendered.
            const uint64_t capacity = std::max<uint64_t>({instanceCount, uint64_t{buffer.Capacity} * 2, MIN_INSTANCE_POOL_CAPACITY});
            if (capacity * instanceStride > UINT32_MAX)
            {
                throw std::runtime_error{"Instance data is too large"};
            }

            bgfx::VertexLayout instanceLayout{};
            // skip() takes a uint8_t, which would truncate the 256-byte stride of a full 16-slot layout.
            instanceLayout.begin();
            instanceLayout.m_stride = instanceStride;
            instanceLayout.end();
            const bgfx::DynamicVertexBufferHandle handle = bgfx::createDynamicVertexBuffer(static_cast<uint32_t>(capacity), instanceLayout);
            if (!bgfx::isValid(handle))
            {
                throw std::runtime_error{"Failed to create instance data vertex buffer"};
            }

            if (bgfx::isValid(buffer.Handle))
            {
                bgfx::destroy(buffer.Handle);
            }
            buffer = {handle, static_cast<uint32_t>(capacity), 0};
        }

        bgfx::update(buffer.Handle, buffer.Used, memory);
        encoder->setInstanceDataBuffer(buffer.Handle, buffer.Used, instanceCount);
        buffer.Used += instanceCount;
    }

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

        if (m_gpuStorage)
        {
            m_gpuStorage->Dispose();
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
        if (m_gpuStorage)
        {
            if (byteOffset > UINT32_MAX)
            {
                throw std::runtime_error{"Cannot update vertex buffer due to buffer overflow"};
            }
            m_gpuStorage->Update(bytes, static_cast<uint32_t>(byteOffset));
            return;
        }

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
            return;
        }

        if (byteOffset + bytes.size() > m_bytes.size())
        {
            throw std::runtime_error{"Cannot update vertex buffer due to buffer overflow"};
        }

        std::memcpy(m_bytes.data() + byteOffset, bytes.data(), bytes.size());
    }

    void VertexBuffer::Build(uint32_t byteStride)
    {
        if (m_disposed || m_deviceId != m_deviceContext.GetDeviceId())
        {
            throw std::runtime_error{"Cannot build a disposed or stale vertex buffer"};
        }

        if (m_byteStride == 0)
        {
            m_byteStride = byteStride;
        }
        else if (m_byteStride != byteStride)
        {
            throw std::runtime_error{"Attributes of a vertex buffer must have the same byte stride"};
        }

        if (!m_gpuStorage && !bgfx::isValid(m_handle))
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

            // As in WebGL, where any vertex buffer can feed transform feedback, keep built buffers readable
            // by the compute shaders that emulate it. Raw views address whole 32-bit words.
            m_computeReadable = (bgfx::getCaps()->supported & BGFX_CAPS_COMPUTE) != 0 && memory->size % sizeof(uint32_t) == 0;
            const uint16_t flags = m_computeReadable ? BGFX_BUFFER_COMPUTE_READ : BGFX_BUFFER_NONE;

            if (m_dynamic)
            {
                m_dynamicHandle = bgfx::createDynamicVertexBuffer(memory, layout, flags);
            }
            else
            {
                m_handle = bgfx::createVertexBuffer(memory, layout, flags);
            }

            if (!bgfx::isValid(m_handle))
            {
                throw std::runtime_error{"Failed to create vertex buffer"};
            }
        }
    }

    void VertexBuffer::Set(bgfx::Encoder* encoder, uint8_t stream, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layout)
    {
        if (m_gpuStorage)
        {
            m_gpuStorage->SetVertex(encoder, stream, startVertex, numVertices, layout);
        }
        else if (m_dynamic)
        {
            encoder->setVertexBuffer(stream, m_dynamicHandle, startVertex, numVertices, layout);
        }
        else
        {
            encoder->setVertexBuffer(stream, m_handle, startVertex, numVertices, layout);
        }
    }

    StorageBuffer& VertexBuffer::PromoteToGpuStorage(uint32_t vertexByteStride)
    {
        if (m_gpuStorage)
        {
            return *m_gpuStorage;
        }
        if (m_disposed || m_deviceId != m_deviceContext.GetDeviceId())
        {
            throw std::runtime_error{"Cannot use a disposed or stale vertex buffer for transform feedback"};
        }
        if (bgfx::isValid(m_handle))
        {
            // Build hands the CPU bytes to bgfx, so they can no longer seed the storage buffer.
            throw std::runtime_error{"Transform feedback cannot use a vertex buffer already bound as a per-vertex stream"};
        }
        if (m_bytes.empty() || m_bytes.size() > UINT32_MAX)
        {
            throw std::runtime_error{"Transform feedback vertex buffers must not be empty"};
        }

        // The storage pads byte- and short-component data to whole words.
        constexpr uint32_t kWordSize = sizeof(uint32_t);
        const bool strideUsable = vertexByteStride != 0 && vertexByteStride % kWordSize == 0 && vertexByteStride <= UINT16_MAX;
        const uint32_t storageStride = strideUsable ? vertexByteStride : kWordSize;
        auto storage = std::make_unique<StorageBuffer>(m_deviceContext, static_cast<uint32_t>(m_bytes.size()), storageStride);
        storage->Update(m_bytes, 0);
        m_gpuStorage = std::move(storage);
        m_bytes.clear();
        m_bytes.shrink_to_fit();
        return *m_gpuStorage;
    }

    void VertexBuffer::ReplaceGpuStorage(std::unique_ptr<StorageBuffer> storage)
    {
        if (m_gpuStorage)
        {
            m_gpuStorage->Dispose();
        }
        m_gpuStorage = std::move(storage);
    }

    uint32_t VertexBuffer::ByteLength() const
    {
        return m_gpuStorage ? m_gpuStorage->ByteLength() : static_cast<uint32_t>(m_bytes.size());
    }

    void VertexBuffer::SetComputeRead(bgfx::Encoder* encoder, uint8_t stage, uint32_t vertexByteStride)
    {
        if (m_gpuStorage || !bgfx::isValid(m_handle))
        {
            PromoteToGpuStorage(vertexByteStride).SetCompute(encoder, stage, bgfx::Access::Read);
        }
        else if (!m_computeReadable)
        {
            throw std::runtime_error{"Transform feedback cannot read this vertex buffer on this renderer"};
        }
        else if (m_dynamic)
        {
            encoder->setBuffer(stage, m_dynamicHandle, bgfx::Access::Read);
        }
        else
        {
            encoder->setBuffer(stage, m_handle, bgfx::Access::Read);
        }
    }

    StorageBuffer* VertexBuffer::InstanceInfo::ResolveStorage() const
    {
        return Buffer != nullptr ? Buffer->GpuStorage() : nullptr;
    }

    VertexBuffer::InstanceDataLayout VertexBuffer::CreateInstanceDataLayout(
        const std::map<uint32_t, InstanceInfo>& instances,
        const std::map<uint32_t, uint32_t>& builtInSlots,
        uint32_t maxSlotCount)
    {
        if (builtInSlots.size() > maxSlotCount)
        {
            throw std::runtime_error{"Shader declares more built-in instance-data slots than the device supports."};
        }

        if (instances.empty())
        {
            // No recorded instance attributes, but the shader may still declare built-in per-instance
            // inputs (see VertexArray::SetVertexBuffers), which then need zeroed slots.
            InstanceDataLayout layout{};
            layout.SlotCount = static_cast<uint32_t>(builtInSlots.size());
            return layout;
        }

        std::vector<bool> usedBuiltInSlots(builtInSlots.size());
        InstanceDataLayout layout{};
        for (const auto& [attrib, slot] : builtInSlots)
        {
            if (slot >= usedBuiltInSlots.size() || usedBuiltInSlots[slot])
            {
                throw std::runtime_error{"Shader has an invalid built-in instance-data layout."};
            }
            usedBuiltInSlots[slot] = true;
            if (instances.count(attrib) != 0)
            {
                layout.Slots.emplace(attrib, slot);
            }
        }

        uint32_t nextSlot = static_cast<uint32_t>(builtInSlots.size());
        for (auto instance = instances.rbegin(); instance != instances.rend(); ++instance)
        {
            if (builtInSlots.count(instance->first) != 0)
            {
                continue;
            }
            if (nextSlot >= maxSlotCount)
            {
                throw std::runtime_error{"Number of vertex buffer instance slots greater than " + std::to_string(maxSlotCount) + " is not supported"};
            }
            layout.Slots.emplace(instance->first, nextSlot++);
        }
        layout.SlotCount = nextSlot;
        return layout;
    }

    void VertexBuffer::SetInstanceDataBuffer(
        bgfx::Encoder* encoder,
        InstanceDataBufferPool& pool,
        const std::map<uint32_t, InstanceInfo>& instances,
        uint32_t instanceCount,
        const InstanceDataLayout& layout)
    {
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

        const size_t slotCount = layout.SlotCount;
        const uint32_t maxInstanceData = bgfx::getCaps()->limits.maxInstanceData;
        if (slotCount > maxInstanceData)
        {
            throw std::runtime_error{"Number of vertex buffer instance slots greater than " + std::to_string(maxInstanceData) + " is not supported"};
        }
        if (layout.Slots.size() != instances.size())
        {
            throw std::runtime_error{"Instance-data layout does not cover every recorded attribute."};
        }

        // Validate before allocating so a rejected layout does not leak the allocation.
        for (const auto& [attrib, element] : instances)
        {
            if (layout.Slots.at(attrib) >= layout.SlotCount)
            {
                throw std::runtime_error{"Instance-data attribute maps outside the allocated buffer."};
            }
        }

        uint16_t instanceStride{};
        const bgfx::Memory* memory{AllocInstanceData(instanceCount, layout.SlotCount, instanceStride)};
        uint8_t* data{memory->data};

        for (const auto& [attrib, element] : instances)
        {
            const uint32_t slot = layout.Slots.at(attrib);
            const uint32_t slotOffset = slot * INSTANCE_SLOT_SIZE;
            assert(element.ElementSize <= INSTANCE_SLOT_SIZE);
            const auto* source{element.Buffer->m_bytes.data()};
            for (uint32_t instance = 0; instance < instanceCount; instance++)
            {
                std::memcpy(data + instance * instanceStride + slotOffset, source + instance * element.Stride + element.Offset, element.ElementSize);
            }
        }

        pool.Set(encoder, memory, instanceStride, instanceCount);
    }

    void VertexBuffer::SetZeroInstanceDataBuffer(bgfx::Encoder* encoder, InstanceDataBufferPool& pool, uint32_t instanceCount, uint32_t slotCount)
    {
        uint16_t instanceStride{};
        const bgfx::Memory* memory{AllocInstanceData(instanceCount, slotCount, instanceStride)};
        pool.Set(encoder, memory, instanceStride, instanceCount);
    }
}
