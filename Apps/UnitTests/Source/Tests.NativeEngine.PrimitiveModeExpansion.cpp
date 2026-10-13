#include "PrimitiveModeExpansion.h"

#include <gtest/gtest.h>

#include <cstring>
#include <limits>
#include <vector>

namespace
{
    template<typename T>
    std::vector<uint8_t> Bytes(std::initializer_list<T> values)
    {
        std::vector<uint8_t> bytes(values.size() * sizeof(T));
        if (!values.size())
        {
            return bytes;
        }
        std::memcpy(bytes.data(), values.begin(), bytes.size());
        return bytes;
    }

    template<typename T>
    std::vector<T> Indices(const Babylon::PrimitiveModeExpansion::Result& result)
    {
        EXPECT_EQ(result.Bytes.size() % sizeof(T), 0u);
        std::vector<T> indices(result.Bytes.size() / sizeof(T));
        if (!indices.empty())
        {
            std::memcpy(indices.data(), result.Bytes.data(), result.Bytes.size());
        }
        return indices;
    }
}

TEST(NativeEnginePrimitiveModeExpansion, UnindexedLineLoopClosesAndOffsetsFromNonzeroFirstVertex)
{
    constexpr uint32_t FirstVertex = 7;
    const auto result = Babylon::PrimitiveModeExpansion::ExpandUnindexed(
        4, Babylon::PrimitiveModeExpansion::Mode::LineLoop);

    EXPECT_FALSE(result.Index32);
    EXPECT_EQ(result.IndexCount, 8u);
    const auto relativeIndices = Indices<uint16_t>(result);
    EXPECT_EQ(relativeIndices, (std::vector<uint16_t>{0, 1, 1, 2, 2, 3, 3, 0}));

    std::vector<uint32_t> resolvedIndices{};
    for (const auto index : relativeIndices)
    {
        resolvedIndices.emplace_back(FirstVertex + index);
    }
    EXPECT_EQ(resolvedIndices, (std::vector<uint32_t>{7, 8, 8, 9, 9, 10, 10, 7}));
}

TEST(NativeEnginePrimitiveModeExpansion, UnindexedTriangleFanPreservesWinding)
{
    const auto result = Babylon::PrimitiveModeExpansion::ExpandUnindexed(
        5, Babylon::PrimitiveModeExpansion::Mode::TriangleFan);

    EXPECT_FALSE(result.Index32);
    EXPECT_EQ(Indices<uint16_t>(result), (std::vector<uint16_t>{0, 1, 2, 0, 2, 3, 0, 3, 4}));
}

TEST(NativeEnginePrimitiveModeExpansion, UnindexedExpansionUses32BitsBeforeRestartValue)
{
    const auto result = Babylon::PrimitiveModeExpansion::ExpandUnindexed(
        static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()) + 1,
        Babylon::PrimitiveModeExpansion::Mode::LineLoop);

    ASSERT_TRUE(result.Index32);
    const auto indices = Indices<uint32_t>(result);
    ASSERT_EQ(indices.size(), 131072u);
    EXPECT_EQ(indices[indices.size() - 2], 65535u);
    EXPECT_EQ(indices.back(), 0u);
}

TEST(NativeEnginePrimitiveModeExpansion, Indexed16BitSubrangeBecomesItsOwnLoop)
{
    const auto bytes = Bytes<uint16_t>({20, 21, 22, 23, 24, 25});
    const Babylon::PrimitiveModeExpansion::IndexData data{gsl::span<const uint8_t>{bytes.data(), bytes.size()}, false};
    const auto result = Babylon::PrimitiveModeExpansion::ExpandIndexed(
        data, 2, 3, Babylon::PrimitiveModeExpansion::Mode::LineLoop);

    EXPECT_FALSE(result.Index32);
    EXPECT_EQ(Indices<uint16_t>(result), (std::vector<uint16_t>{22, 23, 23, 24, 24, 22}));
}

TEST(NativeEnginePrimitiveModeExpansion, Indexed32BitFanUsesSubrangeRootAndPreservesValues)
{
    const auto bytes = Bytes<uint32_t>({100000, 200000, 300000, 400000, 500000});
    const Babylon::PrimitiveModeExpansion::IndexData data{gsl::span<const uint8_t>{bytes.data(), bytes.size()}, true};
    const auto result = Babylon::PrimitiveModeExpansion::ExpandIndexed(
        data, 1, 4, Babylon::PrimitiveModeExpansion::Mode::TriangleFan);

    EXPECT_TRUE(result.Index32);
    EXPECT_EQ(Indices<uint32_t>(result), (std::vector<uint32_t>{200000, 300000, 400000, 200000, 400000, 500000}));
}

TEST(NativeEnginePrimitiveModeExpansion, RestartSeparatesLineLoops)
{
    constexpr uint16_t Restart = std::numeric_limits<uint16_t>::max();
    const auto bytes = Bytes<uint16_t>({1, 2, 3, Restart, 10, 11, Restart, 20});
    const Babylon::PrimitiveModeExpansion::IndexData data{gsl::span<const uint8_t>{bytes.data(), bytes.size()}, false};
    const auto result = Babylon::PrimitiveModeExpansion::ExpandIndexed(
        data, 0, 8, Babylon::PrimitiveModeExpansion::Mode::LineLoop);

    EXPECT_EQ(Indices<uint16_t>(result), (std::vector<uint16_t>{1, 2, 2, 3, 3, 1, 10, 11, 11, 10}));
}

TEST(NativeEnginePrimitiveModeExpansion, RestartSeparatesTriangleFans)
{
    constexpr uint32_t Restart = std::numeric_limits<uint32_t>::max();
    const auto bytes = Bytes<uint32_t>({5, 6, 7, 8, Restart, 20, 21, Restart, 30, 31, 32});
    const Babylon::PrimitiveModeExpansion::IndexData data{gsl::span<const uint8_t>{bytes.data(), bytes.size()}, true};
    const auto result = Babylon::PrimitiveModeExpansion::ExpandIndexed(
        data, 0, 11, Babylon::PrimitiveModeExpansion::Mode::TriangleFan);

    EXPECT_EQ(Indices<uint32_t>(result), (std::vector<uint32_t>{5, 6, 7, 5, 7, 8, 30, 31, 32}));
}

TEST(NativeEnginePrimitiveModeExpansion, DynamicUpdateUsesIndexOffsetAndChangesExpansion)
{
    auto bytes = Bytes<uint16_t>({0, 1, 2, 3});
    Babylon::PrimitiveModeExpansion::IndexData data{gsl::span<const uint8_t>{bytes.data(), bytes.size()}, false};
    const auto before = Babylon::PrimitiveModeExpansion::ExpandIndexed(
        data, 0, 4, Babylon::PrimitiveModeExpansion::Mode::TriangleFan);

    const auto update = Bytes<uint16_t>({9, 10});
    data.Update(gsl::span<const uint8_t>{update.data(), update.size()}, 1);
    const auto after = Babylon::PrimitiveModeExpansion::ExpandIndexed(
        data, 0, 4, Babylon::PrimitiveModeExpansion::Mode::TriangleFan);

    EXPECT_EQ(data.Revision(), 1u);
    EXPECT_EQ(Indices<uint16_t>(before), (std::vector<uint16_t>{0, 1, 2, 0, 2, 3}));
    EXPECT_EQ(Indices<uint16_t>(after), (std::vector<uint16_t>{0, 9, 10, 0, 10, 3}));
}

TEST(NativeEnginePrimitiveModeExpansion, Dynamic32BitUpdateUsesIndexOffset)
{
    auto bytes = Bytes<uint32_t>({100000, 200000, 300000, 400000});
    Babylon::PrimitiveModeExpansion::IndexData data{gsl::span<const uint8_t>{bytes.data(), bytes.size()}, true};
    const auto update = Bytes<uint32_t>({900000});

    data.Update(gsl::span<const uint8_t>{update.data(), update.size()}, 2);
    const auto result = Babylon::PrimitiveModeExpansion::ExpandIndexed(
        data, 1, 3, Babylon::PrimitiveModeExpansion::Mode::LineLoop);

    EXPECT_EQ(Indices<uint32_t>(result), (std::vector<uint32_t>{200000, 900000, 900000, 400000, 400000, 200000}));
}

TEST(NativeEnginePrimitiveModeExpansion, TooShortInputsProduceNoPrimitives)
{
    EXPECT_EQ(
        Babylon::PrimitiveModeExpansion::ExpandUnindexed(1, Babylon::PrimitiveModeExpansion::Mode::LineLoop).IndexCount,
        0u);
    EXPECT_EQ(
        Babylon::PrimitiveModeExpansion::ExpandUnindexed(2, Babylon::PrimitiveModeExpansion::Mode::TriangleFan).IndexCount,
        0u);
}

TEST(NativeEnginePrimitiveModeExpansion, OversizedExpansionThrowsBeforeAllocation)
{
    EXPECT_THROW(
        Babylon::PrimitiveModeExpansion::ExpandUnindexed(
            std::numeric_limits<uint32_t>::max(),
            Babylon::PrimitiveModeExpansion::Mode::TriangleFan),
        std::length_error);
}

TEST(NativeEnginePrimitiveModeExpansion, InvalidRangesAndUpdatesThrow)
{
    auto bytes = Bytes<uint16_t>({0, 1, 2});
    Babylon::PrimitiveModeExpansion::IndexData data{gsl::span<const uint8_t>{bytes.data(), bytes.size()}, false};
    const auto update = Bytes<uint16_t>({7, 8});

    EXPECT_THROW(
        Babylon::PrimitiveModeExpansion::ExpandIndexed(
            data, 2, 2, Babylon::PrimitiveModeExpansion::Mode::LineLoop),
        std::out_of_range);
    EXPECT_THROW(data.Update(gsl::span<const uint8_t>{update.data(), update.size()}, 2), std::out_of_range);
}
