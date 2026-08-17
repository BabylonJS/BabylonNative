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

    /// The built-in per-instance attributes occupy the top BUILTIN_INSTANCE_DATA_SLOT_COUNT i_data
    /// slots: world0-3 and splatIndex0-3 map to i_data0..i_data3, instanceColor to i_data4 (see
    /// ShaderCompilerTraversers.cpp's attribute table). BUILTIN_INSTANCE_DATA_LAST_LOCATION is the
    /// lowest synthetic location any of them can occupy; it is the boundary NativeEngine::Draw's
    /// "< bgfx::Attrib::Count means a real per-vertex attribute that needs rerouting" guard rests
    /// on, so it -- not just INSTANCE_DATA_FIRST_LOCATION -- must stay >= bgfx::Attrib::Count.
    /// Keep in sync when adding a built-in per-instance attribute on a lower i_data slot.
    inline constexpr uint32_t BUILTIN_INSTANCE_DATA_SLOT_COUNT{5};
    inline constexpr uint32_t BUILTIN_INSTANCE_DATA_LAST_LOCATION{INSTANCE_DATA_FIRST_LOCATION - (BUILTIN_INSTANCE_DATA_SLOT_COUNT - 1)};

    /// Name of the uniform the shader compiler declares in any fragment shader that reads
    /// gl_FragCoord, so FragCoordYFlipTraverser can convert the hardware's top-left-origin value
    /// into the bottom-left-origin one Babylon.js shaders are written against. Its .x/.y hold the
    /// width/height of the bound framebuffer, which NativeEngine writes before each draw.
    ///
    /// This cannot be bgfx's predefined u_viewRect: that is the view rect, which
    /// FrameBuffer::SetBgfxViewPortAndScissor narrows to the viewport whenever one is set, whereas
    /// gl_FragCoord is relative to the whole render target. The name is deliberately outside the
    /// u_ namespace Babylon.js uses for its own uniforms so it cannot collide with a shader uniform.
    inline constexpr const char* FRAGCOORD_TARGET_SIZE_UNIFORM_NAME{"bnFragCoordTargetSize"};

    struct BgfxShaderInfo
    {
        std::vector<uint8_t> VertexBytes{};
        std::vector<uint8_t> FragmentBytes{};
        std::map<std::string, uint32_t> VertexAttributeLocations{};
        std::map<std::string, uint8_t> UniformStages{};
    };
}
