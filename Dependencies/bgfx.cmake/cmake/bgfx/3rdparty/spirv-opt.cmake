# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

# Ensure the directory exists
if(NOT IS_DIRECTORY ${BGFX_DIR})
	message(SEND_ERROR "Could not load bgfx, directory does not exist. ${BGFX_DIR}")
	return()
endif()

set(SPIRV_HEADERS ${BGFX_DIR}/3rdparty/spirv-headers)
set(SPIRV_TOOLS ${BGFX_DIR}/3rdparty/spirv-tools)

file(
	GLOB
	SPIRV_OPT_SOURCES
	# libspirv
	${SPIRV_TOOLS}/source/opt/*.cpp
	${SPIRV_TOOLS}/source/opt/*.h
	${SPIRV_TOOLS}/source/reduce/*.cpp
	${SPIRV_TOOLS}/source/reduce/*.h
	${SPIRV_TOOLS}/source/assembly_grammar.cpp
	${SPIRV_TOOLS}/source/assembly_grammar.h
	${SPIRV_TOOLS}/source/binary.cpp
	${SPIRV_TOOLS}/source/binary.h
	${SPIRV_TOOLS}/source/cfa.h
	${SPIRV_TOOLS}/source/diagnostic.cpp
	${SPIRV_TOOLS}/source/diagnostic.h
	${SPIRV_TOOLS}/source/disassemble.cpp
	${SPIRV_TOOLS}/source/disassemble.h
	${SPIRV_TOOLS}/source/enum_set.h
	${SPIRV_TOOLS}/source/enum_string_mapping.cpp
	${SPIRV_TOOLS}/source/enum_string_mapping.h
	${SPIRV_TOOLS}/source/ext_inst.cpp
	${SPIRV_TOOLS}/source/ext_inst.h
	${SPIRV_TOOLS}/source/extensions.cpp
	${SPIRV_TOOLS}/source/extensions.h
	${SPIRV_TOOLS}/source/instruction.h
	${SPIRV_TOOLS}/source/latest_version_glsl_std_450_header.h
	${SPIRV_TOOLS}/source/latest_version_opencl_std_header.h
	${SPIRV_TOOLS}/source/latest_version_spirv_header.h
	${SPIRV_TOOLS}/source/libspirv.cpp
	${SPIRV_TOOLS}/source/macro.h
	${SPIRV_TOOLS}/source/name_mapper.cpp
	${SPIRV_TOOLS}/source/name_mapper.h
	${SPIRV_TOOLS}/source/opcode.cpp
	${SPIRV_TOOLS}/source/opcode.h
	${SPIRV_TOOLS}/source/operand.cpp
	${SPIRV_TOOLS}/source/operand.h
	${SPIRV_TOOLS}/source/parsed_operand.cpp
	${SPIRV_TOOLS}/source/parsed_operand.h
	${SPIRV_TOOLS}/source/print.cpp
	${SPIRV_TOOLS}/source/print.h
	${SPIRV_TOOLS}/source/software_version.cpp
	${SPIRV_TOOLS}/source/spirv_constant.h
	${SPIRV_TOOLS}/source/spirv_definition.h
	${SPIRV_TOOLS}/source/spirv_endian.cpp
	${SPIRV_TOOLS}/source/spirv_endian.h
	${SPIRV_TOOLS}/source/spirv_optimizer_options.cpp
	${SPIRV_TOOLS}/source/spirv_reducer_options.cpp
	${SPIRV_TOOLS}/source/spirv_target_env.cpp
	${SPIRV_TOOLS}/source/spirv_target_env.h
	${SPIRV_TOOLS}/source/spirv_validator_options.cpp
	${SPIRV_TOOLS}/source/spirv_validator_options.h
	${SPIRV_TOOLS}/source/table.cpp
	${SPIRV_TOOLS}/source/table.h
	${SPIRV_TOOLS}/source/text.cpp
	${SPIRV_TOOLS}/source/text.h
	${SPIRV_TOOLS}/source/text_handler.cpp
	${SPIRV_TOOLS}/source/text_handler.h
	${SPIRV_TOOLS}/source/util/bit_vector.cpp
	${SPIRV_TOOLS}/source/util/bit_vector.h
	${SPIRV_TOOLS}/source/util/bitutils.h
	${SPIRV_TOOLS}/source/util/hex_float.h
	${SPIRV_TOOLS}/source/util/parse_number.cpp
	${SPIRV_TOOLS}/source/util/parse_number.h
	${SPIRV_TOOLS}/source/util/string_utils.cpp
	${SPIRV_TOOLS}/source/util/string_utils.h
	${SPIRV_TOOLS}/source/util/timer.h
	${SPIRV_TOOLS}/source/val/basic_block.cpp
	${SPIRV_TOOLS}/source/val/construct.cpp
	${SPIRV_TOOLS}/source/val/decoration.h
	${SPIRV_TOOLS}/source/val/function.cpp
	${SPIRV_TOOLS}/source/val/instruction.cpp
	${SPIRV_TOOLS}/source/val/validate.cpp
	${SPIRV_TOOLS}/source/val/validate.h
	${SPIRV_TOOLS}/source/val/validate_adjacency.cpp
	${SPIRV_TOOLS}/source/val/validate_annotation.cpp
	${SPIRV_TOOLS}/source/val/validate_arithmetics.cpp
	${SPIRV_TOOLS}/source/val/validate_atomics.cpp
	${SPIRV_TOOLS}/source/val/validate_barriers.cpp
	${SPIRV_TOOLS}/source/val/validate_bitwise.cpp
	${SPIRV_TOOLS}/source/val/validate_builtins.cpp
	${SPIRV_TOOLS}/source/val/validate_capability.cpp
	${SPIRV_TOOLS}/source/val/validate_cfg.cpp
	${SPIRV_TOOLS}/source/val/validate_composites.cpp
	${SPIRV_TOOLS}/source/val/validate_constants.cpp
	${SPIRV_TOOLS}/source/val/validate_conversion.cpp
	${SPIRV_TOOLS}/source/val/validate_debug.cpp
	${SPIRV_TOOLS}/source/val/validate_decorations.cpp
	${SPIRV_TOOLS}/source/val/validate_derivatives.cpp
	${SPIRV_TOOLS}/source/val/validate_execution_limitations.cpp
	${SPIRV_TOOLS}/source/val/validate_extensions.cpp
	${SPIRV_TOOLS}/source/val/validate_function.cpp
	${SPIRV_TOOLS}/source/val/validate_id.cpp
	${SPIRV_TOOLS}/source/val/validate_image.cpp
	${SPIRV_TOOLS}/source/val/validate_instruction.cpp
	${SPIRV_TOOLS}/source/val/validate_interfaces.cpp
	${SPIRV_TOOLS}/source/val/validate_layout.cpp
	${SPIRV_TOOLS}/source/val/validate_literals.cpp
	${SPIRV_TOOLS}/source/val/validate_logicals.cpp
	${SPIRV_TOOLS}/source/val/validate_memory.cpp
	${SPIRV_TOOLS}/source/val/validate_memory_semantics.cpp
	${SPIRV_TOOLS}/source/val/validate_mesh_shading.cpp
	${SPIRV_TOOLS}/source/val/validate_misc.cpp
	${SPIRV_TOOLS}/source/val/validate_mode_setting.cpp
	${SPIRV_TOOLS}/source/val/validate_non_uniform.cpp
	${SPIRV_TOOLS}/source/val/validate_primitives.cpp
	${SPIRV_TOOLS}/source/val/validate_ray_query.cpp
	${SPIRV_TOOLS}/source/val/validate_ray_tracing.cpp
	${SPIRV_TOOLS}/source/val/validate_ray_tracing_reorder.cpp
	${SPIRV_TOOLS}/source/val/validate_scopes.cpp
	${SPIRV_TOOLS}/source/val/validate_small_type_uses.cpp
	${SPIRV_TOOLS}/source/val/validate_type.cpp
	${SPIRV_TOOLS}/source/val/validation_state.cpp
)

add_library(spirv-opt STATIC ${SPIRV_OPT_SOURCES})

# Put in a "bgfx" folder in Visual Studio
set_target_properties(spirv-opt PROPERTIES FOLDER "bgfx")

target_include_directories(
	spirv-opt
	PUBLIC ${SPIRV_TOOLS}/include #
	PRIVATE ${SPIRV_TOOLS} #
			${SPIRV_TOOLS}/include/generated #
			${SPIRV_TOOLS}/source #
			${SPIRV_HEADERS}/include #
)
