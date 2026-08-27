#include "VertexBuffer.h"

#include <Babylon/Graphics/BgfxShaderInfo.h>
#include <gtest/gtest.h>

namespace
{
    using InstanceInfo = Babylon::VertexBuffer::InstanceInfo;

    uint32_t InstanceLocation(uint32_t slot)
    {
        return Babylon::Graphics::INSTANCE_DATA_FIRST_LOCATION - slot;
    }
}

TEST(NativeEngineInstanceData, SparseBuiltInsKeepCompilerAssignedSlots)
{
    std::map<uint32_t, uint32_t> builtInSlots{
        {InstanceLocation(7), 7},
        {InstanceLocation(6), 6},
        {InstanceLocation(5), 5},
        {InstanceLocation(4), 4},
        {InstanceLocation(3), 3},
        {InstanceLocation(2), 2},
        {InstanceLocation(1), 1},
        {InstanceLocation(0), 0},
    };

    std::map<uint32_t, InstanceInfo> instances{
        {InstanceLocation(7), {}},
        {InstanceLocation(6), {}},
        {InstanceLocation(5), {}},
        {InstanceLocation(4), {}},
        {static_cast<uint32_t>(bgfx::Attrib::TexCoord3), {}},
        {static_cast<uint32_t>(bgfx::Attrib::Position), {}},
    };

    const auto layout = Babylon::VertexBuffer::CreateInstanceDataLayout(
        instances, builtInSlots, Babylon::Graphics::MAX_INSTANCE_DATA_SLOT_COUNT);

    EXPECT_EQ(layout.SlotCount, 10u);
    EXPECT_EQ(layout.Slots.at(InstanceLocation(7)), 7u);
    EXPECT_EQ(layout.Slots.at(InstanceLocation(6)), 6u);
    EXPECT_EQ(layout.Slots.at(InstanceLocation(5)), 5u);
    EXPECT_EQ(layout.Slots.at(InstanceLocation(4)), 4u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::TexCoord3)), 8u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::Position)), 9u);
}

TEST(NativeEngineInstanceData, BuiltInsKeepAssignedSlotsAtRealLocations)
{
    std::map<uint32_t, uint32_t> builtInSlots{
        {static_cast<uint32_t>(bgfx::Attrib::Color0), 1},
        {static_cast<uint32_t>(bgfx::Attrib::TexCoord0), 0},
    };
    std::map<uint32_t, InstanceInfo> instances{
        {static_cast<uint32_t>(bgfx::Attrib::Position), {}},
        {static_cast<uint32_t>(bgfx::Attrib::Color0), {}},
        {static_cast<uint32_t>(bgfx::Attrib::TexCoord0), {}},
        {static_cast<uint32_t>(bgfx::Attrib::TexCoord3), {}},
    };

    const auto layout = Babylon::VertexBuffer::CreateInstanceDataLayout(
        instances, builtInSlots, Babylon::Graphics::MAX_INSTANCE_DATA_SLOT_COUNT);

    EXPECT_EQ(layout.SlotCount, 4u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::Color0)), 1u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::TexCoord0)), 0u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::TexCoord3)), 2u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::Position)), 3u);
}
