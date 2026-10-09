#include "PrimitiveModeExpansion.h"

#include <cstring>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace Babylon::PrimitiveModeExpansion
{
    namespace
    {
        template<typename T>
        T ReadIndex(gsl::span<const uint8_t> bytes, size_t index)
        {
            T value{};
            std::memcpy(&value, bytes.data() + index * sizeof(T), sizeof(T));
            return value;
        }

        uint32_t CheckedIndexCount(uint64_t count, size_t indexStride)
        {
            if (count > std::numeric_limits<uint32_t>::max() ||
                count > std::numeric_limits<uint32_t>::max() / indexStride ||
                count > std::numeric_limits<size_t>::max() / indexStride)
            {
                throw std::length_error{"Expanded primitive index data is too large"};
            }

            return static_cast<uint32_t>(count);
        }

        uint64_t PrimitiveIndexCount(uint64_t runLength, Mode mode)
        {
            switch (mode)
            {
                case Mode::LineLoop:
                    return runLength >= 2 ? runLength * 2 : 0;
                case Mode::TriangleFan:
                    return runLength >= 3 ? (runLength - 2) * 3 : 0;
            }

            throw std::invalid_argument{"Unsupported primitive expansion mode"};
        }

        template<typename T>
        Result ExpandIndexedTyped(const IndexData& data, uint32_t firstIndex, uint32_t indexCount, Mode mode)
        {
            const auto bytes = data.Bytes();
            if (bytes.size() % sizeof(T) != 0)
            {
                throw std::runtime_error{"Index buffer byte length is not aligned to its index type"};
            }

            const size_t totalIndexCount = bytes.size() / sizeof(T);
            if (firstIndex > totalIndexCount || indexCount > totalIndexCount - firstIndex)
            {
                throw std::out_of_range{"Primitive expansion index range exceeds the index buffer"};
            }

            constexpr T RestartIndex = std::numeric_limits<T>::max();
            uint64_t expandedCount{};
            uint64_t runLength{};
            for (uint64_t offset = 0; offset < indexCount; ++offset)
            {
                if (ReadIndex<T>(bytes, static_cast<size_t>(firstIndex) + static_cast<size_t>(offset)) == RestartIndex)
                {
                    expandedCount += PrimitiveIndexCount(runLength, mode);
                    runLength = 0;
                }
                else
                {
                    ++runLength;
                }
            }
            expandedCount += PrimitiveIndexCount(runLength, mode);

            Result result{};
            result.Index32 = std::is_same_v<T, uint32_t>;
            result.IndexCount = CheckedIndexCount(expandedCount, sizeof(T));

            std::vector<T> expanded{};
            expanded.reserve(result.IndexCount);

            const auto appendRun = [&](size_t runStart, size_t runEnd) {
                const size_t length = runEnd - runStart;
                if (mode == Mode::LineLoop)
                {
                    if (length < 2)
                    {
                        return;
                    }

                    for (size_t index = runStart; index + 1 < runEnd; ++index)
                    {
                        expanded.emplace_back(ReadIndex<T>(bytes, index));
                        expanded.emplace_back(ReadIndex<T>(bytes, index + 1));
                    }
                    expanded.emplace_back(ReadIndex<T>(bytes, runEnd - 1));
                    expanded.emplace_back(ReadIndex<T>(bytes, runStart));
                }
                else if (mode == Mode::TriangleFan)
                {
                    if (length < 3)
                    {
                        return;
                    }

                    const T root = ReadIndex<T>(bytes, runStart);
                    for (size_t index = runStart + 1; index + 1 < runEnd; ++index)
                    {
                        expanded.emplace_back(root);
                        expanded.emplace_back(ReadIndex<T>(bytes, index));
                        expanded.emplace_back(ReadIndex<T>(bytes, index + 1));
                    }
                }
                else
                {
                    throw std::invalid_argument{"Unsupported primitive expansion mode"};
                }
            };

            size_t runStart = firstIndex;
            const size_t rangeEnd = static_cast<size_t>(firstIndex) + indexCount;
            for (size_t index = firstIndex; index < rangeEnd; ++index)
            {
                if (ReadIndex<T>(bytes, index) == RestartIndex)
                {
                    appendRun(runStart, index);
                    runStart = index + 1;
                }
            }
            appendRun(runStart, rangeEnd);

            result.Bytes.resize(expanded.size() * sizeof(T));
            if (!expanded.empty())
            {
                std::memcpy(result.Bytes.data(), expanded.data(), result.Bytes.size());
            }
            return result;
        }

        template<typename T>
        Result ExpandUnindexedTyped(uint32_t vertexCount, Mode mode)
        {
            const uint64_t expandedCount = PrimitiveIndexCount(vertexCount, mode);

            Result result{};
            result.Index32 = std::is_same_v<T, uint32_t>;
            result.IndexCount = CheckedIndexCount(expandedCount, sizeof(T));

            std::vector<T> expanded{};
            expanded.reserve(result.IndexCount);
            if (mode == Mode::LineLoop && vertexCount >= 2)
            {
                for (uint32_t index = 0; index + 1 < vertexCount; ++index)
                {
                    expanded.emplace_back(static_cast<T>(index));
                    expanded.emplace_back(static_cast<T>(index + 1));
                }
                expanded.emplace_back(static_cast<T>(vertexCount - 1));
                expanded.emplace_back(T{});
            }
            else if (mode == Mode::TriangleFan && vertexCount >= 3)
            {
                for (uint32_t index = 1; index + 1 < vertexCount; ++index)
                {
                    expanded.emplace_back(T{});
                    expanded.emplace_back(static_cast<T>(index));
                    expanded.emplace_back(static_cast<T>(index + 1));
                }
            }

            result.Bytes.resize(expanded.size() * sizeof(T));
            if (!expanded.empty())
            {
                std::memcpy(result.Bytes.data(), expanded.data(), result.Bytes.size());
            }
            return result;
        }
    }

    IndexData::IndexData(gsl::span<const uint8_t> bytes, bool index32)
        : m_index32{index32}
    {
        if (!bytes.empty())
        {
            m_bytes.assign(bytes.begin(), bytes.end());
        }
    }

    void IndexData::Update(gsl::span<const uint8_t> bytes, uint32_t startIndex)
    {
        const size_t byteStride = m_index32 ? sizeof(uint32_t) : sizeof(uint16_t);
        if (startIndex > std::numeric_limits<size_t>::max() / byteStride)
        {
            throw std::out_of_range{"Failed to update index buffer: buffer overflow"};
        }

        const size_t byteOffset = static_cast<size_t>(startIndex) * byteStride;
        if (byteOffset > m_bytes.size() || bytes.size() > m_bytes.size() - byteOffset)
        {
            throw std::out_of_range{"Failed to update index buffer: buffer overflow"};
        }

        if (bytes.empty())
        {
            return;
        }

        std::memcpy(m_bytes.data() + byteOffset, bytes.data(), bytes.size());
        ++m_revision;
    }

    void IndexData::Clear()
    {
        m_bytes.clear();
        ++m_revision;
    }

    gsl::span<const uint8_t> IndexData::Bytes() const
    {
        return gsl::span<const uint8_t>{m_bytes.data(), m_bytes.size()};
    }

    bool IndexData::Is32Bit() const
    {
        return m_index32;
    }

    uint64_t IndexData::Revision() const
    {
        return m_revision;
    }

    Result ExpandIndexed(const IndexData& data, uint32_t firstIndex, uint32_t indexCount, Mode mode)
    {
        return data.Is32Bit()
            ? ExpandIndexedTyped<uint32_t>(data, firstIndex, indexCount, mode)
            : ExpandIndexedTyped<uint16_t>(data, firstIndex, indexCount, mode);
    }

    Result ExpandUnindexed(uint32_t vertexCount, Mode mode)
    {
        return vertexCount <= std::numeric_limits<uint16_t>::max()
            ? ExpandUnindexedTyped<uint16_t>(vertexCount, mode)
            : ExpandUnindexedTyped<uint32_t>(vertexCount, mode);
    }
}
