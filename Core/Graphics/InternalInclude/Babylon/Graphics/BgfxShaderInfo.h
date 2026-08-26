#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <map>
#include <string_view>
#include <vector>

namespace Babylon::Graphics
{
    /// bgfx binds per-instance vertex data (i_data0, i_data1, ...) to descending TEXCOORD
    /// semantics beginning at BGFX_CONFIG_INSTANCE_DATA_FIRST_TEXCOORD: i_data0 == TEXCOORD31,
    /// i_data1 == TEXCOORD30, and so on. Those semantics sit above the regular vertex-attribute
    /// range (TEXCOORD0..15) and have no corresponding bgfx::Attrib enum value, so Babylon Native
    /// encodes every instance-data input as a synthetic attribute location equal to
    /// (bgfx::Attrib::TexCoord0 + semanticIndex). INSTANCE_DATA_FIRST_LOCATION is the synthetic
    /// location of i_data0 (TEXCOORD31); the location of i_data{n} is INSTANCE_DATA_FIRST_LOCATION - n.
    /// Any attribute whose location is >= bgfx::Attrib::Count is instance data (bound by semantic,
    /// not by bgfx::Attrib) and is excluded from the bgfx shader attribute table. ShaderCompilerCommon.cpp
    /// static_asserts these values against the live bgfx::Attrib enum. Defined here (rather than in
    /// ShaderCompiler.h) so the constants are visible even in builds without the shader compiler
    /// (e.g. precompiled-shader configurations).
    inline constexpr uint32_t INSTANCE_DATA_FIRST_TEXCOORD{31};
    inline constexpr uint32_t TEXCOORD0_ATTRIBUTE_LOCATION{10};
    inline constexpr uint32_t INSTANCE_DATA_FIRST_LOCATION{TEXCOORD0_ATTRIBUTE_LOCATION + INSTANCE_DATA_FIRST_TEXCOORD};

    /// Mirrors bgfx's BGFX_CONFIG_MAX_INSTANCE_DATA_COUNT (bgfx/src/config.h, a private header):
    /// the number of 16-byte per-instance slots (i_data0..i_data15) bgfx can bind in one draw.
    inline constexpr uint32_t MAX_INSTANCE_DATA_SLOT_COUNT{16};

    /// The built-in per-instance attributes occupy the top BUILTIN_INSTANCE_DATA_SLOT_COUNT i_data
    /// slots. Which slot each gets is decided per shader from the set it declares (see
    /// ShaderCompilerTraversers.cpp), because bgfx requires the used i_data slots to be a
    /// contiguous run starting at i_data0. The count is the largest possible set: world0-3 (or
    /// splatIndex0-3), instanceColor, and previousWorld0-3 for motion vectors.
    /// BUILTIN_INSTANCE_DATA_LAST_LOCATION is the lowest synthetic location any can occupy, and is
    /// the boundary NativeEngine::Draw's "< bgfx::Attrib::Count means a real per-vertex attribute"
    /// guard rests on, so it -- not just INSTANCE_DATA_FIRST_LOCATION -- must stay >= Attrib::Count.
    inline constexpr uint32_t BUILTIN_INSTANCE_DATA_SLOT_COUNT{9};
    inline constexpr uint32_t BUILTIN_INSTANCE_DATA_LAST_LOCATION{INSTANCE_DATA_FIRST_LOCATION - (BUILTIN_INSTANCE_DATA_SLOT_COUNT - 1)};

    /// The names Babylon.js uses for those attributes. The shader compiler recognizes them by name
    /// and NativeEngine counts how many a program declares to size the instance data buffer, so
    /// both must read the same table.
    inline constexpr std::array<std::string_view, 13> BUILTIN_INSTANCE_ATTRIBUTE_NAMES{
        "world0",
        "world1",
        "world2",
        "world3",
        "previousWorld0",
        "previousWorld1",
        "previousWorld2",
        "previousWorld3",
        "instanceColor",
        "splatIndex0",
        "splatIndex1",
        "splatIndex2",
        "splatIndex3",
    };

    inline constexpr bool IsBuiltInInstanceAttributeName(std::string_view name)
    {
        for (const std::string_view builtIn : BUILTIN_INSTANCE_ATTRIBUTE_NAMES)
        {
            if (builtIn == name)
            {
                return true;
            }
        }
        return false;
    }

    struct BgfxShaderInfo
    {
        std::vector<uint8_t> VertexBytes{};
        std::vector<uint8_t> FragmentBytes{};
        std::map<std::string, uint32_t> VertexAttributeLocations{};
        std::map<std::string, uint8_t> UniformStages{};
    };
}
