#pragma once

#include <cstdint>
#include <string>
#include <map>
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


    /// slots. Which slot each one gets is decided per shader, from the set the shader actually
    /// declares (see ShaderCompilerTraversers.cpp), because bgfx requires the used i_data slots to
    /// be a contiguous run starting at i_data0. The count is the size of the largest possible set:
    /// world0-3 (or splatIndex0-3), instanceColor, and previousWorld0-3 for motion vectors.
    /// BUILTIN_INSTANCE_DATA_LAST_LOCATION is the lowest synthetic location any of them can occupy;
    /// it is the boundary NativeEngine::Draw's "< bgfx::Attrib::Count means a real per-vertex
    /// attribute that needs rerouting" guard rests on, so it -- not just INSTANCE_DATA_FIRST_LOCATION
    /// -- must stay >= bgfx::Attrib::Count. Keep in sync when adding a built-in per-instance attribute.
    inline constexpr uint32_t BUILTIN_INSTANCE_DATA_SLOT_COUNT{9};
    inline constexpr uint32_t BUILTIN_INSTANCE_DATA_LAST_LOCATION{INSTANCE_DATA_FIRST_LOCATION - (BUILTIN_INSTANCE_DATA_SLOT_COUNT - 1)};

    struct BgfxShaderInfo
    {
        std::vector<uint8_t> VertexBytes{};
        std::vector<uint8_t> FragmentBytes{};
        std::map<std::string, uint32_t> VertexAttributeLocations{};
        std::map<std::string, uint8_t> UniformStages{};
    };
}
