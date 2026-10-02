/*
 * Copyright 2016-2021 The Brenwill Workshop Ltd.
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * At your option, you may choose to accept this material under either:
 *  1. The Apache License, Version 2.0, found at <http://www.apache.org/licenses/LICENSE-2.0>, or
 *  2. The MIT License, found at <http://opensource.org/licenses/MIT>.
 */

#ifndef SPIRV_CROSS_MSL_HPP
#define SPIRV_CROSS_MSL_HPP
#include "GLSL.std.450.h"
#include "spirv_cross.hpp"

#include <map>
#include <set>
#include <stddef.h>
#include <unordered_map>
#include <unordered_set>

namespace SPIRV_CROSS_NAMESPACE
{

// Indicates the format of a shader interface variable. Currently limited to specifying
// if the input is an 8-bit unsigned integer, 16-bit unsigned integer, or
// some other format.
enum MSLShaderVariableFormat
{
	MSL_SHADER_VARIABLE_FORMAT_OTHER = 0,
	MSL_SHADER_VARIABLE_FORMAT_UINT8 = 1,
	MSL_SHADER_VARIABLE_FORMAT_UINT16 = 2,
	MSL_SHADER_VARIABLE_FORMAT_ANY16 = 3,
	MSL_SHADER_VARIABLE_FORMAT_ANY32 = 4,

	// Deprecated aliases.
	MSL_VERTEX_FORMAT_OTHER = MSL_SHADER_VARIABLE_FORMAT_OTHER,
	MSL_VERTEX_FORMAT_UINT8 = MSL_SHADER_VARIABLE_FORMAT_UINT8,
	MSL_VERTEX_FORMAT_UINT16 = MSL_SHADER_VARIABLE_FORMAT_UINT16,
	MSL_SHADER_INPUT_FORMAT_OTHER = MSL_SHADER_VARIABLE_FORMAT_OTHER,
	MSL_SHADER_INPUT_FORMAT_UINT8 = MSL_SHADER_VARIABLE_FORMAT_UINT8,
	MSL_SHADER_INPUT_FORMAT_UINT16 = MSL_SHADER_VARIABLE_FORMAT_UINT16,
	MSL_SHADER_INPUT_FORMAT_ANY16 = MSL_SHADER_VARIABLE_FORMAT_ANY16,
	MSL_SHADER_INPUT_FORMAT_ANY32 = MSL_SHADER_VARIABLE_FORMAT_ANY32,

	MSL_SHADER_VARIABLE_FORMAT_INT_MAX = 0x7fffffff
};

// Indicates the rate at which a variable changes value, one of: per-vertex,
// per-primitive, or per-patch.
enum MSLShaderVariableRate
{
	MSL_SHADER_VARIABLE_RATE_PER_VERTEX = 0,
	MSL_SHADER_VARIABLE_RATE_PER_PRIMITIVE = 1,
	MSL_SHADER_VARIABLE_RATE_PER_PATCH = 2,

	MSL_SHADER_VARIABLE_RATE_INT_MAX = 0x7fffffff,
};

// Defines MSL characteristics of a shader interface variable at a particular location.
// After compilation, it is possible to query whether or not this location was used.
// If vecsize is nonzero, it must be greater than or equal to the vecsize declared in the shader,
// or behavior is undefined.
struct MSLShaderInterfaceVariable
{
	uint32_t location = 0;
	uint32_t component = 0;
	MSLShaderVariableFormat format = MSL_SHADER_VARIABLE_FORMAT_OTHER;
	spv::BuiltIn builtin = spv::BuiltInMax;
	uint32_t vecsize = 0;
	MSLShaderVariableRate rate = MSL_SHADER_VARIABLE_RATE_PER_VERTEX;
};

// Matches the binding index of a MSL resource for a binding within a descriptor set.
// Taken together, the stage, desc_set and binding combine to form a reference to a resource
// descriptor used in a particular shading stage. The count field indicates the number of
// resources consumed by this binding, if the binding represents an array of resources.
// If the resource array is a run-time-sized array, which are legal in GLSL or SPIR-V, this value
// will be used to declare the array size in MSL, which does not support run-time-sized arrays.
// If pad_argument_buffer_resources is enabled, the base_type and count values are used to
// specify the base type and array size of the resource in the argument buffer, if that resource
// is not defined and used by the shader. With pad_argument_buffer_resources enabled, this
// information will be used to pad the argument buffer structure, in order to align that
// structure consistently for all uses, across all shaders, of the descriptor set represented
// by the arugment buffer. If pad_argument_buffer_resources is disabled, base_type does not
// need to be populated, and if the resource is also not a run-time sized array, the count
// field does not need to be populated.
// If using MSL 2.0 argument buffers, the descriptor set is not marked as a discrete descriptor set,
// and (for iOS only) the resource is not a storage image (sampled != 2), the binding reference we
// remap to will become an [[id(N)]] attribute within the "descriptor set" argument buffer structure.
// For resources which are bound in the "classic" MSL 1.0 way or discrete descriptors, the remap will
// become a [[buffer(N)]], [[texture(N)]] or [[sampler(N)]] depending on the resource types used.
struct MSLResourceBinding
{
	spv::ExecutionModel stage = spv::ExecutionModelMax;
	SPIRType::BaseType basetype = SPIRType::Unknown;
	uint32_t desc_set = 0;
	uint32_t binding = 0;
	uint32_t count = 0;
	uint32_t msl_buffer = 0;
	uint32_t msl_texture = 0;
	uint32_t msl_sampler = 0;
};

enum MSLSamplerCoord
{
	MSL_SAMPLER_COORD_NORMALIZED = 0,
	MSL_SAMPLER_COORD_PIXEL = 1,
	MSL_SAMPLER_INT_MAX = 0x7fffffff
};

enum MSLSamplerFilter
{
	MSL_SAMPLER_FILTER_NEAREST = 0,
	MSL_SAMPLER_FILTER_LINEAR = 1,
	MSL_SAMPLER_FILTER_INT_MAX = 0x7fffffff
};

enum MSLSamplerMipFilter
{
	MSL_SAMPLER_MIP_FILTER_NONE = 0,
	MSL_SAMPLER_MIP_FILTER_NEAREST = 1,
	MSL_SAMPLER_MIP_FILTER_LINEAR = 2,
	MSL_SAMPLER_MIP_FILTER_INT_MAX = 0x7fffffff
};

enum MSLSamplerAddress
{
	MSL_SAMPLER_ADDRESS_CLAMP_TO_ZERO = 0,
	MSL_SAMPLER_ADDRESS_CLAMP_TO_EDGE = 1,
	MSL_SAMPLER_ADDRESS_CLAMP_TO_BORDER = 2,
	MSL_SAMPLER_ADDRESS_REPEAT = 3,
	MSL_SAMPLER_ADDRESS_MIRRORED_REPEAT = 4,
	MSL_SAMPLER_ADDRESS_INT_MAX = 0x7fffffff
};

enum MSLSamplerCompareFunc
{
	MSL_SAMPLER_COMPARE_FUNC_NEVER = 0,
	MSL_SAMPLER_COMPARE_FUNC_LESS = 1,
	MSL_SAMPLER_COMPARE_FUNC_LESS_EQUAL = 2,
	MSL_SAMPLER_COMPARE_FUNC_GREATER = 3,
	MSL_SAMPLER_COMPARE_FUNC_GREATER_EQUAL = 4,
	MSL_SAMPLER_COMPARE_FUNC_EQUAL = 5,
	MSL_SAMPLER_COMPARE_FUNC_NOT_EQUAL = 6,
	MSL_SAMPLER_COMPARE_FUNC_ALWAYS = 7,
	MSL_SAMPLER_COMPARE_FUNC_INT_MAX = 0x7fffffff
};

enum MSLSamplerBorderColor
{
	MSL_SAMPLER_BORDER_COLOR_TRANSPARENT_BLACK = 0,
	MSL_SAMPLER_BORDER_COLOR_OPAQUE_BLACK = 1,
	MSL_SAMPLER_BORDER_COLOR_OPAQUE_WHITE = 2,
	MSL_SAMPLER_BORDER_COLOR_INT_MAX = 0x7fffffff
};

enum MSLFormatResolution
{
	MSL_FORMAT_RESOLUTION_444 = 0,
	MSL_FORMAT_RESOLUTION_422,
	MSL_FORMAT_RESOLUTION_420,
	MSL_FORMAT_RESOLUTION_INT_MAX = 0x7fffffff
};

enum MSLChromaLocation
{
	MSL_CHROMA_LOCATION_COSITED_EVEN = 0,
	MSL_CHROMA_LOCATION_MIDPOINT,
	MSL_CHROMA_LOCATION_INT_MAX = 0x7fffffff
};

enum MSLComponentSwizzle
{
	MSL_COMPONENT_SWIZZLE_IDENTITY = 0,
	MSL_COMPONENT_SWIZZLE_ZERO,
	MSL_COMPONENT_SWIZZLE_ONE,
	MSL_COMPONENT_SWIZZLE_R,
	MSL_COMPONENT_SWIZZLE_G,
	MSL_COMPONENT_SWIZZLE_B,
	MSL_COMPONENT_SWIZZLE_A,
	MSL_COMPONENT_SWIZZLE_INT_MAX = 0x7fffffff
};

enum MSLSamplerYCbCrModelConversion
{
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY = 0,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_IDENTITY,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_BT_709,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_BT_601,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_BT_2020,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_INT_MAX = 0x7fffffff
};

enum MSLSamplerYCbCrRange
{
	MSL_SAMPLER_YCBCR_RANGE_ITU_FULL = 0,
	MSL_SAMPLER_YCBCR_RANGE_ITU_NARROW,
	MSL_SAMPLER_YCBCR_RANGE_INT_MAX = 0x7fffffff
};

struct MSLConstexprSampler
{
	MSLSamplerCoord coord = MSL_SAMPLER_COORD_NORMALIZED;
	MSLSamplerFilter min_filter = MSL_SAMPLER_FILTER_NEAREST;
	MSLSamplerFilter mag_filter = MSL_SAMPLER_FILTER_NEAREST;
	MSLSamplerMipFilter mip_filter = MSL_SAMPLER_MIP_FILTER_NONE;
	MSLSamplerAddress s_address = MSL_SAMPLER_ADDRESS_CLAMP_TO_EDGE;
	MSLSamplerAddress t_address = MSL_SAMPLER_ADDRESS_CLAMP_TO_EDGE;
	MSLSamplerAddress r_address = MSL_SAMPLER_ADDRESS_CLAMP_TO_EDGE;
	MSLSamplerCompareFunc compare_func = MSL_SAMPLER_COMPARE_FUNC_NEVER;
	MSLSamplerBorderColor border_color = MSL_SAMPLER_BORDER_COLOR_TRANSPARENT_BLACK;
	float lod_clamp_min = 0.0f;
	float lod_clamp_max = 1000.0f;
	int max_anisotropy = 1;

	// Sampler Y'CbCr conversion parameters
	uint32_t planes = 0;
	MSLFormatResolution resolution = MSL_FORMAT_RESOLUTION_444;
	MSLSamplerFilter chroma_filter = MSL_SAMPLER_FILTER_NEAREST;
	MSLChromaLocation x_chroma_offset = MSL_CHROMA_LOCATION_COSITED_EVEN;
	MSLChromaLocation y_chroma_offset = MSL_CHROMA_LOCATION_COSITED_EVEN;
	MSLComponentSwizzle swizzle[4]; // IDENTITY, IDENTITY, IDENTITY, IDENTITY
	MSLSamplerYCbCrModelConversion ycbcr_model = MSL_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY;
	MSLSamplerYCbCrRange ycbcr_range = MSL_SAMPLER_YCBCR_RANGE_ITU_FULL;
	uint32_t bpc = 8;

	bool compare_enable = false;
	bool lod_clamp_enable = false;
	bool anisotropy_enable = false;
	bool ycbcr_conversion_enable = false;

	MSLConstexprSampler()
	{
		for (uint32_t i = 0; i < 4; i++)
			swizzle[i] = MSL_COMPONENT_SWIZZLE_IDENTITY;
	}
	bool swizzle_is_identity() const
	{
		return (swizzle[0] == MSL_COMPONENT_SWIZZLE_IDENTITY && swizzle[1] == MSL_COMPONENT_SWIZZLE_IDENTITY &&
		        swizzle[2] == MSL_COMPONENT_SWIZZLE_IDENTITY && swizzle[3] == MSL_COMPONENT_SWIZZLE_IDENTITY);
	}
	bool swizzle_has_one_or_zero() const
	{
		return (swizzle[0] == MSL_COMPONENT_SWIZZLE_ZERO || swizzle[0] == MSL_COMPONENT_SWIZZLE_ONE ||
		        swizzle[1] == MSL_COMPONENT_SWIZZLE_ZERO || swizzle[1] == MSL_COMPONENT_SWIZZLE_ONE ||
		        swizzle[2] == MSL_COMPONENT_SWIZZLE_ZERO || swizzle[2] == MSL_COMPONENT_SWIZZLE_ONE ||
		        swizzle[3] == MSL_COMPONENT_SWIZZLE_ZERO || swizzle[3] == MSL_COMPONENT_SWIZZLE_ONE);
	}
};

// Special constant used in a MSLResourceBinding desc_set
// element to indicate the bindings for the push constants.
// Kinda deprecated. Just use ResourceBindingPushConstant{DescriptorSet,Binding} directly.
static const uint32_t kPushConstDescSet = ResourceBindingPushConstantDescriptorSet;

// Special constant used in a MSLResourceBinding binding
// element to indicate the bindings for the push constants.
// Kinda deprecated. Just use ResourceBindingPushConstant{DescriptorSet,Binding} directly.
static const uint32_t kPushConstBinding = ResourceBindingPushConstantBinding;

// Special constant used in a MSLResourceBinding binding
// element to indicate the buffer binding for swizzle buffers.
static const uint32_t kSwizzleBufferBinding = ~(1u);

// Special constant used in a MSLResourceBinding binding
// element to indicate the buffer binding for buffer size buffers to support OpArrayLength.
static const uint32_t kBufferSizeBufferBinding = ~(2u);

// Special constant used in a MSLResourceBinding binding
// element to indicate the buffer binding used for the argument buffer itself.
// This buffer binding should be kept as small as possible as all automatic bindings for buffers
// will start at max(kArgumentBufferBinding) + 1.
static const uint32_t kArgumentBufferBinding = ~(3u);

static const uint32_t kMaxArgumentBuffers = 8;

// The arbitrary maximum for the nesting of array of array copies.
static const uint32_t kArrayCopyMultidimMax = 6;

// Decompiles SPIR-V to Metal Shading Language
class CompilerMSL : public Compiler
{
public:
	enum PlsFormat
	{
		PlsNone = 0,

		PlsR11FG11FB10F,
		PlsR32F,
		PlsRG16F,
		PlsRGB10A2,
		PlsRGBA8,
		PlsRG16,

		PlsRGBA8I,
		PlsRG16I,

		PlsRGB10A2UI,
		PlsRGBA8UI,
		PlsRG16UI,
		PlsR32UI
	};

	struct PlsRemap
	{
		uint32_t id;
		PlsFormat format;
	};

	// Options for compiling to Metal Shading Language
	struct Options
	{
		typedef enum
		{
			iOS = 0,
			macOS = 1
		} Platform;

		Platform platform = macOS;
		uint32_t msl_version = make_msl_version(1, 2);
		uint32_t texel_buffer_texture_width = 4096; // Width of 2D Metal textures used as 1D texel buffers
		uint32_t r32ui_linear_texture_alignment = 4;
		uint32_t r32ui_alignment_constant_id = 65535;
		uint32_t swizzle_buffer_index = 30;
		uint32_t indirect_params_buffer_index = 29;
		uint32_t shader_output_buffer_index = 28;
		uint32_t shader_patch_output_buffer_index = 27;
		uint32_t shader_tess_factor_buffer_index = 26;
		uint32_t buffer_size_buffer_index = 25;
		uint32_t view_mask_buffer_index = 24;
		uint32_t dynamic_offsets_buffer_index = 23;
		uint32_t shader_input_buffer_index = 22;
		uint32_t shader_index_buffer_index = 21;
		uint32_t shader_patch_input_buffer_index = 20;
		uint32_t shader_input_wg_index = 0;
		uint32_t device_index = 0;
		uint32_t enable_frag_output_mask = 0xffffffff;
		// Metal doesn't allow setting a fixed sample mask directly in the pipeline.
		// We can evade this restriction by ANDing the internal sample_mask output
		// of the shader with the additional fixed sample mask.
		uint32_t additional_fixed_sample_mask = 0xffffffff;
		bool enable_point_size_builtin = true;
		bool enable_frag_depth_builtin = true;
		bool enable_frag_stencil_ref_builtin = true;
		bool disable_rasterization = false;
		bool capture_output_to_buffer = false;
		bool swizzle_texture_samples = false;
		bool tess_domain_origin_lower_left = false;
		bool multiview = false;
		bool multiview_layered_rendering = true;
		bool view_index_from_device_index = false;
		bool dispatch_base = false;
		bool texture_1D_as_2D = false;

		// Enable use of Metal argument buffers.
		// MSL 2.0 must also be enabled.
		bool argument_buffers = false;

		// Defines Metal argument buffer tier levels.
		// Uses same values as Metal MTLArgumentBuffersTier enumeration.
		enum class ArgumentBuffersTier
		{
			Tier1 = 0,
			Tier2 = 1,
		};

		// When using Metal argument buffers, indicates the Metal argument buffer tier level supported by the Metal platform.
		// Ignored when Options::argument_buffers is disabled.
		// - Tier1 supports writable images on macOS, but not on iOS.
		// - Tier2 supports writable images on macOS and iOS, and higher resource count limits.
		// Tier capabilities based on recommendations from Apple engineering.
		ArgumentBuffersTier argument_buffers_tier = ArgumentBuffersTier::Tier1;

		// Enables specifick argument buffer format with extra information to track SSBO-length
		bool runtime_array_rich_descriptor = false;

		// Ensures vertex and instance indices start at zero. This reflects the behavior of HLSL with SV_VertexID and SV_InstanceID.
		bool enable_base_index_zero = false;

		// Fragment output in MSL must have at least as many components as the render pass.
		// Add support to explicit pad out components.
		bool pad_fragment_output_components = false;

		// Specifies whether the iOS target version supports the [[base_vertex]] and [[base_instance]] attributes.
		bool ios_support_base_vertex_instance = false;

		// Use Metal's native frame-buffer fetch API for subpass inputs.
		bool use_framebuffer_fetch_subpasses = false;

		// Enables use of "fma" intrinsic for invariant float math
		bool invariant_float_math = false;

		// Emulate texturecube_array with texture2d_array for iOS where this type is not available
		bool emulate_cube_array = false;

		// Allow user to enable decoration binding
		bool enable_decoration_binding = false;

		// Requires MSL 2.1, use the native support for texel buffers.
		bool texture_buffer_native = false;

		// Forces all resources which are part of an argument buffer to be considered active.
		// This ensures ABI compatibility between shaders where some resources might be unused,
		// and would otherwise declare a different IAB.
		bool force_active_argument_buffer_resources = false;

		// Aligns each resource in an argument buffer to its assigned index value, id(N),
		// by adding synthetic padding members in the argument buffer struct for any resources
		// in the argument buffer that are not defined and used by the shader. This allows
		// the shader to index into the correct argument in a descriptor set argument buffer
		// that is shared across shaders, where not all resources in the argument buffer are
		// defined in each shader. For this to work, an MSLResourceBinding must be provided for
		// all descriptors in any descriptor set held in an argument buffer in the shader, and
		// that MSLResourceBinding must have the basetype and count members populated correctly.
		// The implementation here assumes any inline blocks in the argument buffer is provided
		// in a Metal buffer, and doesn't take into consideration inline blocks that are
		// optionally embedded directly into the argument buffer via add_inline_uniform_block().
		bool pad_argument_buffer_resources = false;

		// Forces the use of plain arrays, which works around certain driver bugs on certain versions
		// of Intel Macbooks. See https://github.com/KhronosGroup/SPIRV-Cross/issues/1210.
		// May reduce performance in scenarios where arrays are copied around as value-types.
		bool force_native_arrays = false;

		// If a shader writes clip distance, also emit user varyings which
		// can be read in subsequent stages.
		bool enable_clip_distance_user_varying = true;

		// In a tessellation control shader, assume that more than one patch can be processed in a
		// single workgroup. This requires changes to the way the InvocationId and PrimitiveId
		// builtins are processed, but should result in more efficient usage of the GPU.
		bool multi_patch_workgroup = false;

		// Use storage buffers instead of vertex-style attributes for tessellation evaluation
		// input. This may require conversion of inputs in the generated post-tessellation
		// vertex shader, but allows the use of nested arrays.
		bool raw_buffer_tese_input = false;

		// If set, a vertex shader will be compiled as part of a tessellation pipeline.
		// It will be translated as a compute kernel, so it can use the global invocation ID
		// to index the output buffer.
		bool vertex_for_tessellation = false;

		// Assume that SubpassData images have multiple layers. Layered input attachments
		// are addressed relative to the Layer output from the vertex pipeline. This option
		// has no effect with multiview, since all input attachments are assumed to be layered
		// and will be addressed using the current ViewIndex.
		bool arrayed_subpass_input = false;

		// Whether to use SIMD-group or quadgroup functions to implement group non-uniform
		// operations. Some GPUs on iOS do not support the SIMD-group functions, only the
		// quadgroup functions.
		bool ios_use_simdgroup_functions = false;

		// If set, the subgroup size will be assumed to be one, and subgroup-related
		// builtins and operations will be emitted accordingly. This mode is intended to
		// be used by MoltenVK on hardware/software configurations which do not provide
		// sufficient support for subgroups.
		bool emulate_subgroups = false;

		// If nonzero, a fixed subgroup size to assume. Metal, similarly to VK_EXT_subgroup_size_control,
		// allows the SIMD-group size (aka thread execution width) to vary depending on
		// register usage and requirements. In certain circumstances--for example, a pipeline
		// in MoltenVK without VK_PIPELINE_SHADER_STAGE_CREATE_ALLOW_VARYING_SUBGROUP_SIZE_BIT_EXT--
		// this is undesirable. This fixes the value of the SubgroupSize builtin, instead of
		// mapping it to the Metal builtin [[thread_execution_width]]. If the thread
		// execution width is reduced, the extra invocations will appear to be inactive.
		// If zero, the SubgroupSize will be allowed to vary, and the builtin will be mapped
		// to the Metal [[thread_execution_width]] builtin.
		uint32_t fixed_subgroup_size = 0;

		enum class IndexType
		{
			None = 0,
			UInt16 = 1,
			UInt32 = 2
		};

		// The type of index in the index buffer, if present. For a compute shader, Metal
		// requires specifying the indexing at pipeline creation, rather than at draw time
		// as with graphics pipelines. This means we must create three different pipelines,
		// for no indexing, 16-bit indices, and 32-bit indices. Each requires different
		// handling for the gl_VertexIndex builtin. We may as well, then, create three
		// different shaders for these three scenarios.
		IndexType vertex_index_type = IndexType::None;

		// If set, a dummy [[sample_id]] input is added to a fragment shader if none is present.
		// This will force the shader to run at sample rate, assuming Metal does not optimize
		// the extra threads away.
		bool force_sample_rate_shading = false;

		// If set, gl_HelperInvocation will be set manually whenever a fragment is discarded.
		// Some Metal devices have a bug where simd_is_helper_thread() does not return true
		// after a fragment has been discarded. This is a workaround that is only expected to be needed
		// until the bug is fixed in Metal; it is provided as an option to allow disabling it when that occurs.
		bool manual_helper_invocation_updates = true;

		// If set, extra checks will be emitted in fragment shaders to prevent writes
		// from discarded fragments. Some Metal devices have a bug where writes to storage resources
		// from discarded fragment threads continue to occur, despite the fragment being
		// discarded. This is a workaround that is only expected to be needed until the
		// bug is fixed in Metal; it is provided as an option so it can be enabled
		// only when the bug is present.
		bool check_discarded_frag_stores = false;

		// If set, Lod operands to OpImageSample*DrefExplicitLod for 1D and 2D array images
		// will be implemented using a gradient instead of passing the level operand directly.
		// Some Metal devices have a bug where the level() argument to depth2d_array<T>::sample_compare()
		// in a fragment shader is biased by some unknown amount, possibly dependent on the
		// partial derivatives of the texture coordinates. This is a workaround that is only
		// expected to be needed until the bug is fixed in Metal; it is provided as an option
		// so it can be enabled only when the bug is present.
		bool sample_dref_lod_array_as_grad = false;

		// MSL doesn't guarantee coherence between writes and subsequent reads of read_write textures.
		// This inserts fences before each read of a read_write texture to ensure coherency.
		// If you're sure you never rely on this, you can set this to false for a possible performance improvement.
		// Note: Only Apple's GPU compiler takes advantage of the lack of coherency, so make sure to test on Apple GPUs if you disable this.
		bool readwrite_texture_fences = true;

		bool is_ios() const
		{
			return platform == iOS;
		}

		bool is_macos() const
		{
			return platform == macOS;
		}

		bool use_quadgroup_operation() const
		{
			return is_ios() && !ios_use_simdgroup_functions;
		}

		void set_msl_version(uint32_t major, uint32_t minor = 0, uint32_t patch = 0)
		{
			msl_version = make_msl_version(major, minor, patch);
		}

		bool supports_msl_version(uint32_t major, uint32_t minor = 0, uint32_t patch = 0) const
		{
			return msl_version >= make_msl_version(major, minor, patch);
		}

		static uint32_t make_msl_version(uint32_t major, uint32_t minor = 0, uint32_t patch = 0)
		{
			return (major * 10000) + (minor * 100) + patch;
		}
	};

	struct TextureFunctionBaseArguments
	{
		// GCC 4.8 workarounds, it doesn't understand '{}' constructor here, use explicit default constructor.
		TextureFunctionBaseArguments() = default;
		VariableID img = 0;
		const SPIRType *imgtype = nullptr;
		bool is_fetch = false, is_gather = false, is_proj = false;
	};

	struct TextureFunctionNameArguments
	{
		// GCC 4.8 workarounds, it doesn't understand '{}' constructor here, use explicit default constructor.
		TextureFunctionNameArguments() = default;
		TextureFunctionBaseArguments base;
		bool has_array_offsets = false, has_offset = false, has_grad = false;
		bool has_dref = false, is_sparse_feedback = false, has_min_lod = false;
		uint32_t lod = 0;
	};
	
	struct TextureFunctionArguments
	{
		// GCC 4.8 workarounds, it doesn't understand '{}' constructor here, use explicit default constructor.
		TextureFunctionArguments() = default;
		TextureFunctionBaseArguments base;
		uint32_t coord = 0, coord_components = 0, dref = 0;
		uint32_t grad_x = 0, grad_y = 0, lod = 0, offset = 0;
		uint32_t bias = 0, component = 0, sample = 0, sparse_texel = 0, min_lod = 0;
		bool nonuniform_expression = false;
	};

	const Options &get_msl_options() const
	{
		return msl_options;
	}

	void set_msl_options(const Options &opts)
	{
		msl_options = opts;
	}

	// Provide feedback to calling API to allow runtime to disable pipeline
	// rasterization if vertex shader requires rasterization to be disabled.
	bool get_is_rasterization_disabled() const
	{
		return is_rasterization_disabled && (get_entry_point().model == spv::ExecutionModelVertex ||
		                                     get_entry_point().model == spv::ExecutionModelTessellationControl ||
		                                     get_entry_point().model == spv::ExecutionModelTessellationEvaluation);
	}

	// Provide feedback to calling API to allow it to pass an auxiliary
	// swizzle buffer if the shader needs it.
	bool needs_swizzle_buffer() const
	{
		return used_swizzle_buffer;
	}

	// Provide feedback to calling API to allow it to pass a buffer
	// containing STORAGE_BUFFER buffer sizes to support OpArrayLength.
	bool needs_buffer_size_buffer() const
	{
		return !buffers_requiring_array_length.empty();
	}

	bool buffer_requires_array_length(VariableID id) const
	{
		return buffers_requiring_array_length.count(id) != 0;
	}

	// Provide feedback to calling API to allow it to pass a buffer
	// containing the view mask for the current multiview subpass.
	bool needs_view_mask_buffer() const
	{
		return msl_options.multiview && !msl_options.view_index_from_device_index;
	}

	// Provide feedback to calling API to allow it to pass a buffer
	// containing the dispatch base workgroup ID.
	bool needs_dispatch_base_buffer() const
	{
		return msl_options.dispatch_base && !msl_options.supports_msl_version(1, 2);
	}

	// Provide feedback to calling API to allow it to pass an output
	// buffer if the shader needs it.
	bool needs_output_buffer() const
	{
		return capture_output_to_buffer && stage_out_var_id != ID(0);
	}

	// Provide feedback to calling API to allow it to pass a patch output
	// buffer if the shader needs it.
	bool needs_patch_output_buffer() const
	{
		return capture_output_to_buffer && patch_stage_out_var_id != ID(0);
	}

	// Provide feedback to calling API to allow it to pass an input threadgroup
	// buffer if the shader needs it.
	bool needs_input_threadgroup_mem() const
	{
		return capture_output_to_buffer && stage_in_var_id != ID(0);
	}

	explicit CompilerMSL(std::vector<uint32_t> spirv);
	CompilerMSL(const uint32_t *ir, size_t word_count);
	explicit CompilerMSL(const ParsedIR &ir);
	explicit CompilerMSL(ParsedIR &&ir);

	// input is a shader interface variable description used to fix up shader input variables.
	// If shader inputs are provided, is_msl_shader_input_used() will return true after
	// calling ::compile() if the location were used by the MSL code.
	void add_msl_shader_input(const MSLShaderInterfaceVariable &input);

	// output is a shader interface variable description used to fix up shader output variables.
	// If shader outputs are provided, is_msl_shader_output_used() will return true after
	// calling ::compile() if the location were used by the MSL code.
	void add_msl_shader_output(const MSLShaderInterfaceVariable &output);

	// resource is a resource binding to indicate the MSL buffer,
	// texture or sampler index to use for a particular SPIR-V description set
	// and binding. If resource bindings are provided,
	// is_msl_resource_binding_used() will return true after calling ::compile() if
	// the set/binding combination was used by the MSL code.
	void add_msl_resource_binding(const MSLResourceBinding &resource);

	// desc_set and binding are the SPIR-V descriptor set and binding of a buffer resource
	// in this shader. index is the index within the dynamic offset buffer to use. This
	// function marks that resource as using a dynamic offset (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
	// or VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC). This function only has any effect if argument buffers
	// are enabled. If so, the buffer will have its address adjusted at the beginning of the shader with
	// an offset taken from the dynamic offset buffer.
	void add_dynamic_buffer(uint32_t desc_set, uint32_t binding, uint32_t index);

	// desc_set and binding are the SPIR-V descriptor set and binding of a buffer resource
	// in this shader. This function marks that resource as an inline uniform block
	// (VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT). This function only has any effect if argument buffers
	// are enabled. If so, the buffer block will be directly embedded into the argument
	// buffer, instead of being referenced indirectly via pointer.
	void add_inline_uniform_block(uint32_t desc_set, uint32_t binding);

	// When using MSL argument buffers, we can force "classic" MSL 1.0 binding schemes for certain descriptor sets.
	// This corresponds to VK_KHR_push_descriptor in Vulkan.
	void add_discrete_descriptor_set(uint32_t desc_set);

	// If an argument buffer is large enough, it may need to be in the device storage space rather than
	// constant. Opt-in to this behavior here on a per set basis.
	void set_argument_buffer_device_address_space(uint32_t desc_set, bool device_storage);

	// Query after compilation is done. This allows you to check if an input location was used by the shader.
	bool is_msl_shader_input_used(uint32_t location);

	// Query after compilation is done. This allows you to check if an output location were used by the shader.
	bool is_msl_shader_output_used(uint32_t location);

	// If not using add_msl_shader_input, it's possible
	// that certain builtin attributes need to be automatically assigned locations.
	// This is typical for tessellation builtin inputs such as tess levels, gl_Position, etc.
	// This returns k_unknown_location if the location was explicitly assigned with
	// add_msl_shader_input or the builtin is not used, otherwise returns N in [[attribute(N)]].
	uint32_t get_automatic_builtin_input_location(spv::BuiltIn builtin) const;

	// If not using add_msl_shader_output, it's possible
	// that certain builtin attributes need to be automatically assigned locations.
	// This is typical for tessellation builtin outputs such as tess levels, gl_Position, etc.
	// This returns k_unknown_location if the location were explicitly assigned with
	// add_msl_shader_output or the builtin were not used, otherwise returns N in [[attribute(N)]].
	uint32_t get_automatic_builtin_output_location(spv::BuiltIn builtin) const;

	// NOTE: Only resources which are remapped using add_msl_resource_binding will be reported here.
	// Constexpr samplers are always assumed to be emitted.
	// No specific MSLResourceBinding remapping is required for constexpr samplers as long as they are remapped
	// by remap_constexpr_sampler(_by_binding).
	bool is_msl_resource_binding_used(spv::ExecutionModel model, uint32_t set, uint32_t binding) const;

	// This must only be called after a successful call to CompilerMSL::compile().
	// For a variable resource ID obtained through reflection API, report the automatically assigned resource index.
	// If the descriptor set was part of an argument buffer, report the [[id(N)]],
	// or [[buffer/texture/sampler]] binding for other resources.
	// If the resource was a combined image sampler, report the image binding here,
	// use the _secondary version of this call to query the sampler half of the resource.
	// If no binding exists, uint32_t(-1) is returned.
	uint32_t get_automatic_msl_resource_binding(uint32_t id) const;

	// Same as get_automatic_msl_resource_binding, but should only be used for combined image samplers, in which case the
	// sampler's binding is returned instead. For any other resource type, -1 is returned.
	// Secondary bindings are also used for the auxillary image atomic buffer.
	uint32_t get_automatic_msl_resource_binding_secondary(uint32_t id) const;

	// Same as get_automatic_msl_resource_binding, but should only be used for combined image samplers for multiplanar images,
	// in which case the second plane's binding is returned instead. For any other resource type, -1 is returned.
	uint32_t get_automatic_msl_resource_binding_tertiary(uint32_t id) const;

	// Same as get_automatic_msl_resource_binding, but should only be used for combined image samplers for triplanar images,
	// in which case the third plane's binding is returned instead. For any other resource type, -1 is returned.
	uint32_t get_automatic_msl_resource_binding_quaternary(uint32_t id) const;

	// Compiles the SPIR-V code into Metal Shading Language.
	std::string compile() override;

	// Remap a sampler with ID to a constexpr sampler.
	// Older iOS targets must use constexpr samplers in certain cases (PCF),
	// so a static sampler must be used.
	// The sampler will not consume a binding, but be declared in the entry point as a constexpr sampler.
	// This can be used on both combined image/samplers (sampler2D) or standalone samplers.
	// The remapped sampler must not be an array of samplers.
	// Prefer remap_constexpr_sampler_by_binding unless you're also doing reflection anyways.
	void remap_constexpr_sampler(VariableID id, const MSLConstexprSampler &sampler);

	// Same as remap_constexpr_sampler, except you provide set/binding, rather than variable ID.
	// Remaps based on ID take priority over set/binding remaps.
	void remap_constexpr_sampler_by_binding(uint32_t desc_set, uint32_t binding, const MSLConstexprSampler &sampler);

	// If using CompilerMSL::Options::pad_fragment_output_components, override the number of components we expect
	// to use for a particular location. The default is 4 if number of components is not overridden.
	void set_fragment_output_components(uint32_t location, uint32_t components);

	void set_combined_sampler_suffix(const char *suffix);
	const char *get_combined_sampler_suffix() const;

protected:
	// An enum of SPIR-V functions that are implemented in additional
	// source code that is added to the shader if necessary.
	enum SPVFuncImpl : uint8_t
	{
		SPVFuncImplNone,
		SPVFuncImplMod,
		SPVFuncImplRadians,
		SPVFuncImplDegrees,
		SPVFuncImplFindILsb,
		SPVFuncImplFindSMsb,
		SPVFuncImplFindUMsb,
		SPVFuncImplSSign,
		SPVFuncImplArrayCopyMultidimBase,
		// Unfortunately, we cannot use recursive templates in the MSL compiler properly,
		// so stamp out variants up to some arbitrary maximum.
		SPVFuncImplArrayCopy = SPVFuncImplArrayCopyMultidimBase + 1,
		SPVFuncImplArrayOfArrayCopy2Dim = SPVFuncImplArrayCopyMultidimBase + 2,
		SPVFuncImplArrayOfArrayCopy3Dim = SPVFuncImplArrayCopyMultidimBase + 3,
		SPVFuncImplArrayOfArrayCopy4Dim = SPVFuncImplArrayCopyMultidimBase + 4,
		SPVFuncImplArrayOfArrayCopy5Dim = SPVFuncImplArrayCopyMultidimBase + 5,
		SPVFuncImplArrayOfArrayCopy6Dim = SPVFuncImplArrayCopyMultidimBase + 6,
		SPVFuncImplTexelBufferCoords,
		SPVFuncImplImage2DAtomicCoords, // Emulate texture2D atomic operations
		SPVFuncImplFMul,
		SPVFuncImplFAdd,
		SPVFuncImplFSub,
		SPVFuncImplQuantizeToF16,
		SPVFuncImplCubemapTo2DArrayFace,
		SPVFuncImplUnsafeArray, // Allow Metal to use the array<T> template to make arrays a value type
		SPVFuncImplStorageMatrix, // Allow threadgroup construction of matrices
		SPVFuncImplInverse4x4,
		SPVFuncImplInverse3x3,
		SPVFuncImplInverse2x2,
		// It is very important that this come before *Swizzle and ChromaReconstruct*, to ensure it's
		// emitted before them.
		SPVFuncImplForwardArgs,
		// Likewise, this must come before *Swizzle.
		SPVFuncImplGetSwizzle,
		SPVFuncImplTextureSwizzle,
		SPVFuncImplGatherSwizzle,
		SPVFuncImplGatherCompareSwizzle,
		SPVFuncImplSubgroupBroadcast,
		SPVFuncImplSubgroupBroadcastFirst,
		SPVFuncImplSubgroupBallot,
		SPVFuncImplSubgroupBallotBitExtract,
		SPVFuncImplSubgroupBallotFindLSB,
		SPVFuncImplSubgroupBallotFindMSB,
		SPVFuncImplSubgroupBallotBitCount,
		SPVFuncImplSubgroupAllEqual,
		SPVFuncImplSubgroupShuffle,
		SPVFuncImplSubgroupShuffleXor,
		SPVFuncImplSubgroupShuffleUp,
		SPVFuncImplSubgroupShuffleDown,
		SPVFuncImplQuadBroadcast,
		SPVFuncImplQuadSwap,
		SPVFuncImplReflectScalar,
		SPVFuncImplRefractScalar,
		SPVFuncImplFaceForwardScalar,
		SPVFuncImplChromaReconstructNearest2Plane,
		SPVFuncImplChromaReconstructNearest3Plane,
		SPVFuncImplChromaReconstructLinear422CositedEven2Plane,
		SPVFuncImplChromaReconstructLinear422CositedEven3Plane,
		SPVFuncImplChromaReconstructLinear422Midpoint2Plane,
		SPVFuncImplChromaReconstructLinear422Midpoint3Plane,
		SPVFuncImplChromaReconstructLinear420XCositedEvenYCositedEven2Plane,
		SPVFuncImplChromaReconstructLinear420XCositedEvenYCositedEven3Plane,
		SPVFuncImplChromaReconstructLinear420XMidpointYCositedEven2Plane,
		SPVFuncImplChromaReconstructLinear420XMidpointYCositedEven3Plane,
		SPVFuncImplChromaReconstructLinear420XCositedEvenYMidpoint2Plane,
		SPVFuncImplChromaReconstructLinear420XCositedEvenYMidpoint3Plane,
		SPVFuncImplChromaReconstructLinear420XMidpointYMidpoint2Plane,
		SPVFuncImplChromaReconstructLinear420XMidpointYMidpoint3Plane,
		SPVFuncImplExpandITUFullRange,
		SPVFuncImplExpandITUNarrowRange,
		SPVFuncImplConvertYCbCrBT709,
		SPVFuncImplConvertYCbCrBT601,
		SPVFuncImplConvertYCbCrBT2020,
		SPVFuncImplDynamicImageSampler,
		SPVFuncImplRayQueryIntersectionParams,
		SPVFuncImplVariableDescriptor,
		SPVFuncImplVariableSizedDescriptor,
		SPVFuncImplVariableDescriptorArray,
	};

	// If the underlying resource has been used for comparison then duplicate loads of that resource must be too
	// Use Metal's native frame-buffer fetch API for subpass inputs.
	void emit_texture_op(const Instruction &i, bool sparse);
	void emit_binary_ptr_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op);
	std::string to_ptr_expression(uint32_t id, bool register_expression_read = true);
	void emit_binary_unord_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op);
	void emit_instruction(const Instruction &instr);
	void emit_glsl_op(uint32_t result_type, uint32_t result_id, uint32_t op, const uint32_t *args,
	                  uint32_t count);
	void emit_spv_amd_shader_trinary_minmax_op(uint32_t result_type, uint32_t result_id, uint32_t op,
	                                           const uint32_t *args, uint32_t count);
	void emit_header();
	void emit_function_prototype(SPIRFunction &func, const Bitset &return_flags);
	void emit_sampled_image_op(uint32_t result_type, uint32_t result_id, uint32_t image_id, uint32_t samp_id);
	void emit_subgroup_op(const Instruction &i);
	std::string to_texture_op(const Instruction &i, bool sparse, bool *forward,
	                          SmallVector<uint32_t> &inherited_expressions);
	void emit_fixup();
	std::string to_struct_member(const SPIRType &type, uint32_t member_type_id, uint32_t index,
	                             const std::string &qualifier = "");
	void emit_struct_member(const SPIRType &type, uint32_t member_type_id, uint32_t index,
	                        const std::string &qualifier = "", uint32_t base_offset = 0);
	void emit_struct_padding_target(const SPIRType &type);
	std::string type_to_glsl(const SPIRType &type, uint32_t id, bool member);
	std::string type_to_glsl(const SPIRType &type, uint32_t id = 0);
	void emit_block_hints(const SPIRBlock &block);

	// Allow Metal to use the array<T> template to make arrays a value type
	std::string type_to_array_glsl(const SPIRType &type);
	std::string constant_op_expression(const SPIRConstantOp &cop);

	// Threadgroup arrays can't have a wrapper type
	std::string variable_decl(const SPIRVariable &variable);

	bool variable_decl_is_remapped_storage(const SPIRVariable &variable, spv::StorageClass storage) const;

	// GCC workaround of lambdas calling protected functions (for older GCC versions)
	std::string variable_decl(const SPIRType &type, const std::string &name, uint32_t id = 0);

	std::string image_type_glsl(const SPIRType &type, uint32_t id = 0);
	std::string sampler_type(const SPIRType &type, uint32_t id);
	std::string builtin_to_glsl(spv::BuiltIn builtin, spv::StorageClass storage);
	std::string to_func_call_arg(const SPIRFunction::Parameter &arg, uint32_t id);
	std::string to_name(uint32_t id, bool allow_alias = true) const override;
	std::string to_function_name(const TextureFunctionNameArguments &args);
	std::string to_function_args(const TextureFunctionArguments &args, bool *p_forward);
	std::string to_initializer_expression(const SPIRVariable &var);
	std::string to_zero_initialized_expression(uint32_t type_id);

	std::string unpack_expression_type(std::string expr_str, const SPIRType &type, uint32_t physical_type_id,
	                                   bool is_packed, bool row_major);

	// Returns true for BuiltInSampleMask because gl_SampleMask[] is an array in SPIR-V, but [[sample_mask]] is a scalar in Metal.
	bool builtin_translates_to_nonarray(spv::BuiltIn builtin) const;

	std::string bitcast_glsl_op(const SPIRType &result_type, const SPIRType &argument_type);
	bool emit_complex_bitcast(uint32_t result_id, uint32_t id, uint32_t op0);
	bool skip_argument(uint32_t id) const;
	std::string to_member_reference(uint32_t base, const SPIRType &type, uint32_t index, bool ptr_chain_is_resolved);
	std::string to_qualifiers_glsl(uint32_t id);
	void replace_illegal_names();
	void declare_constant_arrays();

	void replace_illegal_entry_point_names();
	void sync_entry_point_aliases_and_names();

	static const std::unordered_set<std::string> &get_reserved_keyword_set();
	static const std::unordered_set<std::string> &get_illegal_func_names();

	// Constant arrays of non-primitive types (i.e. matrices) won't link properly into Metal libraries
	void declare_complex_constant_arrays();

	bool is_patch_block(const SPIRType &type);
	bool is_non_native_row_major_matrix(uint32_t id);
	bool member_is_non_native_row_major_matrix(const SPIRType &type, uint32_t index);
	std::string convert_row_major_matrix(std::string exp_str, const SPIRType &exp_type, uint32_t physical_type_id,
	                                     bool is_packed, bool relaxed);

	bool is_tesc_shader() const;
	bool is_tese_shader() const;

	void preprocess_op_codes();
	void localize_global_variables();
	void extract_global_variables_from_functions();
	void mark_packable_structs();
	void mark_as_packable(SPIRType &type);
	void mark_as_workgroup_struct(SPIRType &type);

	std::unordered_map<uint32_t, std::set<uint32_t>> function_global_vars;
	void extract_global_variables_from_function(uint32_t func_id, std::set<uint32_t> &added_arg_ids,
	                                            std::unordered_set<uint32_t> &global_var_ids,
	                                            std::unordered_set<uint32_t> &processed_func_ids);
	uint32_t add_interface_block(spv::StorageClass storage, bool patch = false);
	uint32_t add_interface_block_pointer(uint32_t ib_var_id, spv::StorageClass storage);

	struct InterfaceBlockMeta
	{
		struct LocationMeta
		{
			uint32_t base_type_id = 0;
			uint32_t num_components = 0;
			bool flat = false;
			bool noperspective = false;
			bool centroid = false;
			bool sample = false;
		};
		std::unordered_map<uint32_t, LocationMeta> location_meta;
		bool strip_array = false;
		bool allow_local_declaration = false;
	};

	std::string to_tesc_invocation_id();
	void emit_local_masked_variable(const SPIRVariable &masked_var, bool strip_array);
	void add_variable_to_interface_block(spv::StorageClass storage, const std::string &ib_var_ref, SPIRType &ib_type,
	                                     SPIRVariable &var, InterfaceBlockMeta &meta);
	void add_composite_variable_to_interface_block(spv::StorageClass storage, const std::string &ib_var_ref,
	                                               SPIRType &ib_type, SPIRVariable &var, InterfaceBlockMeta &meta);
	void add_plain_variable_to_interface_block(spv::StorageClass storage, const std::string &ib_var_ref,
	                                           SPIRType &ib_type, SPIRVariable &var, InterfaceBlockMeta &meta);
	bool add_component_variable_to_interface_block(spv::StorageClass storage, const std::string &ib_var_ref,
	                                               SPIRVariable &var, const SPIRType &type,
	                                               InterfaceBlockMeta &meta);
	void add_plain_member_variable_to_interface_block(spv::StorageClass storage,
	                                                  const std::string &ib_var_ref, SPIRType &ib_type,
	                                                  SPIRVariable &var, SPIRType &var_type,
	                                                  uint32_t mbr_idx, InterfaceBlockMeta &meta,
	                                                  const std::string &mbr_name_qual,
	                                                  const std::string &var_chain_qual,
	                                                  uint32_t &location, uint32_t &var_mbr_idx);
	void add_composite_member_variable_to_interface_block(spv::StorageClass storage,
	                                                      const std::string &ib_var_ref, SPIRType &ib_type,
	                                                      SPIRVariable &var, SPIRType &var_type,
	                                                      uint32_t mbr_idx, InterfaceBlockMeta &meta,
	                                                      const std::string &mbr_name_qual,
	                                                      const std::string &var_chain_qual,
	                                                      uint32_t &location, uint32_t &var_mbr_idx);
	void add_tess_level_input_to_interface_block(const std::string &ib_var_ref, SPIRType &ib_type, SPIRVariable &var);
	void add_tess_level_input(const std::string &base_ref, const std::string &mbr_name, SPIRVariable &var);

	void fix_up_interface_member_indices(spv::StorageClass storage, uint32_t ib_type_id);

	void mark_location_as_used_by_shader(uint32_t location, const SPIRType &type,
	                                     spv::StorageClass storage, bool fallback = false);
	uint32_t ensure_correct_builtin_type(uint32_t type_id, spv::BuiltIn builtin);
	uint32_t ensure_correct_input_type(uint32_t type_id, uint32_t location, uint32_t component,
	                                   uint32_t num_components, bool strip_array);

	void emit_custom_templates();
	void emit_custom_functions();
	void emit_resources();
	void emit_specialization_constants_and_structs();
	void emit_interface_block(uint32_t ib_var_id);
	bool maybe_emit_array_assignment(uint32_t id_lhs, uint32_t id_rhs);
	uint32_t get_resource_array_size(uint32_t id) const;

	void fix_up_shader_inputs_outputs();

	std::string func_type_decl(SPIRType &type);
	std::string entry_point_args_classic(bool append_comma);
	std::string entry_point_args_argument_buffer(bool append_comma);
	std::string entry_point_arg_stage_in();
	void entry_point_args_builtin(std::string &args);
	void entry_point_args_discrete_descriptors(std::string &args);
	std::string append_member_name(const std::string &qualifier, const SPIRType &type, uint32_t index);
	std::string ensure_valid_name(std::string name, std::string pfx);
	std::string to_sampler_expression(uint32_t id);
	std::string to_swizzle_expression(uint32_t id);
	std::string to_buffer_size_expression(uint32_t id);
	bool is_sample_rate() const;
	bool is_intersection_query() const;
	bool is_direct_input_builtin(spv::BuiltIn builtin);
	std::string builtin_qualifier(spv::BuiltIn builtin);
	std::string builtin_type_decl(spv::BuiltIn builtin, uint32_t id = 0);
	std::string built_in_func_arg(spv::BuiltIn builtin, bool prefix_comma);
	std::string member_attribute_qualifier(const SPIRType &type, uint32_t index);
	std::string member_location_attribute_qualifier(const SPIRType &type, uint32_t index);
	std::string argument_decl(const SPIRFunction::Parameter &arg);
	const char *descriptor_address_space(uint32_t id, spv::StorageClass storage, const char *plain_address_space) const;
	std::string round_fp_tex_coords(std::string tex_coords, bool coord_is_fp);
	uint32_t get_metal_resource_index(SPIRVariable &var, SPIRType::BaseType basetype, uint32_t plane = 0);
	uint32_t get_member_location(uint32_t type_id, uint32_t index, uint32_t *comp = nullptr) const;
	uint32_t get_or_allocate_builtin_input_member_location(spv::BuiltIn builtin,
	                                                       uint32_t type_id, uint32_t index, uint32_t *comp = nullptr);
	uint32_t get_or_allocate_builtin_output_member_location(spv::BuiltIn builtin,
	                                                        uint32_t type_id, uint32_t index, uint32_t *comp = nullptr);

	uint32_t get_physical_tess_level_array_size(spv::BuiltIn builtin) const;

	// MSL packing rules. These compute the effective packing rules as observed by the MSL compiler in the MSL output.
	// These values can change depending on various extended decorations which control packing rules.
	// We need to make these rules match up with SPIR-V declared rules.
	uint32_t get_declared_type_size_msl(const SPIRType &type, bool packed, bool row_major) const;
	uint32_t get_declared_type_array_stride_msl(const SPIRType &type, bool packed, bool row_major) const;
	uint32_t get_declared_type_matrix_stride_msl(const SPIRType &type, bool packed, bool row_major) const;
	uint32_t get_declared_type_alignment_msl(const SPIRType &type, bool packed, bool row_major) const;

	uint32_t get_declared_struct_member_size_msl(const SPIRType &struct_type, uint32_t index) const;
	uint32_t get_declared_struct_member_array_stride_msl(const SPIRType &struct_type, uint32_t index) const;
	uint32_t get_declared_struct_member_matrix_stride_msl(const SPIRType &struct_type, uint32_t index) const;
	uint32_t get_declared_struct_member_alignment_msl(const SPIRType &struct_type, uint32_t index) const;

	uint32_t get_declared_input_size_msl(const SPIRType &struct_type, uint32_t index) const;
	uint32_t get_declared_input_array_stride_msl(const SPIRType &struct_type, uint32_t index) const;
	uint32_t get_declared_input_matrix_stride_msl(const SPIRType &struct_type, uint32_t index) const;
	uint32_t get_declared_input_alignment_msl(const SPIRType &struct_type, uint32_t index) const;

	const SPIRType &get_physical_member_type(const SPIRType &struct_type, uint32_t index) const;
	SPIRType get_presumed_input_type(const SPIRType &struct_type, uint32_t index) const;

	uint32_t get_declared_struct_size_msl(const SPIRType &struct_type, bool ignore_alignment = false,
	                                      bool ignore_padding = false) const;

	std::string to_component_argument(uint32_t id);
	void align_struct(SPIRType &ib_type, std::unordered_set<uint32_t> &aligned_structs);
	void mark_scalar_layout_structs(const SPIRType &ib_type);
	void mark_struct_members_packed(const SPIRType &type);
	void ensure_member_packing_rules_msl(SPIRType &ib_type, uint32_t index);
	bool validate_member_packing_rules_msl(const SPIRType &type, uint32_t index) const;
	std::string get_argument_address_space(const SPIRVariable &argument);
	std::string get_type_address_space(const SPIRType &type, uint32_t id, bool argument = false);
	const char *to_restrict(uint32_t id, bool space);
	SPIRType &get_stage_in_struct_type();
	SPIRType &get_stage_out_struct_type();
	SPIRType &get_patch_stage_in_struct_type();
	SPIRType &get_patch_stage_out_struct_type();
	std::string get_tess_factor_struct_name();
	SPIRType &get_uint_type();
	uint32_t get_uint_type_id();
	void emit_atomic_func_op(uint32_t result_type, uint32_t result_id, const char *op, spv::Op opcode,
	                         uint32_t mem_order_1, uint32_t mem_order_2, bool has_mem_order_2, uint32_t op0, uint32_t op1 = 0,
	                         bool op1_is_pointer = false, bool op1_is_literal = false, uint32_t op2 = 0);
	const char *get_memory_order(uint32_t spv_mem_sem);
	void add_pragma_line(const std::string &line);
	void add_typedef_line(const std::string &line);
	void emit_barrier(uint32_t id_exe_scope, uint32_t id_mem_scope, uint32_t id_mem_sem);
	bool emit_array_copy(const char *expr, uint32_t lhs_id, uint32_t rhs_id,
	                     spv::StorageClass lhs_storage, spv::StorageClass rhs_storage);
	void build_implicit_builtins();
	uint32_t build_constant_uint_array_pointer();
	void emit_entry_point_declarations();
	bool uses_explicit_early_fragment_test();

	uint32_t builtin_frag_coord_id = 0;
	uint32_t builtin_sample_id_id = 0;
	uint32_t builtin_sample_mask_id = 0;
	uint32_t builtin_helper_invocation_id = 0;
	uint32_t builtin_vertex_idx_id = 0;
	uint32_t builtin_base_vertex_id = 0;
	uint32_t builtin_instance_idx_id = 0;
	uint32_t builtin_base_instance_id = 0;
	uint32_t builtin_view_idx_id = 0;
	uint32_t builtin_layer_id = 0;
	uint32_t builtin_invocation_id_id = 0;
	uint32_t builtin_primitive_id_id = 0;
	uint32_t builtin_subgroup_invocation_id_id = 0;
	uint32_t builtin_subgroup_size_id = 0;
	uint32_t builtin_dispatch_base_id = 0;
	uint32_t builtin_stage_input_size_id = 0;
	uint32_t builtin_local_invocation_index_id = 0;
	uint32_t builtin_workgroup_size_id = 0;
	uint32_t swizzle_buffer_id = 0;
	uint32_t buffer_size_buffer_id = 0;
	uint32_t view_mask_buffer_id = 0;
	uint32_t dynamic_offsets_buffer_id = 0;
	uint32_t uint_type_id = 0;
	uint32_t argument_buffer_padding_buffer_type_id = 0;
	uint32_t argument_buffer_padding_image_type_id = 0;
	uint32_t argument_buffer_padding_sampler_type_id = 0;

	bool does_shader_write_sample_mask = false;
	bool frag_shader_needs_discard_checks = false;

	void cast_to_variable_store(uint32_t target_id, std::string &expr, const SPIRType &expr_type);
	void cast_from_variable_load(uint32_t source_id, std::string &expr, const SPIRType &expr_type);
	void emit_store_statement(uint32_t lhs_expression, uint32_t rhs_expression);

	void analyze_sampled_image_usage();

	bool access_chain_needs_stage_io_builtin_translation(uint32_t base);
	void prepare_access_chain_for_scalar_access(std::string &expr, const SPIRType &type, spv::StorageClass storage,
	                                            bool &is_packed);
	void fix_up_interpolant_access_chain(const uint32_t *ops, uint32_t length);
	void check_physical_type_cast(std::string &expr, const SPIRType *type, uint32_t physical_type);

	bool emit_tessellation_access_chain(const uint32_t *ops, uint32_t length);
	bool emit_tessellation_io_load(uint32_t result_type, uint32_t id, uint32_t ptr);
	bool is_out_of_bounds_tessellation_level(uint32_t id_lhs);

	void ensure_builtin(spv::StorageClass storage, spv::BuiltIn builtin);

	void mark_implicit_builtin(spv::StorageClass storage, spv::BuiltIn builtin, uint32_t id);

	std::string convert_to_f32(const std::string &expr, uint32_t components);

	Options msl_options;
	std::set<SPVFuncImpl> spv_function_implementations;
	// Must be ordered to ensure declarations are in a specific order.
	std::map<LocationComponentPair, MSLShaderInterfaceVariable> inputs_by_location;
	std::unordered_map<uint32_t, MSLShaderInterfaceVariable> inputs_by_builtin;
	std::map<LocationComponentPair, MSLShaderInterfaceVariable> outputs_by_location;
	std::unordered_map<uint32_t, MSLShaderInterfaceVariable> outputs_by_builtin;
	std::unordered_set<uint32_t> location_inputs_in_use;
	std::unordered_set<uint32_t> location_inputs_in_use_fallback;
	std::unordered_set<uint32_t> location_outputs_in_use;
	std::unordered_set<uint32_t> location_outputs_in_use_fallback;
	std::unordered_map<uint32_t, uint32_t> fragment_output_components;
	std::unordered_map<uint32_t, uint32_t> builtin_to_automatic_input_location;
	std::unordered_map<uint32_t, uint32_t> builtin_to_automatic_output_location;
	std::set<std::string> pragma_lines;
	std::set<std::string> typedef_lines;
	SmallVector<uint32_t> vars_needing_early_declaration;

	std::unordered_map<StageSetBinding, std::pair<MSLResourceBinding, bool>, InternalHasher> resource_bindings;
	std::unordered_map<StageSetBinding, uint32_t, InternalHasher> resource_arg_buff_idx_to_binding_number;

	uint32_t next_metal_resource_index_buffer = 0;
	uint32_t next_metal_resource_index_texture = 0;
	uint32_t next_metal_resource_index_sampler = 0;
	// Intentionally uninitialized, works around MSVC 2013 bug.
	uint32_t next_metal_resource_ids[kMaxArgumentBuffers];

	VariableID stage_in_var_id = 0;
	VariableID stage_out_var_id = 0;
	VariableID patch_stage_in_var_id = 0;
	VariableID patch_stage_out_var_id = 0;
	VariableID stage_in_ptr_var_id = 0;
	VariableID stage_out_ptr_var_id = 0;
	VariableID tess_level_inner_var_id = 0;
	VariableID tess_level_outer_var_id = 0;
	VariableID stage_out_masked_builtin_type_id = 0;

	// Handle HLSL-style 0-based vertex/instance index.
	enum class TriState
	{
		Neutral,
		No,
		Yes
	};
	TriState needs_base_vertex_arg = TriState::Neutral;
	TriState needs_base_instance_arg = TriState::Neutral;

	bool has_sampled_images = false;
	bool builtin_declaration = false; // Handle HLSL-style 0-based vertex/instance index.

	bool is_using_builtin_array = false; // Force the use of C style array declaration.
	bool using_builtin_array() const;

	bool is_rasterization_disabled = false;
	bool capture_output_to_buffer = false;
	bool needs_swizzle_buffer_def = false;
	bool used_swizzle_buffer = false;
	bool added_builtin_tess_level = false;
	bool needs_subgroup_invocation_id = false;
	bool needs_subgroup_size = false;
	bool needs_sample_id = false;
	bool needs_helper_invocation = false;
	std::string qual_pos_var_name;
	std::string stage_in_var_name = "in";
	std::string stage_out_var_name = "out";
	std::string patch_stage_in_var_name = "patchIn";
	std::string patch_stage_out_var_name = "patchOut";
	std::string sampler_name_suffix = "Smplr";
	std::string swizzle_name_suffix = "Swzl";
	std::string buffer_size_name_suffix = "BufferSize";
	std::string plane_name_suffix = "Plane";
	std::string input_wg_var_name = "gl_in";
	std::string input_buffer_var_name = "spvIn";
	std::string output_buffer_var_name = "spvOut";
	std::string patch_input_buffer_var_name = "spvPatchIn";
	std::string patch_output_buffer_var_name = "spvPatchOut";
	std::string tess_factor_buffer_var_name = "spvTessLevel";
	std::string index_buffer_var_name = "spvIndices";
	spv::Op previous_instruction_opcode = spv::OpNop;

	// Must be ordered since declaration is in a specific order.
	std::map<uint32_t, MSLConstexprSampler> constexpr_samplers_by_id;
	std::unordered_map<SetBindingPair, MSLConstexprSampler, InternalHasher> constexpr_samplers_by_binding;
	const MSLConstexprSampler *find_constexpr_sampler(uint32_t id) const;

	std::unordered_set<uint32_t> buffers_requiring_array_length;
	SmallVector<std::pair<uint32_t, uint32_t>> buffer_aliases_argument;
	SmallVector<uint32_t> buffer_aliases_discrete;
	std::unordered_set<uint32_t> atomic_image_vars; // Emulate texture2D atomic operations
	std::unordered_set<uint32_t> pull_model_inputs;

	SmallVector<SPIRVariable *> entry_point_bindings;

	// Must be ordered since array is in a specific order.
	std::map<SetBindingPair, std::pair<uint32_t, uint32_t>> buffers_requiring_dynamic_offset;

	SmallVector<uint32_t> disabled_frag_outputs;

	std::unordered_set<SetBindingPair, InternalHasher> inline_uniform_blocks;

	uint32_t argument_buffer_ids[kMaxArgumentBuffers];
	uint32_t argument_buffer_discrete_mask = 0;
	uint32_t argument_buffer_device_storage_mask = 0;

	void analyze_argument_buffers();
	bool descriptor_set_is_argument_buffer(uint32_t desc_set) const;
	MSLResourceBinding &get_argument_buffer_resource(uint32_t desc_set, uint32_t arg_idx);
	void add_argument_buffer_padding_buffer_type(SPIRType &struct_type, uint32_t &mbr_idx, uint32_t &arg_buff_index, MSLResourceBinding &rez_bind);
	void add_argument_buffer_padding_image_type(SPIRType &struct_type, uint32_t &mbr_idx, uint32_t &arg_buff_index, MSLResourceBinding &rez_bind);
	void add_argument_buffer_padding_sampler_type(SPIRType &struct_type, uint32_t &mbr_idx, uint32_t &arg_buff_index, MSLResourceBinding &rez_bind);
	void add_argument_buffer_padding_type(uint32_t mbr_type_id, SPIRType &struct_type, uint32_t &mbr_idx, uint32_t &arg_buff_index, uint32_t count);

	uint32_t get_target_components_for_fragment_location(uint32_t location) const;
	uint32_t build_extended_vector_type(uint32_t type_id, uint32_t components,
	                                    SPIRType::BaseType basetype = SPIRType::Unknown);
	uint32_t build_msl_interpolant_type(uint32_t type_id, bool is_noperspective);

	bool suppress_missing_prototypes = false;

	void add_spv_func_and_recompile(SPVFuncImpl spv_func);

	void activate_argument_buffer_resources();

	bool type_is_msl_framebuffer_fetch(const SPIRType &type) const;
	bool type_is_pointer(const SPIRType &type) const;
	bool type_is_pointer_to_pointer(const SPIRType &type) const;
	bool is_supported_argument_buffer_type(const SPIRType &type) const;

	bool variable_storage_requires_stage_io(spv::StorageClass storage) const;

	bool needs_manual_helper_invocation_updates() const
	{
		return msl_options.manual_helper_invocation_updates && msl_options.supports_msl_version(2, 3);
	}
	bool needs_frag_discard_checks() const
	{
		return get_execution_model() == spv::ExecutionModelFragment && msl_options.supports_msl_version(2, 3) &&
		       msl_options.check_discarded_frag_stores && frag_shader_needs_discard_checks;
	}

	bool has_additional_fixed_sample_mask() const { return msl_options.additional_fixed_sample_mask != 0xffffffff; }
	std::string additional_fixed_sample_mask_str() const;

	// OpcodeHandler that handles several MSL preprocessing operations.
	struct OpCodePreprocessor : OpcodeHandler
	{
		OpCodePreprocessor(CompilerMSL &compiler_)
		    : compiler(compiler_)
		{
		}

		bool handle(spv::Op opcode, const uint32_t *args, uint32_t length) override;
		CompilerMSL::SPVFuncImpl get_spv_func_impl(spv::Op opcode, const uint32_t *args);
		void check_resource_write(uint32_t var_id);

		CompilerMSL &compiler;
		std::unordered_map<uint32_t, uint32_t> result_types;
		std::unordered_map<uint32_t, uint32_t> image_pointers; // Emulate texture2D atomic operations
		bool suppress_missing_prototypes = false;
		bool uses_atomics = false;
		bool uses_image_write = false;
		bool uses_buffer_write = false;
		bool uses_discard = false;
		bool needs_subgroup_invocation_id = false;
		bool needs_subgroup_size = false;
		bool needs_sample_id = false;
		bool needs_helper_invocation = false;
	};

	// OpcodeHandler that scans for uses of sampled images
	struct SampledImageScanner : OpcodeHandler
	{
		SampledImageScanner(CompilerMSL &compiler_)
		    : compiler(compiler_)
		{
		}

		bool handle(spv::Op opcode, const uint32_t *args, uint32_t) override;

		CompilerMSL &compiler;
	};

	// Sorts the members of a SPIRType and associated Meta info based on a settable sorting
	// aspect, which defines which aspect of the struct members will be used to sort them.
	// Regardless of the sorting aspect, built-in members always appear at the end of the struct.
	struct MemberSorter
	{
		enum SortAspect
		{
			LocationThenBuiltInType,
			Offset
		};

		void sort();
		bool operator()(uint32_t mbr_idx1, uint32_t mbr_idx2);
		MemberSorter(SPIRType &t, Meta &m, SortAspect sa);

		SPIRType &type;
		Meta &meta;
		SortAspect sort_aspect;
	};

	// FROM GLSL
	struct GLSLOptions
	{
		// The shading language version. Corresponds to #version $VALUE.
		uint32_t version = 450;

		// Emit the OpenGL ES shading language instead of desktop OpenGL.
		bool es = false;

		// Debug option to always emit temporary variables for all expressions.
		bool force_temporary = false;
		// Debug option, can be increased in an attempt to workaround SPIRV-Cross bugs temporarily.
		// If this limit has to be increased, it points to an implementation bug.
		// In certain scenarios, the maximum number of debug iterations may increase beyond this limit
		// as long as we can prove we're making certain kinds of forward progress.
		uint32_t force_recompile_max_debug_iterations = 3;

		// If true, Vulkan GLSL features are used instead of GL-compatible features.
		// Mostly useful for debugging SPIR-V files.
		bool vulkan_semantics = false;

		// If true, gl_PerVertex is explicitly redeclared in vertex, geometry and tessellation shaders.
		// The members of gl_PerVertex is determined by which built-ins are declared by the shader.
		// This option is ignored in ES versions, as redeclaration in ES is not required, and it depends on a different extension
		// (EXT_shader_io_blocks) which makes things a bit more fuzzy.
		bool separate_shader_objects = false;

		// Flattens multidimensional arrays, e.g. float foo[a][b][c] into single-dimensional arrays,
		// e.g. float foo[a * b * c].
		// This function does not change the actual SPIRType of any object.
		// Only the generated code, including declarations of interface variables are changed to be single array dimension.
		bool flatten_multidimensional_arrays = false;

		// For older desktop GLSL targets than version 420, the
		// GL_ARB_shading_language_420pack extensions is used to be able to support
		// layout(binding) on UBOs and samplers.
		// If disabled on older targets, binding decorations will be stripped.
		bool enable_420pack_extension = true;

		// In non-Vulkan GLSL, emit push constant blocks as UBOs rather than plain uniforms.
		bool emit_push_constant_as_uniform_buffer = false;

		// Always emit uniform blocks as plain uniforms, regardless of the GLSL version, even when UBOs are supported.
		// Does not apply to shader storage or push constant blocks.
		bool emit_uniform_buffer_as_plain_uniforms = false;

		// Emit OpLine directives if present in the module.
		// May not correspond exactly to original source, but should be a good approximation.
		bool emit_line_directives = false;

		// In cases where readonly/writeonly decoration are not used at all,
		// we try to deduce which qualifier(s) we should actually used, since actually emitting
		// read-write decoration is very rare, and older glslang/HLSL compilers tend to just emit readwrite as a matter of fact.
		// The default (true) is to enable automatic deduction for these cases, but if you trust the decorations set
		// by the SPIR-V, it's recommended to set this to false.
		bool enable_storage_image_qualifier_deduction = true;

		// On some targets (WebGPU), uninitialized variables are banned.
		// If this is enabled, all variables (temporaries, Private, Function)
		// which would otherwise be uninitialized will now be initialized to 0 instead.
		bool force_zero_initialized_variables = false;

		// In GLSL, force use of I/O block flattening, similar to
		// what happens on legacy GLSL targets for blocks and structs.
		bool force_flattened_io_blocks = false;

		// For opcodes where we have to perform explicit additional nan checks, very ugly code is generated.
		// If we opt-in, ignore these requirements.
		// In opcodes like NClamp/NMin/NMax and FP compare, ignore NaN behavior.
		// Use FClamp/FMin/FMax semantics for clamps and lets implementation choose ordered or unordered
		// compares.
		bool relax_nan_checks = false;

		// Loading row-major matrices from UBOs on older AMD Windows OpenGL drivers is problematic.
		// To load these types correctly, we must generate a wrapper. them in a dummy function which only purpose is to
		// ensure row_major decoration is actually respected.
		// This workaround may cause significant performance degeneration on some Android devices.
		bool enable_row_major_load_workaround = true;

		// If non-zero, controls layout(num_views = N) in; in GL_OVR_multiview2.
		uint32_t ovr_multiview_view_count = 0;

		enum Precision
		{
			DontCare,
			Lowp,
			Mediump,
			Highp
		};

		struct VertexOptions
		{
			// "Vertex-like shader" here is any shader stage that can write BuiltInPosition.

			// GLSL: In vertex-like shaders, rewrite [0, w] depth (Vulkan/D3D style) to [-w, w] depth (GL style).
			// MSL: In vertex-like shaders, rewrite [-w, w] depth (GL style) to [0, w] depth.
			// HLSL: In vertex-like shaders, rewrite [-w, w] depth (GL style) to [0, w] depth.
			bool fixup_clipspace = false;

			// In vertex-like shaders, inverts gl_Position.y or equivalent.
			bool flip_vert_y = false;

			// GLSL only, for HLSL version of this option, see CompilerHLSL.
			// If true, the backend will assume that InstanceIndex will need to apply
			// a base instance offset. Set to false if you know you will never use base instance
			// functionality as it might remove some internal uniforms.
			bool support_nonzero_base_instance = true;
		} vertex;

		struct FragmentOptions
		{
			// Add precision mediump float in ES targets when emitting GLES source.
			// Add precision highp int in ES targets when emitting GLES source.
			Precision default_float_precision = Mediump;
			Precision default_int_precision = Highp;
		} fragment;
	};

	GLSLOptions options;

	struct ShaderSubgroupSupportHelper
	{
		// lower enum value = greater priority
		enum Candidate
		{
			KHR_shader_subgroup_ballot,
			KHR_shader_subgroup_basic,
			KHR_shader_subgroup_vote,
			KHR_shader_subgroup_arithmetic,
			NV_gpu_shader_5,
			NV_shader_thread_group,
			NV_shader_thread_shuffle,
			ARB_shader_ballot,
			ARB_shader_group_vote,
			AMD_gcn_shader,

			CandidateCount
		};

		static const char *get_extension_name(Candidate c);
		static SmallVector<std::string> get_extra_required_extension_names(Candidate c);
		static const char *get_extra_required_extension_predicate(Candidate c);

		enum Feature
		{
			SubgroupMask = 0,
			SubgroupSize = 1,
			SubgroupInvocationID = 2,
			SubgroupID = 3,
			NumSubgroups = 4,
			SubgroupBroadcast_First = 5,
			SubgroupBallotFindLSB_MSB = 6,
			SubgroupAll_Any_AllEqualBool = 7,
			SubgroupAllEqualT = 8,
			SubgroupElect = 9,
			SubgroupBarrier = 10,
			SubgroupMemBarrier = 11,
			SubgroupBallot = 12,
			SubgroupInverseBallot_InclBitCount_ExclBitCout = 13,
			SubgroupBallotBitExtract = 14,
			SubgroupBallotBitCount = 15,
			SubgroupArithmeticIAddReduce = 16,
			SubgroupArithmeticIAddExclusiveScan = 17,
			SubgroupArithmeticIAddInclusiveScan = 18,
			SubgroupArithmeticFAddReduce = 19,
			SubgroupArithmeticFAddExclusiveScan = 20,
			SubgroupArithmeticFAddInclusiveScan = 21,
			SubgroupArithmeticIMulReduce = 22,
			SubgroupArithmeticIMulExclusiveScan = 23,
			SubgroupArithmeticIMulInclusiveScan = 24,
			SubgroupArithmeticFMulReduce = 25,
			SubgroupArithmeticFMulExclusiveScan = 26,
			SubgroupArithmeticFMulInclusiveScan = 27,
			FeatureCount
		};

		using FeatureMask = uint32_t;
		static_assert(sizeof(FeatureMask) * 8u >= FeatureCount, "Mask type needs more bits.");

		using CandidateVector = SmallVector<Candidate, CandidateCount>;
		using FeatureVector = SmallVector<Feature>;

		static FeatureVector get_feature_dependencies(Feature feature);
		static FeatureMask get_feature_dependency_mask(Feature feature);
		static bool can_feature_be_implemented_without_extensions(Feature feature);
		static Candidate get_KHR_extension_for_feature(Feature feature);

		struct Result
		{
			Result();
			uint32_t weights[CandidateCount];
		};

		void request_feature(Feature feature);
		bool is_feature_requested(Feature feature) const;
		Result resolve() const;

		static CandidateVector get_candidates_for_feature(Feature ft, const Result &r);

	private:
		static CandidateVector get_candidates_for_feature(Feature ft);
		static FeatureMask build_mask(const SmallVector<Feature> &features);
		FeatureMask feature_mask = 0;
	};


	struct BackendVariations
	{
		std::string discard_literal = "discard";
		std::string demote_literal = "demote";
		std::string null_pointer_literal = "";
		bool float_literal_suffix = false;
		bool double_literal_suffix = true;
		bool uint32_t_literal_suffix = true;
		bool long_long_literal_suffix = false;
		const char *basic_int_type = "int";
		const char *basic_uint_type = "uint";
		const char *basic_int8_type = "int8_t";
		const char *basic_uint8_type = "uint8_t";
		const char *basic_int16_type = "int16_t";
		const char *basic_uint16_type = "uint16_t";
		const char *int16_t_literal_suffix = "s";
		const char *uint16_t_literal_suffix = "us";
		const char *nonuniform_qualifier = "nonuniformEXT";
		const char *boolean_mix_function = "mix";
		SPIRType::BaseType boolean_in_struct_remapped_type = SPIRType::Boolean;
		bool swizzle_is_function = false;
		bool shared_is_implied = false;
		bool unsized_array_supported = true;
		bool explicit_struct_type = false;
		bool use_initializer_list = false;
		bool use_typed_initializer_list = false;
		bool can_declare_struct_inline = true;
		bool can_declare_arrays_inline = true;
		bool native_row_major_matrix = true;
		bool use_constructor_splatting = true;
		bool allow_precision_qualifiers = false;
		bool can_swizzle_scalar = false;
		bool force_gl_in_out_block = false;
		bool force_merged_mesh_block = false;
		bool can_return_array = true;
		bool allow_truncated_access_chain = false;
		bool supports_extensions = false;
		bool supports_empty_struct = false;
		bool array_is_value_type = true;
		bool array_is_value_type_in_buffer_blocks = true;
		bool comparison_image_samples_scalar = false;
		bool native_pointers = false;
		bool support_small_type_sampling_result = false;
		bool support_case_fallthrough = true;
		bool use_array_constructor = false;
		bool needs_row_major_load_workaround = false;
		bool support_pointer_to_pointer = false;
		bool support_precise_qualifier = false;
		bool support_64bit_switch = false;
		bool workgroup_size_is_hidden = false;
		bool requires_relaxed_precision_analysis = false;
		bool implicit_c_integer_promotion_rules = false;
	} backend;

	enum AccessChainFlagBits
	{
		ACCESS_CHAIN_INDEX_IS_LITERAL_BIT = 1 << 0,
		ACCESS_CHAIN_CHAIN_ONLY_BIT = 1 << 1,
		ACCESS_CHAIN_PTR_CHAIN_BIT = 1 << 2,
		ACCESS_CHAIN_SKIP_REGISTER_EXPRESSION_READ_BIT = 1 << 3,
		ACCESS_CHAIN_LITERAL_MSB_FORCE_ID = 1 << 4,
		ACCESS_CHAIN_FLATTEN_ALL_MEMBERS_BIT = 1 << 5,
		ACCESS_CHAIN_FORCE_COMPOSITE_BIT = 1 << 6
	};
	typedef uint32_t AccessChainFlags;

	SmallVector<std::string> header_lines;
	SmallVector<std::string> forced_extensions;
	std::unordered_map<uint32_t, uint32_t> extra_sub_expressions;
	std::unordered_set<std::string> local_variable_names;
	std::unordered_set<std::string> resource_names;
	std::unordered_set<uint32_t> composite_insert_overwritten;
	std::unordered_map<uint32_t, uint32_t> expression_usage_counts;
	std::unordered_set<uint32_t> flushed_phi_variables;
	std::unordered_set<uint32_t> block_composite_insert_overwrite;
	std::unordered_map<uint32_t, bool> flattened_structs;
	std::unordered_set<uint32_t> masked_output_builtins;
	std::unordered_set<uint32_t> flattened_buffer_blocks;
	SmallVector<SPIRBlock *> current_emitting_switch_stack;
	std::unordered_map<std::string, std::unordered_set<uint64_t>> function_overloads;
	std::unordered_map<uint32_t, uint32_t> temporary_to_mirror_precision_alias;
	std::unordered_map<uint32_t, SmallVector<ConstantID>> const_composite_insert_ids;
	std::unordered_set<std::string> block_names; // A union of all block_*_names.
	std::unordered_map<uint32_t, std::string> preserved_aliases;
	std::unordered_set<std::string> block_input_names;
	std::unordered_set<std::string> block_output_names;
	std::unordered_set<std::string> block_ubo_names;
	std::unordered_set<std::string> block_ssbo_names;
	std::unordered_set<LocationComponentPair, InternalHasher> masked_output_locations;
	std::vector<std::pair<uint32_t, uint32_t>> subpass_to_framebuffer_fetch_attachment;
	
	bool processing_entry_point = false;
	bool block_debug_directives = false;
	const SPIRBlock *current_continue_block = nullptr;
	bool block_temporary_hoisting = false;
	char current_locale_radix_character = '.';
	bool current_emitting_switch_fallthrough = false;
	bool ray_tracing_is_khr = false;
	bool barycentric_is_nv = false;
	uint32_t statement_count = 0;
	StringStream<> buffer;
	SPIRBlock *current_emitting_block = nullptr;
	SmallVector<std::string> *redirect_statement = nullptr;
	uint32_t indent = 0;
	template <typename T>
	inline void statement_inner(T &&t)
	{
		buffer << std::forward<T>(t);
		statement_count++;
	}

	template <typename T, typename... Ts>
	inline void statement_inner(T &&t, Ts &&... ts)
	{
		buffer << std::forward<T>(t);
		statement_count++;
		statement_inner(std::forward<Ts>(ts)...);
	}

	template <typename... Ts>
	inline void statement(Ts &&... ts)
	{
		if (is_forcing_recompilation())
		{
			// Do not bother emitting code while force_recompile is active.
			// We will compile again.
			statement_count++;
			return;
		}

		if (redirect_statement)
		{
			redirect_statement->push_back(join(std::forward<Ts>(ts)...));
			statement_count++;
		}
		else
		{
			for (uint32_t i = 0; i < indent; i++)
				buffer << "    ";
			statement_inner(std::forward<Ts>(ts)...);
			buffer << '\n';
		}
	}

	template <typename... Ts>
	inline void statement_no_indent(Ts &&... ts)
	{
		auto old_indent = indent;
		indent = 0;
		statement(std::forward<Ts>(ts)...);
		indent = old_indent;
	}

	inline bool is_legacy() const
	{
		return (options.es && options.version < 300) || (!options.es && options.version < 130);
	}

	inline bool is_legacy_es() const
	{
		return options.es && options.version < 300;
	}

	inline bool is_legacy_desktop() const
	{
		return !options.es && options.version < 130;
	}

	void add_resource_name(uint32_t id);
	uint32_t to_array_size_literal(const SPIRType &type) const;
	uint32_t to_array_size_literal(const SPIRType &type, uint32_t index) const;
	void begin_scope();
	void end_scope();
	void end_scope(const std::string &trailer);
	void end_scope_decl();
	void end_scope_decl(const std::string &decl);
	void add_local_variable_name(uint32_t id);
	void fixup_anonymous_struct_names();
	void fixup_anonymous_struct_names(std::unordered_set<uint32_t> &visited, const SPIRType &type);
	void fixup_type_alias();
	void fixup_image_load_store_access();
	void reorder_type_alias();
	void reset(uint32_t iteration_count);
	void emit_function(SPIRFunction &func, const Bitset &return_flags);
	void add_header_line(const std::string &str);
	bool variable_is_lut(const SPIRVariable &var) const;
	bool is_stage_output_variable_masked(const SPIRVariable &var) const;
	bool is_stage_output_block_member_masked(const SPIRVariable &var, uint32_t index, bool strip_array) const;
	uint32_t get_declared_member_location(const SPIRVariable &var, uint32_t mbr_idx, bool strip_array) const;
	uint32_t type_to_location_count(const SPIRType &type) const;
	static const char *vector_swizzle(int vecsize, int index);
	std::string to_expression(uint32_t id, bool register_expression_read = true);
	void handle_invalid_expression(uint32_t id);
	void force_temporary_and_recompile(uint32_t id);
	std::string load_flattened_struct(const std::string &basename, const SPIRType &type);
	std::string to_flattened_struct_member(const std::string &basename, const SPIRType &type,
	                                                    uint32_t index);
	void track_expression_read(uint32_t id);
	bool expression_is_forwarded(uint32_t id) const;
	bool expression_suppresses_usage_tracking(uint32_t id) const;
	int get_constant_mapping_to_workgroup_component(const SPIRConstant &constant) const;
	bool expression_read_implies_multiple_reads(uint32_t id) const;

	std::string to_member_name(const SPIRType &type, uint32_t index);
	std::string remap_swizzle(const SPIRType &result_type, uint32_t input_components, const std::string &expr);
	uint32_t get_accumulated_member_location(const SPIRVariable &var, uint32_t mbr_idx, bool strip_array) const;
	std::string constant_expression(const SPIRConstant &c,
	                                bool inside_block_like_struct_scope = false,
	                                bool inside_struct_scope = false);
	std::string constant_expression_vector(const SPIRConstant &c, uint32_t vector);
	std::string convert_half_to_string(const SPIRConstant &value, uint32_t col, uint32_t row);
	std::string convert_float_to_string(const SPIRConstant &value, uint32_t col, uint32_t row);
	std::string convert_double_to_string(const SPIRConstant &value, uint32_t col, uint32_t row);
	void require_extension_internal(const std::string &ext);
	bool has_extension(const std::string &ext) const;
	void mask_stage_output_by_builtin(spv::BuiltIn builtin);
	std::string to_unpacked_row_major_matrix_expression(uint32_t id);
	std::string to_unpacked_expression(uint32_t id, bool register_expression_read = true);
	std::string to_dereferenced_expression(uint32_t id, bool register_expression_read = true);
	std::string to_extract_component_expression(uint32_t id, uint32_t index);
	std::string to_enclosed_expression(uint32_t id, bool register_expression_read = true);
	std::string type_to_glsl_constructor(const SPIRType &type);
	std::string to_enclosed_unpacked_expression(uint32_t id, bool register_expression_read = true);
	std::string to_pointer_expression(uint32_t id, bool register_expression_read = true);
	std::string enclose_expression(const std::string &expr);
	bool optimize_read_modify_write(const SPIRType &type, const std::string &lhs, const std::string &rhs);
	static bool needs_enclose_expression(const std::string &expr);
	std::string bitcast_expression(SPIRType::BaseType target_type, uint32_t arg);
	std::string bitcast_expression(const SPIRType &target_type, SPIRType::BaseType expr_type, const std::string &expr);
	std::string constant_value_macro_name(uint32_t id);
	void emit_struct(SPIRType &type);
	void add_member_name(SPIRType &type, uint32_t name);
	bool type_is_empty(const SPIRType &type);
	bool should_forward(uint32_t id) const;
	SPIRExpression &emit_op(uint32_t result_type, uint32_t result_id, const std::string &rhs, bool forward_rhs,
	                        bool suppress_usage_tracking = false);
	std::string declare_temporary(uint32_t type, uint32_t id);
	bool should_dereference(uint32_t id);
	std::string flags_to_qualifiers_glsl(const SPIRType &type, const Bitset &flags);
	std::string address_of_expression(const std::string &expr);
	std::string access_chain_internal(uint32_t base, const uint32_t *indices, uint32_t count, AccessChainFlags flags,
	                                  AccessChainMeta *meta);

	bool is_user_type_structured(uint32_t id) const;
	void access_chain_internal_append_index(std::string &expr, uint32_t base, const SPIRType *type,
	                                        AccessChainFlags flags, bool &access_chain_is_arrayed, uint32_t index);
	bool is_per_primitive_variable(const SPIRVariable &var) const;

	std::string access_chain(uint32_t base, const uint32_t *indices, uint32_t count, const SPIRType &target_type,
	                         AccessChainMeta *meta = nullptr, bool ptr_chain = false);

	std::pair<std::string, uint32_t> flattened_access_chain_offset(const SPIRType &basetype, const uint32_t *indices,
	                                                               uint32_t count, uint32_t offset,
	                                                               uint32_t word_stride, bool *need_transpose = nullptr,
	                                                               uint32_t *matrix_stride = nullptr,
	                                                               uint32_t *array_stride = nullptr,
	                                                               bool ptr_chain = false);
	std::string flattened_access_chain(uint32_t base, const uint32_t *indices, uint32_t count,
	                                   const SPIRType &target_type, uint32_t offset, uint32_t matrix_stride,
	                                   uint32_t array_stride, bool need_transpose);
	std::string flattened_access_chain_struct(uint32_t base, const uint32_t *indices, uint32_t count,
	                                          const SPIRType &target_type, uint32_t offset);
	std::string flattened_access_chain_matrix(uint32_t base, const uint32_t *indices, uint32_t count,
	                                          const SPIRType &target_type, uint32_t offset, uint32_t matrix_stride,
	                                          bool need_transpose);
	std::string flattened_access_chain_vector(uint32_t base, const uint32_t *indices, uint32_t count,
	                                          const SPIRType &target_type, uint32_t offset, uint32_t matrix_stride,
	                                          bool need_transpose);
	std::string to_flattened_access_chain_expression(uint32_t id);
	spv::Op get_remapped_spirv_op(spv::Op op) const;
	uint32_t get_integer_width_for_instruction(const Instruction &instr) const;
	void emit_binary_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op,
	                         SPIRType::BaseType input_type, bool skip_cast_if_equal_type, bool implicit_integer_promotion);
	void emit_binary_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op);
	void emit_unary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op);
	void register_control_dependent_expression(uint32_t expr);
	void emit_bitfield_insert_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2,
	                             uint32_t op3, const char *op, SPIRType::BaseType offset_count_type);
	void emit_trinary_func_op_bitextract(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1,
	                                     uint32_t op2, const char *op, SPIRType::BaseType expected_result_type,
	                                     SPIRType::BaseType input_type0, SPIRType::BaseType input_type1,
	                                     SPIRType::BaseType input_type2);
	std::string bitcast_glsl(const SPIRType &result_type, uint32_t arg);
	void emit_unary_func_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op,
	                             SPIRType::BaseType input_type, SPIRType::BaseType expected_result_type);
	void emit_binary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op);
	SPIRExpression &emit_uninitialized_temporary_expression(uint32_t type, uint32_t id);
	void emit_uninitialized_temporary(uint32_t type, uint32_t id);
	bool type_can_zero_initialize(const SPIRType &type) const;
	SPIRType binary_op_bitcast_helper(std::string &cast_op0, std::string &cast_op1, SPIRType::BaseType &input_type,
	                                  uint32_t op0, uint32_t op1, bool skip_cast_if_equal_type);
	void flush_variable_declaration(uint32_t id);
	void emit_variable_temporary_copies(const SPIRVariable &var);
	spv::StorageClass get_expression_effective_storage_class(uint32_t ptr);
	uint32_t get_integer_width_for_glsl_instruction(GLSLstd450 op, const uint32_t *arguments, uint32_t length) const;
	GLSLstd450 get_remapped_glsl_op(GLSLstd450 std450_op) const;
	void emit_trinary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2,
	                          const char *op);
	const char *index_to_swizzle(uint32_t index);
	void register_call_out_argument(uint32_t id);
	void add_function_overload(const SPIRFunction &func);
	bool expression_is_constant_null(uint32_t id) const;
	std::string dereference_expression(const SPIRType &expression_type, const std::string &expr);
	void strip_enclosed_expression(std::string &expr);
	bool member_is_remapped_physical_type(const SPIRType &type, uint32_t index) const;
	bool member_is_packed_physical_type(const SPIRType &type, uint32_t index) const;
	bool is_stage_output_builtin_masked(spv::BuiltIn builtin) const;
	std::string to_array_size(const SPIRType &type, uint32_t index);
	std::string to_rerolled_array_expression(const SPIRType &parent_type, const std::string &expr, const SPIRType &type);

	void add_variable(std::unordered_set<std::string> &variables_primary,
	                  const std::unordered_set<std::string> &variables_secondary, std::string &name);
	void reset_name_caches();
	void emit_line_directive(uint32_t file_id, uint32_t line_literal);
	std::string variable_decl_function_local(SPIRVariable &variable);
	void emit_block_chain(SPIRBlock &block);
	bool is_stage_output_location_masked(uint32_t location, uint32_t component) const;
	bool remove_duplicate_swizzle(std::string &op);
	void emit_hoisted_temporaries(SmallVector<std::pair<TypeID, ID>> &temporaries);
	void flush_undeclared_variables(SPIRBlock &block);
	void emit_while_loop_initializers(const SPIRBlock &block);
	void emit_block_instructions(SPIRBlock &block);
	std::string emit_continue_block(uint32_t continue_block, bool follow_true_block, bool follow_false_block);

	struct TemporaryCopy
	{
		uint32_t dst_id;
		uint32_t src_id;
	};
	TemporaryCopy handle_instruction_precision(const Instruction &instr);
	void analyze_precision_requirements(uint32_t type_id, uint32_t dst_id, uint32_t *args, uint32_t length);
	GLSLOptions::Precision analyze_expression_precision(const uint32_t *args, uint32_t length) const;
	uint32_t consume_temporary_in_precision_context(uint32_t type_id, uint32_t id, GLSLOptions::Precision precision);
	void forward_relaxed_precision(uint32_t dst_id, const uint32_t *args, uint32_t length);
	void branch(BlockID from, BlockID to);
	void branch(BlockID from, uint32_t cond, BlockID true_block, BlockID false_block);
	void branch_to_continue(BlockID from, BlockID to);
	void flush_phi(BlockID from, BlockID to);
	bool attempt_emit_loop_header(SPIRBlock &block, SPIRBlock::Method method);
	void emit_block_instructions_with_masked_debug(SPIRBlock &block);
	std::string emit_for_loop_initializers(const SPIRBlock &block);
	void emit_mesh_tasks(SPIRBlock &block);
	bool for_loop_initializers_are_same_type(const SPIRBlock &block);
	void handle_store_to_invariant_variable(uint32_t store_id, uint32_t value_id);
	bool unroll_array_to_complex_store(uint32_t target_id, uint32_t source_id);
	void convert_non_uniform_expression(std::string &expr, uint32_t ptr_id);
	void disallow_forwarding_in_expression_chain(const SPIRExpression &expr);
	void emit_sparse_feedback_temporaries(uint32_t result_type_id, uint32_t id, uint32_t &feedback_id,
	                                      uint32_t &texel_id);
	void emit_binary_func_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op,
	                              SPIRType::BaseType input_type, bool skip_cast_if_equal_type);
	void emit_trinary_func_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2,
	                               const char *op, SPIRType::BaseType input_type);
	void emit_emulated_ahyper_op(uint32_t result_type, uint32_t result_id, uint32_t op0, GLSLstd450 op);
	void emit_mix_op(uint32_t result_type, uint32_t id, uint32_t left, uint32_t right, uint32_t lerp);
	void emit_nminmax_op(uint32_t result_type, uint32_t id, uint32_t op0, uint32_t op1, GLSLstd450 op);
	std::string to_ternary_expression(const SPIRType &result_type, uint32_t select, uint32_t true_value,
	                                  uint32_t false_value);
	bool to_trivial_mix_op(const SPIRType &type, std::string &op, uint32_t left, uint32_t right, uint32_t lerp);
	std::string to_enclosed_pointer_expression(uint32_t id, bool register_expression_read = true);


	enum Polyfill : uint32_t
	{
		PolyfillTranspose2x2 = 1 << 0,
		PolyfillTranspose3x3 = 1 << 1,
		PolyfillTranspose4x4 = 1 << 2,
		PolyfillDeterminant2x2 = 1 << 3,
		PolyfillDeterminant3x3 = 1 << 4,
		PolyfillDeterminant4x4 = 1 << 5,
		PolyfillMatrixInverse2x2 = 1 << 6,
		PolyfillMatrixInverse3x3 = 1 << 7,
		PolyfillMatrixInverse4x4 = 1 << 8,
	};
	std::vector<PlsRemap> pls_inputs;


	uint32_t required_polyfills = 0;
	uint32_t required_polyfills_relaxed = 0;
	ShaderSubgroupSupportHelper shader_subgroup_supporter;
	void require_polyfill(Polyfill polyfill, bool relaxed);
	uint32_t get_sparse_feedback_texel_id(uint32_t id) const;
	SmallVector<ConstantID> get_composite_constant_ids(ConstantID const_id);
	void set_composite_constant(ConstantID const_id, TypeID type_id, const SmallVector<ConstantID> &initializers);
	TypeID get_composite_member_type(TypeID type_id, uint32_t member_idx);
	void fill_composite_constant(SPIRConstant &constant, TypeID type_id, const SmallVector<ConstantID> &initializers);
	void request_subgroup_feature(ShaderSubgroupSupportHelper::Feature feature);
	void unroll_array_from_complex_load(uint32_t target_id, uint32_t source_id, std::string &expr);
	void rewrite_load_for_wrapped_row_major(std::string &expr, TypeID loaded_type, ID ptr);
	SmallVector<TypeID> workaround_ubo_load_overload_types;
	void request_workaround_wrapper_overload(TypeID id);
	bool expression_is_non_value_type_array(uint32_t ptr);
	void store_flattened_struct(uint32_t lhs_id, uint32_t value);
	void store_flattened_struct(const std::string &basename, uint32_t rhs, const SPIRType &type,
	                            const SmallVector<uint32_t> &indices);
	std::string to_multi_member_reference(const SPIRType &type, const SmallVector<uint32_t> &indices);

	void register_impure_function_call();
	std::string to_combined_image_sampler(VariableID image_id, VariableID samp_id);
	void append_global_func_args(const SPIRFunction &func, uint32_t index, SmallVector<std::string> &arglist);
	void check_function_call_constraints(const uint32_t *args, uint32_t length);
	bool args_will_forward(uint32_t id, const uint32_t *args, uint32_t num_args, bool pure);
	std::string build_composite_combiner(uint32_t result_type, const uint32_t *elems, uint32_t length);
	std::string to_extract_constant_composite_expression(uint32_t result_type, const SPIRConstant &c,
	                                                     const uint32_t *chain, uint32_t length);
	std::string to_composite_constructor_expression(const SPIRType &parent_type, uint32_t id, bool block_like_type);
	bool remove_unity_swizzle(uint32_t base, std::string &op);
	bool should_suppress_usage_tracking(uint32_t id) const;
	void emit_copy_logical_type(uint32_t lhs_id, uint32_t lhs_type_id, uint32_t rhs_id, uint32_t rhs_type_id,
	                            SmallVector<uint32_t> chain);
	void emit_unary_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op);
	void emit_unary_op(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op);
	void emit_unrolled_binary_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op,
	                             bool negate, SPIRType::BaseType expected_type);
	bool check_atomic_image(uint32_t id);
	void emit_atomic_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op);
	void emit_atomic_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2, const char *op);
	std::string to_non_uniform_aware_expression(uint32_t id);
	std::string convert_separate_image_to_expression(uint32_t id);
	std::string legacy_tex_op(const std::string &op, const SPIRType &imgtype, uint32_t id);
	bool subpass_input_is_framebuffer_fetch(uint32_t id) const;
	static uint32_t mask_relevant_memory_semantics(uint32_t semantics);
	const Instruction *get_next_instruction_in_block(const Instruction &instr);
	void emit_spv_amd_shader_ballot_op(uint32_t result_type, uint32_t result_id, uint32_t op,
	                                          const uint32_t *args, uint32_t count);
	void emit_spv_amd_shader_explicit_vertex_parameter_op(uint32_t result_type, uint32_t result_id, uint32_t op,
	                                                              const uint32_t *args, uint32_t count);
	void emit_spv_amd_gcn_shader_op(uint32_t result_type, uint32_t result_id, uint32_t op, const uint32_t *args,
	                                        uint32_t count);
	//Override methods
	void GLSL_emit_store_statement(uint32_t lhs_expression, uint32_t rhs_expression);
	void GLSL_emit_texture_op(const Instruction &i, bool sparse);
	void GLSL_emit_glsl_op(uint32_t result_type, uint32_t result_id, uint32_t op, const uint32_t *args,
	                          uint32_t count);
	void GLSL_emit_spv_amd_shader_trinary_minmax_op(uint32_t result_type, uint32_t result_id, uint32_t op,
	                                                   const uint32_t *args, uint32_t count);
	std::string GLSL_to_texture_op(const Instruction &i, bool sparse, bool *forward,
	                                  SmallVector<uint32_t> &inherited_expressions);
	std::string GLSL_to_func_call_arg(const SPIRFunction::Parameter &arg, uint32_t id);
	std::string GLSL_convert_row_major_matrix(std::string exp_str, const SPIRType &exp_type,
	                                             uint32_t physical_type_id, bool is_packed,
	                                             bool relaxed = false);
	void GLSL_replace_illegal_names();
	void GLSL_replace_illegal_names(const std::unordered_set<std::string> &keywords);
	std::string GLSL_type_to_array_glsl(
	    const SPIRType &type); // Allow Metal to use the array<T> template to make arrays a value type
	std::string GLSL_constant_op_expression(const SPIRConstantOp &cop);
	std::string GLSL_variable_decl(const SPIRType &type, const std::string &name, uint32_t id = 0);
	std::string GLSL_variable_decl(const SPIRVariable &variable);
	std::string GLSL_builtin_to_glsl(spv::BuiltIn builtin, spv::StorageClass storage);
	std::string GLSL_to_initializer_expression(const SPIRVariable &var);
	void GLSL_emit_instruction(const Instruction &instr);
};
} // namespace SPIRV_CROSS_NAMESPACE

#endif
