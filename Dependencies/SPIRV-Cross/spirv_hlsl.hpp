/*
 * Copyright 2016-2021 Robert Konrad
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

#ifndef SPIRV_HLSL_HPP
#define SPIRV_HLSL_HPP

#include "GLSL.std.450.h"
#include "spirv_cross.hpp"
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace SPIRV_CROSS_NAMESPACE
{
// Interface which remaps vertex inputs to a fixed semantic name to make linking easier.
struct HLSLVertexAttributeRemap
{
	uint32_t location;
	std::string semantic;
};
// Specifying a root constant (d3d12) or push constant range (vulkan).
//
// `start` and `end` denotes the range of the root constant in bytes.
// Both values need to be multiple of 4.
struct RootConstants
{
	uint32_t start;
	uint32_t end;

	uint32_t binding;
	uint32_t space;
};

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

// For finer control, decorations may be removed from specific resources instead with unset_decoration().
enum HLSLBindingFlagBits
{
	HLSL_BINDING_AUTO_NONE_BIT = 0,

	// Push constant (root constant) resources will be declared as CBVs (b-space) without a register() declaration.
	// A register will be automatically assigned by the D3D compiler, but must therefore be reflected in D3D-land.
	// Push constants do not normally have a DecorationBinding set, but if they do, this can be used to ignore it.
	HLSL_BINDING_AUTO_PUSH_CONSTANT_BIT = 1 << 0,

	// cbuffer resources will be declared as CBVs (b-space) without a register() declaration.
	// A register will be automatically assigned, but must be reflected in D3D-land.
	HLSL_BINDING_AUTO_CBV_BIT = 1 << 1,

	// All SRVs (t-space) will be declared without a register() declaration.
	HLSL_BINDING_AUTO_SRV_BIT = 1 << 2,

	// All UAVs (u-space) will be declared without a register() declaration.
	HLSL_BINDING_AUTO_UAV_BIT = 1 << 3,

	// All samplers (s-space) will be declared without a register() declaration.
	HLSL_BINDING_AUTO_SAMPLER_BIT = 1 << 4,

	// No resources will be declared with register().
	HLSL_BINDING_AUTO_ALL = 0x7fffffff
};
using HLSLBindingFlags = uint32_t;

// By matching stage, desc_set and binding for a SPIR-V resource,
// register bindings are set based on whether the HLSL resource is a
// CBV, UAV, SRV or Sampler. A single binding in SPIR-V might contain multiple
// resource types, e.g. COMBINED_IMAGE_SAMPLER, and SRV/Sampler bindings will be used respectively.
// On SM 5.0 and lower, register_space is ignored.
//
// To remap a push constant block which does not have any desc_set/binding associated with it,
// use ResourceBindingPushConstant{DescriptorSet,Binding} as values for desc_set/binding.
// For deeper control of push constants, set_root_constant_layouts() can be used instead.
struct HLSLResourceBinding
{
	spv::ExecutionModel stage = spv::ExecutionModelMax;
	uint32_t desc_set = 0;
	uint32_t binding = 0;

	struct Binding
	{
		uint32_t register_space = 0;
		uint32_t register_binding = 0;
	} cbv, uav, srv, sampler;
};

enum HLSLAuxBinding
{
	HLSL_AUX_BINDING_BASE_VERTEX_INSTANCE = 0
};

class CompilerHLSL : public Compiler
{
public:
	struct Options
	{
		uint32_t shader_model = 30; // TODO: map ps_4_0_level_9_0,... somehow

		// Allows the PointSize builtin in SM 4.0+, and ignores it, as PointSize is not supported in SM 4+.
		bool point_size_compat = false;

		// Allows the PointCoord builtin, returns float2(0.5, 0.5), as PointCoord is not supported in HLSL.
		bool point_coord_compat = false;

		// If true, the backend will assume that VertexIndex and InstanceIndex will need to apply
		// a base offset, and you will need to fill in a cbuffer with offsets.
		// Set to false if you know you will never use base instance or base vertex
		// functionality as it might remove an internal cbuffer.
		bool support_nonzero_base_vertex_base_instance = false;

		// Forces a storage buffer to always be declared as UAV, even if the readonly decoration is used.
		// By default, a readonly storage buffer will be declared as ByteAddressBuffer (SRV) instead.
		// Alternatively, use set_hlsl_force_storage_buffer_as_uav to specify individually.
		bool force_storage_buffer_as_uav = false;

		// Forces any storage image type marked as NonWritable to be considered an SRV instead.
		// For this to work with function call parameters, NonWritable must be considered to be part of the type system
		// so that NonWritable image arguments are also translated to Texture rather than RWTexture.
		bool nonwritable_uav_texture_as_srv = false;

		// Enables native 16-bit types. Needs SM 6.2.
		// Uses half/int16_t/uint16_t instead of min16* types.
		// Also adds support for 16-bit load-store from (RW)ByteAddressBuffer.
		bool enable_16bit_types = false;

		// If matrices are used as IO variables, flatten the attribute declaration to use
		// TEXCOORD{N,N+1,N+2,...} rather than TEXCOORDN_{0,1,2,3}.
		// If add_vertex_attribute_remap is used and this feature is used,
		// the semantic name will be queried once per active location.
		bool flatten_matrix_vertex_input_semantics = false;

		// Rather than emitting main() for the entry point, use the name in SPIR-V.
		bool use_entry_point_name = false;

		// Preserve (RW)StructuredBuffer types if the input source was HLSL.
		// This relies on UserTypeGOOGLE to encode the buffer type either as "structuredbuffer" or "rwstructuredbuffer"
		// whereas the type can be extended with an optional subtype, e.g. "structuredbuffer:int".
		bool preserve_structured_buffers = false;
	};

	struct OptionsGLSL
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


	explicit CompilerHLSL(std::vector<uint32_t> spirv_)
	    : Compiler(std::move(spirv_))
	{
	}

	CompilerHLSL(const uint32_t *ir_, size_t size)
	    : Compiler(ir_, size)
	{
	}

	explicit CompilerHLSL(const ParsedIR &ir_)
	    : Compiler(ir_)
	{
	}

	explicit CompilerHLSL(ParsedIR &&ir_)
	    : Compiler(std::move(ir_))
	{
	}

	const Options &get_hlsl_options() const
	{
		return hlsl_options;
	}

	void set_hlsl_options(const Options &opts)
	{
		hlsl_options = opts;
	}

	// Optionally specify a custom root constant layout.
	//
	// Push constants ranges will be split up according to the
	// layout specified.
	void set_root_constant_layouts(std::vector<RootConstants> layout);

	// Compiles and remaps vertex attributes at specific locations to a fixed semantic.
	// The default is TEXCOORD# where # denotes location.
	// Matrices are unrolled to vectors with notation ${SEMANTIC}_#, where # denotes row.
	// $SEMANTIC is either TEXCOORD# or a semantic name specified here.
	void add_vertex_attribute_remap(const HLSLVertexAttributeRemap &vertex_attributes);
	std::string compile() override;

	// This is a special HLSL workaround for the NumWorkGroups builtin.
	// This does not exist in HLSL, so the calling application must create a dummy cbuffer in
	// which the application will store this builtin.
	// The cbuffer layout will be:
	// cbuffer SPIRV_Cross_NumWorkgroups : register(b#, space#) { uint3 SPIRV_Cross_NumWorkgroups_count; };
	// This must be called before compile().
	// The function returns 0 if NumWorkGroups builtin is not statically used in the shader from the current entry point.
	// If non-zero, this returns the variable ID of a cbuffer which corresponds to
	// the cbuffer declared above. By default, no binding or descriptor set decoration is set,
	// so the calling application should declare explicit bindings on this ID before calling compile().
	VariableID remap_num_workgroups_builtin();

	// Controls how resource bindings are declared in the output HLSL.
	void set_resource_binding_flags(HLSLBindingFlags flags);

	// resource is a resource binding to indicate the HLSL CBV, SRV, UAV or sampler binding
	// to use for a particular SPIR-V description set
	// and binding. If resource bindings are provided,
	// is_hlsl_resource_binding_used() will return true after calling ::compile() if
	// the set/binding combination was used by the HLSL code.
	void add_hlsl_resource_binding(const HLSLResourceBinding &resource);
	bool is_hlsl_resource_binding_used(spv::ExecutionModel model, uint32_t set, uint32_t binding) const;

	// Controls which storage buffer bindings will be forced to be declared as UAVs.
	void set_hlsl_force_storage_buffer_as_uav(uint32_t desc_set, uint32_t binding);

	// By default, these magic buffers are not assigned a specific binding.
	void set_hlsl_aux_buffer_binding(HLSLAuxBinding binding, uint32_t register_index, uint32_t register_space);
	void unset_hlsl_aux_buffer_binding(HLSLAuxBinding binding);
	bool is_hlsl_aux_buffer_binding_used(HLSLAuxBinding binding) const;

private:
	std::string type_to_glsl(const SPIRType &type, uint32_t id = 0);
	std::string image_type_hlsl(const SPIRType &type, uint32_t id);
	std::string image_type_hlsl_modern(const SPIRType &type, uint32_t id);
	std::string image_type_hlsl_legacy(const SPIRType &type, uint32_t id);
	void emit_function_prototype(SPIRFunction &func, const Bitset &return_flags);
	void emit_hlsl_entry_point();
	void emit_header();
	void emit_resources();
	void emit_interface_block_globally(const SPIRVariable &type);
	void emit_interface_block_in_struct(const SPIRVariable &var, std::unordered_set<uint32_t> &active_locations);
	void emit_interface_block_member_in_struct(const SPIRVariable &var, uint32_t member_index, uint32_t location,
	                                           std::unordered_set<uint32_t> &active_locations);
	void emit_builtin_inputs_in_struct();
	void emit_builtin_outputs_in_struct();
	void emit_builtin_primitive_outputs_in_struct();
	void emit_texture_op(const Instruction &i, bool sparse);
	void emit_instruction(const Instruction &instruction);
	void emit_glsl_op(uint32_t result_type, uint32_t result_id, uint32_t op, const uint32_t *args,
	                  uint32_t count);
	void emit_buffer_block(const SPIRVariable &type);
	void emit_push_constant_block(const SPIRVariable &var);
	void emit_uniform(const SPIRVariable &var);
	void emit_modern_uniform(const SPIRVariable &var);
	void emit_legacy_uniform(const SPIRVariable &var);
	void emit_specialization_constants_and_structs();
	void emit_composite_constants();
	void emit_fixup();
	std::string builtin_to_glsl(spv::BuiltIn builtin, spv::StorageClass storage);
	std::string layout_for_member(const SPIRType &type, uint32_t index);
	std::string to_interpolation_qualifiers(const Bitset &flags);
	std::string bitcast_glsl_op(const SPIRType &result_type, const SPIRType &argument_type);
	bool emit_complex_bitcast(uint32_t result_type, uint32_t id, uint32_t op0);
	std::string to_func_call_arg(const SPIRFunction::Parameter &arg, uint32_t id);
	std::string to_sampler_expression(uint32_t id);
	std::string to_resource_binding(const SPIRVariable &var);
	std::string to_resource_binding_sampler(const SPIRVariable &var);
	std::string to_resource_register(HLSLBindingFlagBits flag, char space, uint32_t binding, uint32_t set);
	std::string to_initializer_expression(const SPIRVariable &var);
	void emit_sampled_image_op(uint32_t result_type, uint32_t result_id, uint32_t image_id, uint32_t samp_id);
	void emit_access_chain(const Instruction &instruction);
	void emit_load(const Instruction &instruction);
	void read_access_chain(std::string *expr, const std::string &lhs, const SPIRAccessChain &chain);
	void read_access_chain_struct(const std::string &lhs, const SPIRAccessChain &chain);
	void read_access_chain_array(const std::string &lhs, const SPIRAccessChain &chain);
	void write_access_chain(const SPIRAccessChain &chain, uint32_t value, const SmallVector<uint32_t> &composite_chain);
	void write_access_chain_struct(const SPIRAccessChain &chain, uint32_t value,
	                               const SmallVector<uint32_t> &composite_chain);
	void write_access_chain_array(const SPIRAccessChain &chain, uint32_t value,
	                              const SmallVector<uint32_t> &composite_chain);
	std::string write_access_chain_value(uint32_t value, const SmallVector<uint32_t> &composite_chain, bool enclose);
	void emit_store(const Instruction &instruction);
	void emit_atomic(const uint32_t *ops, uint32_t length, spv::Op op);
	void emit_subgroup_op(const Instruction &i);
	void emit_block_hints(const SPIRBlock &block);

	void emit_struct_member(const SPIRType &type, uint32_t member_type_id, uint32_t index, const std::string &qualifier = "",
	                        uint32_t base_offset = 0);
	void emit_rayquery_function(const char *commited, const char *candidate, const uint32_t *ops);
	void emit_mesh_tasks(SPIRBlock &block);

	const char *to_storage_qualifiers_glsl(const SPIRVariable &var);
	void replace_illegal_names();

	bool is_hlsl_force_storage_buffer_as_uav(ID id) const;

	Options hlsl_options;

	// TODO: Refactor this to be more similar to MSL, maybe have some common system in place?
	bool requires_op_fmod = false;
	bool requires_fp16_packing = false;
	bool requires_uint2_packing = false;
	bool requires_explicit_fp16_packing = false;
	bool requires_unorm8_packing = false;
	bool requires_snorm8_packing = false;
	bool requires_unorm16_packing = false;
	bool requires_snorm16_packing = false;
	bool requires_bitfield_insert = false;
	bool requires_bitfield_extract = false;
	bool requires_inverse_2x2 = false;
	bool requires_inverse_3x3 = false;
	bool requires_inverse_4x4 = false;
	bool requires_scalar_reflect = false;
	bool requires_scalar_refract = false;
	bool requires_scalar_faceforward = false;

	struct TextureSizeVariants
	{
		// MSVC 2013 workaround.
		TextureSizeVariants()
		{
			srv = 0;
			for (auto &unorm : uav)
				for (auto &u : unorm)
					u = 0;
		}
		uint64_t srv;
		uint64_t uav[3][4];
	} required_texture_size_variants;

	void require_texture_query_variant(uint32_t var_id);
	void emit_texture_size_variants(uint64_t variant_mask, const char *vecsize_qualifier, bool uav,
	                                const char *type_qualifier);

	enum TextureQueryVariantDim
	{
		Query1D = 0,
		Query1DArray,
		Query2D,
		Query2DArray,
		Query3D,
		QueryBuffer,
		QueryCube,
		QueryCubeArray,
		Query2DMS,
		Query2DMSArray,
		QueryDimCount
	};

	enum TextureQueryVariantType
	{
		QueryTypeFloat = 0,
		QueryTypeInt = 16,
		QueryTypeUInt = 32,
		QueryTypeCount = 3
	};

	enum BitcastType
	{
		TypeNormal,
		TypePackUint2x32,
		TypeUnpackUint64
	};

	void analyze_meshlet_writes();
	void analyze_meshlet_writes(uint32_t func_id, uint32_t id_per_vertex, uint32_t id_per_primitive,
	                            std::unordered_set<uint32_t> &processed_func_ids);

	BitcastType get_bitcast_type(uint32_t result_type, uint32_t op0);

	void emit_builtin_variables();
	bool require_output = false;
	bool require_input = false;
	SmallVector<HLSLVertexAttributeRemap> remap_vertex_attributes;

	uint32_t type_to_consumed_locations(const SPIRType &type) const;

	std::string to_semantic(uint32_t location, spv::ExecutionModel em, spv::StorageClass sc);

	uint32_t num_workgroups_builtin = 0;
	HLSLBindingFlags resource_binding_flags = 0;

	// Custom root constant layout, which should be emitted
	// when translating push constant ranges.
	std::vector<RootConstants> root_constants_layout;

	void validate_shader_model();

	std::string get_unique_identifier();
	uint32_t unique_identifier_count = 0;

	std::unordered_map<StageSetBinding, std::pair<HLSLResourceBinding, bool>, InternalHasher> resource_bindings;
	void remap_hlsl_resource_binding(HLSLBindingFlagBits type, uint32_t &desc_set, uint32_t &binding);

	std::unordered_set<SetBindingPair, InternalHasher> force_uav_buffer_bindings;

	struct
	{
		uint32_t register_index = 0;
		uint32_t register_space = 0;
		bool explicit_binding = false;
		bool used = false;
	} base_vertex_info;

	// Returns true for BuiltInSampleMask because gl_SampleMask[] is an array in SPIR-V, but SV_Coverage is a scalar in HLSL.
	bool builtin_translates_to_nonarray(spv::BuiltIn builtin) const;

	// Returns true if the specified ID has a UserTypeGOOGLE decoration for StructuredBuffer or RWStructuredBuffer resources.
	bool is_user_type_structured(uint32_t id) const;

	std::vector<TypeID> composite_selection_workaround_types;

	std::string get_inner_entry_point_name() const;

	// FROM GLSL
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

	ShaderSubgroupSupportHelper shader_subgroup_supporter;

	// Can be overriden by subclass backends for trivial things which
	// shouldn't need polymorphism.
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

	OptionsGLSL options;

	SmallVector<std::string> header_lines;
	uint32_t statement_count = 0;
	SmallVector<std::string> *redirect_statement = nullptr;
	std::unordered_set<uint32_t> emitted_functions;

	// Ensure that we declare phi-variable copies even if the original declaration isn't deferred
	std::unordered_set<uint32_t> flushed_phi_variables;

	std::unordered_set<uint32_t> flattened_buffer_blocks;
	std::unordered_map<uint32_t, bool> flattened_structs;
	std::unordered_set<std::string> resource_names;
	std::unordered_set<std::string> block_names; // A union of all block_*_names.
	std::unordered_set<std::string> local_variable_names;
	std::unordered_map<uint32_t, uint32_t> extra_sub_expressions;
	std::unordered_map<uint32_t, std::string> preserved_aliases;
	std::unordered_map<std::string, std::unordered_set<uint64_t>> function_overloads;
	std::unordered_map<uint32_t, uint32_t> expression_usage_counts;
	std::unordered_set<uint32_t> composite_insert_overwritten;
	SmallVector<SPIRBlock *> current_emitting_switch_stack;
	std::unordered_set<std::string> block_input_names;
	std::unordered_set<std::string> block_output_names;
	std::unordered_set<std::string> block_ubo_names;
	std::unordered_set<std::string> block_ssbo_names;
	std::unordered_map<uint32_t, uint32_t> temporary_to_mirror_precision_alias;
	std::vector<std::pair<uint32_t, uint32_t>> subpass_to_framebuffer_fetch_attachment;
	const SPIRBlock *current_continue_block = nullptr;
	char current_locale_radix_character = '.';
	SmallVector<std::string> forced_extensions;
	std::unordered_set<uint32_t> block_composite_insert_overwrite;
	// Declare these temporaries before beginning the block.
	// Used for handling complex continue blocks which have side effects.
	SmallVector<TypeID> workaround_ubo_load_overload_types;
	std::vector<PlsRemap> pls_inputs;
	std::vector<PlsRemap> pls_outputs;

	bool processing_entry_point = false;
	bool block_debug_directives = false;
	bool current_emitting_switch_fallthrough = false;
	bool block_temporary_hoisting = false;
	uint32_t indent = 0;
	SPIRBlock *current_emitting_block = nullptr;
	StringStream<> buffer;
	bool ray_tracing_is_khr = false;
	bool barycentric_is_nv = false;

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

	inline bool is_legacy_es() const
	{
		return options.es && options.version < 300;
	}

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

	void add_resource_name(uint32_t id);
	std::string variable_decl(const SPIRVariable &variable); // Threadgroup arrays can't have a wrapper type
	std::string variable_decl(const SPIRType &type, const std::string &name, uint32_t id = 0);
	std::string constant_expression(const SPIRConstant &c,
	                                bool inside_block_like_struct_scope = false,
	                                bool inside_struct_scope = false);

	std::string to_member_name(const SPIRType &type, uint32_t index);

	std::string type_to_array_glsl(
	    const SPIRType &type); // Allow Metal to use the array<T> template to make arrays a value type

	std::string to_expression(uint32_t id, bool register_expression_read = true);

	void begin_scope();
	void end_scope();
	void end_scope(const std::string &trailer);
	void end_scope_decl();
	void end_scope_decl(const std::string &decl);

	std::string constant_value_macro_name(uint32_t id);
	std::string constant_op_expression(const SPIRConstantOp &cop);

	void emit_struct(SPIRType &type);
	bool type_can_zero_initialize(const SPIRType &type) const;
	std::string to_zero_initialized_expression(uint32_t type_id);
	void fixup_implicit_builtin_block_names(spv::ExecutionModel model);
	uint32_t get_declared_member_location(const SPIRVariable &var, uint32_t mbr_idx, bool strip_array) const;
	bool is_per_primitive_variable(const SPIRVariable &var) const;
	bool variable_is_lut(const SPIRVariable &var) const;
	void flush_variable_declaration(uint32_t id);
	SPIRExpression &emit_op(uint32_t result_type, uint32_t result_id, const std::string &rhs, bool forward_rhs,
	                        bool suppress_usage_tracking = false);
	std::string to_unpacked_expression(uint32_t id, bool register_expression_read = true);
	void emit_buffer_block_flattened(const SPIRVariable &type);
	void add_variable(std::unordered_set<std::string> &variables_primary,
	                  const std::unordered_set<std::string> &variables_secondary, std::string &name);

	bool buffer_is_packing_standard(const SPIRType &type, BufferPackingStandard packing,
	                                uint32_t *failed_index = nullptr, uint32_t start_offset = 0,
	                                uint32_t end_offset = ~(0u));

	void preserve_alias_on_reset(uint32_t id);
	void add_member_name(SPIRType &type, uint32_t name);
	std::string to_non_uniform_aware_expression(uint32_t id);
	std::string to_combined_image_sampler(VariableID image_id, VariableID samp_id);
	void add_function_overload(const SPIRFunction &func);
	std::string flags_to_qualifiers_glsl(const SPIRType &type, const Bitset &flags);
	bool skip_argument(uint32_t id) const;
	void add_local_variable_name(uint32_t id);
	std::string argument_decl(const SPIRFunction::Parameter &arg);
	bool should_forward(uint32_t id) const;
	std::string to_enclosed_expression(uint32_t id, bool register_expression_read = true);
	std::string to_extract_component_expression(uint32_t id, uint32_t index);
	void register_control_dependent_expression(uint32_t expr);
	uint32_t get_integer_width_for_glsl_instruction(GLSLstd450 op, const uint32_t *arguments, uint32_t length) const;
	GLSLstd450 get_remapped_glsl_op(GLSLstd450 std450_op) const;
	
	void emit_unary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op);
	void emit_emulated_ahyper_op(uint32_t result_type, uint32_t result_id, uint32_t op0, GLSLstd450 op);
	void emit_trinary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2,
	                          const char *op);

	void emit_binary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op);
	void emit_unary_func_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op,
	                             SPIRType::BaseType input_type, SPIRType::BaseType expected_result_type);
	std::string to_array_size(const SPIRType &type, uint32_t index);
	void convert_non_uniform_expression(std::string &expr, uint32_t ptr_id);
	SPIRExpression &emit_uninitialized_temporary_expression(uint32_t type, uint32_t id);
	void track_expression_read(uint32_t id);
	std::string access_chain_internal(uint32_t base, const uint32_t *indices, uint32_t count, AccessChainFlags flags,
	                                  AccessChainMeta *meta);

	std::string enclose_expression(const std::string &expr);
	const char *index_to_swizzle(uint32_t index);
	bool remove_duplicate_swizzle(std::string &op);
	std::string to_dereferenced_expression(uint32_t id, bool register_expression_read = true);
	std::string access_chain(uint32_t base, const uint32_t *indices, uint32_t count, const SPIRType &target_type,
	                         AccessChainMeta *meta = nullptr, bool ptr_chain = false);

	std::pair<std::string, uint32_t> flattened_access_chain_offset(const SPIRType &basetype, const uint32_t *indices,
	                                                               uint32_t count, uint32_t offset,
	                                                               uint32_t word_stride, bool *need_transpose = nullptr,
	                                                               uint32_t *matrix_stride = nullptr,
	                                                               uint32_t *array_stride = nullptr,
	                                                               bool ptr_chain = false);

	std::string bitcast_expression(const SPIRType &target_type, SPIRType::BaseType expr_type, const std::string &expr);
	std::string bitcast_expression(SPIRType::BaseType target_type, uint32_t arg);

	uint32_t get_integer_width_for_instruction(const Instruction &instr) const;
	spv::Op get_remapped_spirv_op(spv::Op op) const;
	std::string type_to_glsl_constructor(const SPIRType &type);
	void emit_unrolled_unary_op(uint32_t result_type, uint32_t result_id, uint32_t operand, const char *op);
	void emit_unary_op(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op);

	void emit_unrolled_binary_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op,
	                             bool negate, SPIRType::BaseType expected_type);
	void emit_binary_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op,
	                         SPIRType::BaseType input_type, bool skip_cast_if_equal_type, bool implicit_integer_promotion);
	void emit_binary_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op);
	std::string remap_swizzle(const SPIRType &result_type, uint32_t input_components, const std::string &expr);
	static uint32_t mask_relevant_memory_semantics(uint32_t semantics);
	const Instruction *get_next_instruction_in_block(const Instruction &instr);
	void fixup_anonymous_struct_names();
	void fixup_anonymous_struct_names(std::unordered_set<uint32_t> &visited, const SPIRType &type);

	void fixup_type_alias();
	void reorder_type_alias();
	void reset(uint32_t iteration_count);
	void reset_name_caches();
	
	void emit_function(SPIRFunction &func, const Bitset &return_flags);
	void emit_line_directive(uint32_t file_id, uint32_t line_literal);
	void emit_entry_point_declarations();
	void emit_block_chain(SPIRBlock &block);
	void emit_hoisted_temporaries(SmallVector<std::pair<TypeID, ID>> &temporaries);
	void emit_block_instructions(SPIRBlock &block);
	void emit_while_loop_initializers(const SPIRBlock &block);

	void flush_undeclared_variables(SPIRBlock &block);
	bool attempt_emit_loop_header(SPIRBlock &block, SPIRBlock::Method method);

	struct TemporaryCopy
	{
		uint32_t dst_id;
		uint32_t src_id;
	};
	TemporaryCopy handle_instruction_precision(const Instruction &instr);

	void branch(BlockID from, BlockID to);
	void branch_to_continue(BlockID from, BlockID to);
	void branch(BlockID from, uint32_t cond, BlockID true_block, BlockID false_block);

	void flush_phi(BlockID from, BlockID to);
	std::string to_pointer_expression(uint32_t id, bool register_expression_read = true);
	std::string address_of_expression(const std::string &expr);
	bool optimize_read_modify_write(const SPIRType &type, const std::string &lhs, const std::string &rhs);

	bool emit_array_copy(const char *expr, uint32_t lhs_id, uint32_t rhs_id,
	                             spv::StorageClass lhs_storage, spv::StorageClass rhs_storage);

	std::string emit_continue_block(uint32_t continue_block, bool follow_true_block, bool follow_false_block);
	void emit_block_instructions_with_masked_debug(SPIRBlock &block);
	std::string emit_for_loop_initializers(const SPIRBlock &block);
	bool for_loop_initializers_are_same_type(const SPIRBlock &block);
	void analyze_precision_requirements(uint32_t type_id, uint32_t dst_id, uint32_t *args, uint32_t length);
	OptionsGLSL::Precision analyze_expression_precision(const uint32_t *args, uint32_t length) const;
	uint32_t consume_temporary_in_precision_context(uint32_t type_id, uint32_t id, OptionsGLSL::Precision precision);
	void forward_relaxed_precision(uint32_t dst_id, const uint32_t *args, uint32_t length);
	std::string to_qualifiers_glsl(uint32_t id);
	std::string to_precision_qualifiers_glsl(uint32_t id);
	bool variable_decl_is_remapped_storage(const SPIRVariable &var, spv::StorageClass storage) const;
	bool expression_is_forwarded(uint32_t id) const;
	std::string constant_expression_vector(const SPIRConstant &c, uint32_t vector);
	std::string convert_half_to_string(const SPIRConstant &value, uint32_t col, uint32_t row);
	std::string convert_float_to_string(const SPIRConstant &value, uint32_t col, uint32_t row);
	std::string convert_double_to_string(const SPIRConstant &value, uint32_t col, uint32_t row);
	void require_extension_internal(const std::string &ext);
	bool has_extension(const std::string &ext) const;
	void handle_invalid_expression(uint32_t id);
	std::string convert_row_major_matrix(std::string exp_str, const SPIRType &exp_type,
	                                             uint32_t physical_type_id, bool is_packed,
	                                             bool relaxed = false);
	void strip_enclosed_expression(std::string &expr);
	uint32_t required_polyfills = 0;
	uint32_t required_polyfills_relaxed = 0;
	void require_polyfill(Polyfill polyfill, bool relaxed);
	std::string load_flattened_struct(const std::string &basename, const SPIRType &type);
	std::string to_flattened_struct_member(const std::string &basename, const SPIRType &type, uint32_t index);
	int get_constant_mapping_to_workgroup_component(const SPIRConstant &constant) const;
	static const char *vector_swizzle(int vecsize, int index);
	bool to_trivial_mix_op(const SPIRType &type, std::string &op, uint32_t left, uint32_t right, uint32_t lerp);
	std::string to_ternary_expression(const SPIRType &result_type, uint32_t select, uint32_t true_value,
	                                  uint32_t false_value);
	
	std::string to_enclosed_pointer_expression(uint32_t id, bool register_expression_read = true);
	SmallVector<ConstantID> get_composite_constant_ids(ConstantID const_id);
	void set_composite_constant(ConstantID const_id, TypeID type_id, const SmallVector<ConstantID> &initializers);
	TypeID get_composite_member_type(TypeID type_id, uint32_t member_idx);

	std::unordered_map<uint32_t, SmallVector<ConstantID>> const_composite_insert_ids;

	SPIRType binary_op_bitcast_helper(std::string &cast_op0, std::string &cast_op1, SPIRType::BaseType &input_type,
	                                  uint32_t op0, uint32_t op1, bool skip_cast_if_equal_type);

	std::string bitcast_glsl(const SPIRType &result_type, uint32_t arg);
	void fill_composite_constant(SPIRConstant &constant, TypeID type_id, const SmallVector<ConstantID> &initializers);
	bool type_is_empty(const SPIRType &type);
	void emit_struct_padding_target(const SPIRType &type);
	uint32_t get_accumulated_member_location(const SPIRVariable &var, uint32_t mbr_idx, bool strip_array) const;
	std::string variable_decl_function_local(SPIRVariable &variable);
	void emit_variable_temporary_copies(const SPIRVariable &var);
	std::string unpack_expression_type(std::string expr_str, const SPIRType &type, uint32_t physical_type_id,
	                                           bool packed_type, bool row_major);

	uint32_t type_to_packed_alignment(const SPIRType &type, const Bitset &flags, BufferPackingStandard packing);
	uint32_t type_to_packed_base_size(const SPIRType &type, BufferPackingStandard packing);
	uint32_t type_to_location_count(const SPIRType &type) const;
	uint32_t to_array_size_literal(const SPIRType &type, uint32_t index) const;
	uint32_t to_array_size_literal(const SPIRType &type) const;
	uint32_t type_to_packed_size(const SPIRType &type, const Bitset &flags, BufferPackingStandard packing);
	uint32_t type_to_packed_array_stride(const SPIRType &type, const Bitset &flags, BufferPackingStandard packing);
	std::string declare_temporary(uint32_t type, uint32_t id);
	void emit_uninitialized_temporary(uint32_t type, uint32_t id);
	bool expression_suppresses_usage_tracking(uint32_t id) const;
	bool expression_read_implies_multiple_reads(uint32_t id) const;
	void force_temporary_and_recompile(uint32_t id);

	static bool needs_enclose_expression(const std::string &expr);
	bool should_dereference(uint32_t id);
	std::string dereference_expression(const SPIRType &expression_type, const std::string &expr);
	bool is_non_native_row_major_matrix(uint32_t id);
	void access_chain_internal_append_index(std::string &expr, uint32_t base, const SPIRType *type,
	                                        AccessChainFlags flags, bool &access_chain_is_arrayed, uint32_t index);

	void check_physical_type_cast(std::string &expr, const SPIRType *type, uint32_t physical_type);
	bool access_chain_needs_stage_io_builtin_translation(uint32_t base);
	std::string to_member_reference(uint32_t base, const SPIRType &type, uint32_t index, bool ptr_chain_is_resolved);
	bool member_is_packed_physical_type(const SPIRType &type, uint32_t index) const;
	bool member_is_remapped_physical_type(const SPIRType &type, uint32_t index) const;
	bool member_is_non_native_row_major_matrix(const SPIRType &type, uint32_t index);
	void prepare_access_chain_for_scalar_access(std::string &expr, const SPIRType &type,
	                                                    spv::StorageClass storage, bool &is_packed);

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

	spv::StorageClass get_expression_effective_storage_class(uint32_t ptr);

	std::string to_flattened_access_chain_expression(uint32_t id);
	std::string to_enclosed_unpacked_expression(uint32_t id, bool register_expression_read = true);
	void request_subgroup_feature(ShaderSubgroupSupportHelper::Feature feature);
	void cast_from_variable_load(uint32_t source_id, std::string &expr, const SPIRType &expr_type);
	void unroll_array_from_complex_load(uint32_t target_id, uint32_t source_id, std::string &expr);
	void rewrite_load_for_wrapped_row_major(std::string &expr, TypeID loaded_type, ID ptr);
	void request_workaround_wrapper_overload(TypeID id);
	bool expression_is_non_value_type_array(uint32_t ptr);
	void register_call_out_argument(uint32_t id);
	void emit_binary_func_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op,
	                              SPIRType::BaseType input_type, bool skip_cast_if_equal_type);

	void emit_trinary_func_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2,
	                               const char *op, SPIRType::BaseType input_type);

	void emit_mix_op(uint32_t result_type, uint32_t id, uint32_t left, uint32_t right, uint32_t lerp);
	void emit_nminmax_op(uint32_t result_type, uint32_t id, uint32_t op0, uint32_t op1, GLSLstd450 op);
	void store_flattened_struct(uint32_t lhs_id, uint32_t value);
	void store_flattened_struct(const std::string &basename, uint32_t rhs, const SPIRType &type,
	                            const SmallVector<uint32_t> &indices);

	std::string to_multi_member_reference(const SPIRType &type, const SmallVector<uint32_t> &indices);
	void emit_store_statement(uint32_t lhs_expression, uint32_t rhs_expression);
	void handle_store_to_invariant_variable(uint32_t store_id, uint32_t value_id);
	void disallow_forwarding_in_expression_chain(const SPIRExpression &expr);
	bool unroll_array_to_complex_store(uint32_t target_id, uint32_t source_id);
	void cast_to_variable_store(uint32_t target_id, std::string &expr, const SPIRType &expr_type);
	void register_impure_function_call();
	void append_global_func_args(const SPIRFunction &func, uint32_t index, SmallVector<std::string> &arglist);
	void check_function_call_constraints(const uint32_t *args, uint32_t length);
	bool args_will_forward(uint32_t id, const uint32_t *args, uint32_t num_args, bool pure);
	std::string build_composite_combiner(uint32_t result_type, const uint32_t *elems, uint32_t length);
	bool remove_unity_swizzle(uint32_t base, std::string &op);
	std::string to_composite_constructor_expression(const SPIRType &parent_type, uint32_t id, bool block_like_type);
	std::string to_rerolled_array_expression(const SPIRType &parent_type, const std::string &expr, const SPIRType &type);
	std::string to_extract_constant_composite_expression(uint32_t result_type, const SPIRConstant &c,
	                                                     const uint32_t *chain, uint32_t length);

	bool should_suppress_usage_tracking(uint32_t id) const;

	void emit_copy_logical_type(uint32_t lhs_id, uint32_t lhs_type_id, uint32_t rhs_id, uint32_t rhs_type_id,
	                            SmallVector<uint32_t> chain);

	std::string to_unpacked_row_major_matrix_expression(uint32_t id);
	void emit_unary_op_cast(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op);
	void emit_bitfield_insert_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2,
	                             uint32_t op3, const char *op, SPIRType::BaseType offset_count_type);

	void emit_trinary_func_op_bitextract(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1,
	                                     uint32_t op2, const char *op, SPIRType::BaseType expected_result_type,
	                                     SPIRType::BaseType input_type0, SPIRType::BaseType input_type1,
	                                     SPIRType::BaseType input_type2);

	bool check_atomic_image(uint32_t id);

	void emit_atomic_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op);
	void emit_atomic_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2, const char *op);

	std::string convert_separate_image_to_expression(uint32_t id);
	std::string legacy_tex_op(const std::string &op, const SPIRType &imgtype, uint32_t id);
	bool subpass_input_is_framebuffer_fetch(uint32_t id) const;
	void emit_sparse_feedback_temporaries(uint32_t result_type_id, uint32_t id, uint32_t &feedback_id,
	                                      uint32_t &texel_id);

	void emit_spv_amd_shader_ballot_op(uint32_t result_type, uint32_t result_id, uint32_t op,
	                                           const uint32_t *args, uint32_t count);

	void emit_spv_amd_shader_explicit_vertex_parameter_op(uint32_t result_type, uint32_t result_id, uint32_t op,
	                                                              const uint32_t *args, uint32_t count);

	void emit_spv_amd_shader_trinary_minmax_op(uint32_t result_type, uint32_t result_id, uint32_t op,
	                                                   const uint32_t *args, uint32_t count);

	void emit_spv_amd_gcn_shader_op(uint32_t result_type, uint32_t result_id, uint32_t op, const uint32_t *args,
	                                        uint32_t count);

	// Overriten functions

	std::string CompilerGLSL_to_initializer_expression(const SPIRVariable &var);
	std::string CompilerGLSL_builtin_to_glsl(spv::BuiltIn builtin, spv::StorageClass storage);

	void CompilerGLSL_replace_illegal_names();
	void CompilerGLSL_replace_illegal_names(const std::unordered_set<std::string> &keywords);
	std::string CompilerGLSL_to_func_call_arg(const SPIRFunction::Parameter &arg, uint32_t id);

	void CompilerGLSL_emit_glsl_op(uint32_t result_type, uint32_t result_id, uint32_t op, const uint32_t *args,
	                          uint32_t count);

	void CompilerGLSL_emit_instruction(const Instruction &instr);

	inline bool is_legacy() const
	{
		return (options.es && options.version < 300) || (!options.es && options.version < 130);
	}

	inline bool is_legacy_desktop() const
	{
		return !options.es && options.version < 130;
	}
};
} // namespace SPIRV_CROSS_NAMESPACE

#endif
