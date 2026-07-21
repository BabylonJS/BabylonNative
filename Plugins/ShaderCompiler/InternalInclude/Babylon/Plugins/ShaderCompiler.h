#pragma once

#include <string_view>
#include <cstdint>
#include <map>
#include <string>
#include <Babylon/Graphics/BgfxShaderInfo.h>

namespace Babylon::Plugins
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
    /// static_asserts these values against the live bgfx::Attrib enum.
    inline constexpr uint32_t INSTANCE_DATA_FIRST_TEXCOORD{31};
    inline constexpr uint32_t TEXCOORD0_ATTRIBUTE_LOCATION{10};
    inline constexpr uint32_t INSTANCE_DATA_FIRST_LOCATION{TEXCOORD0_ATTRIBUTE_LOCATION + INSTANCE_DATA_FIRST_TEXCOORD};

    /// This class is responsible for compiling the GLSL shader from Babylon.js into
    /// bgfx shader bytes with information about the shader attributes and uniforms.
    class ShaderCompiler final
    {
    public:
        ShaderCompiler();
        ~ShaderCompiler();

        /// `instancedAttributes` maps consumer-bound per-instance vertex-attribute names
        /// (in addition to the built-in instanced names) to the synthetic per-instance attribute
        /// location (a top TEXCOORD semantic, INSTANCE_DATA_FIRST_LOCATION down) they must occupy.
        /// The location is derived from the draw-time instance packing order so the shader reads
        /// each attribute from the slot bgfx fills. An empty map preserves the legacy per-vertex
        /// mapping for all non-built-in attributes.
        Graphics::BgfxShaderInfo Compile(std::string_view vertexSource, std::string_view fragmentSource, const std::map<std::string, uint32_t>& instancedAttributes = {});
    };
}
