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
    inline constexpr uint32_t INSTANCE_DATA_LAST_LOCATION{INSTANCE_DATA_FIRST_LOCATION - (MAX_INSTANCE_DATA_SLOT_COUNT - 1)};

    /// Largest possible built-in set: world0-3 (or splatIndex0-3), instanceColor, and
    /// previousWorld0-3. The compiler emits the exact slot map for each shader.
    inline constexpr uint32_t BUILTIN_INSTANCE_DATA_SLOT_COUNT{9};

    /// The names Babylon.js uses for built-in per-instance attributes.
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
    /// Per-draw render target orientation, injected on top-left-origin backends (D3D/Metal/Vulkan).
    /// Like ANGLE's driver uniforms, it lets one compiled shader address either storage order:
    ///   .x/.y: window Y to GL Y, glY = x + y * windowY; also the dFdy sign (.y)
    ///   .z:    gl_Position.y scale
    /// Ordinary targets keep top-left rows, so NativeEngine writes (height, -1, 1, 0). Cube faces are
    /// rendered upside down into GL row order, as on OpenGL, so they get (0, 1, -1, 0).
    ///
    /// The height must be the bound framebuffer's, not bgfx's u_viewRect, which
    /// FrameBuffer::SetBgfxViewPortAndScissor narrows to the viewport whenever one is set, whereas
    /// gl_FragCoord is relative to the whole render target. The name is deliberately outside the
    /// u_ namespace Babylon.js uses for its own uniforms so it cannot collide with a shader uniform.
    inline constexpr const char* RENDER_TARGET_TRANSFORM_UNIFORM_NAME{"bnRenderTargetTransform"};
    inline constexpr const char* SAMPLER_STATE_UNIFORM_PREFIX{"bnSamplerState_"};

    /// WebGL2 transform feedback is emulated by compiling the capturing vertex shader into a
    /// compute shader (one invocation per vertex). Its vertex inputs report attribute locations
    /// starting at TRANSFORM_FEEDBACK_ATTRIBUTE_LOCATION_BASE, far above any bgfx attribute or
    /// instance-data location, so vertex arrays can record them as raw storage-buffer inputs.
    inline constexpr uint32_t TRANSFORM_FEEDBACK_ATTRIBUTE_LOCATION_BASE{0x10000};
    inline constexpr uint32_t TRANSFORM_FEEDBACK_WORKGROUP_SIZE{64};
    inline constexpr uint32_t TRANSFORM_FEEDBACK_INPUT_BINDING{0};
    inline constexpr uint32_t TRANSFORM_FEEDBACK_OUTPUT_BINDING{1};
    /// Vertex inputs may come from up to TRANSFORM_FEEDBACK_MAX_INPUT_BUFFERS vertex buffers. The first
    /// is bound at TRANSFORM_FEEDBACK_INPUT_BINDING, the rest from TRANSFORM_FEEDBACK_EXTRA_INPUT_BINDING.
    inline constexpr uint32_t TRANSFORM_FEEDBACK_MAX_INPUT_BUFFERS{4};
    inline constexpr uint32_t TRANSFORM_FEEDBACK_EXTRA_INPUT_BINDING{2};
    inline constexpr uint32_t TRANSFORM_FEEDBACK_FIRST_SAMPLER_BINDING{TRANSFORM_FEEDBACK_EXTRA_INPUT_BINDING + TRANSFORM_FEEDBACK_MAX_INPUT_BUFFERS - 1};
    inline constexpr uint32_t TRANSFORM_FEEDBACK_MAX_SAMPLER_BINDING{16};
    /// x = vertex count, y = first vertex.
    inline constexpr const char* TRANSFORM_FEEDBACK_PARAMS_UNIFORM{"bnTfParams"};
    /// Per vertex input: x = byte offset, y = byte stride, z = component count (0 = unbound),
    /// w = buffer index * 16 + TransformFeedbackInputType * 2 + normalized.
    inline constexpr const char* TRANSFORM_FEEDBACK_INPUT_LAYOUT_UNIFORM{"bnTfInputLayout"};
    enum class TransformFeedbackInputType : uint32_t
    {
        Float = 0,
        Int8 = 1,
        Uint8 = 2,
        Int16 = 3,
        Uint16 = 4,
    };

    struct BgfxShaderInfo
    {
        std::vector<uint8_t> VertexBytes{};
        std::vector<uint8_t> FragmentBytes{};
        std::vector<uint8_t> ComputeBytes{};
        std::map<std::string, uint32_t> VertexAttributeLocations{};
        std::map<std::string, uint32_t> BuiltInInstanceDataSlots{};
        std::map<std::string, uint8_t> UniformStages{};
        std::map<std::string, std::string> UniformNames{};
    };
}
