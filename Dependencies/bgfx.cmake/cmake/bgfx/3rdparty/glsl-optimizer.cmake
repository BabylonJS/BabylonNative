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

set(GLSL_OPTIMIZER ${BGFX_DIR}/3rdparty/glsl-optimizer)

file(
	GLOB
	GLSL_OPTIMIZER_SOURCES
	${GLSL_OPTIMIZER}/src/glsl/glcpp/glcpp.h
	${GLSL_OPTIMIZER}/src/glsl/glcpp/glcpp-lex.c
	${GLSL_OPTIMIZER}/src/glsl/glcpp/glcpp-parse.c
	${GLSL_OPTIMIZER}/src/glsl/glcpp/glcpp-parse.h
	${GLSL_OPTIMIZER}/src/glsl/glcpp/pp.c
	${GLSL_OPTIMIZER}/src/glsl/ast.h
	${GLSL_OPTIMIZER}/src/glsl/ast_array_index.cpp
	${GLSL_OPTIMIZER}/src/glsl/ast_expr.cpp
	${GLSL_OPTIMIZER}/src/glsl/ast_function.cpp
	${GLSL_OPTIMIZER}/src/glsl/ast_to_hir.cpp
	${GLSL_OPTIMIZER}/src/glsl/ast_type.cpp
	${GLSL_OPTIMIZER}/src/glsl/builtin_functions.cpp
	${GLSL_OPTIMIZER}/src/glsl/builtin_type_macros.h
	${GLSL_OPTIMIZER}/src/glsl/builtin_types.cpp
	${GLSL_OPTIMIZER}/src/glsl/builtin_variables.cpp
	${GLSL_OPTIMIZER}/src/glsl/glsl_lexer.cpp
	${GLSL_OPTIMIZER}/src/glsl/glsl_optimizer.cpp
	${GLSL_OPTIMIZER}/src/glsl/glsl_optimizer.h
	${GLSL_OPTIMIZER}/src/glsl/glsl_parser.cpp
	${GLSL_OPTIMIZER}/src/glsl/glsl_parser.h
	${GLSL_OPTIMIZER}/src/glsl/glsl_parser_extras.cpp
	${GLSL_OPTIMIZER}/src/glsl/glsl_parser_extras.h
	${GLSL_OPTIMIZER}/src/glsl/glsl_symbol_table.cpp
	${GLSL_OPTIMIZER}/src/glsl/glsl_symbol_table.h
	${GLSL_OPTIMIZER}/src/glsl/glsl_types.cpp
	${GLSL_OPTIMIZER}/src/glsl/glsl_types.h
	${GLSL_OPTIMIZER}/src/glsl/hir_field_selection.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir.h
	${GLSL_OPTIMIZER}/src/glsl/ir_basic_block.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_basic_block.h
	${GLSL_OPTIMIZER}/src/glsl/ir_builder.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_builder.h
	${GLSL_OPTIMIZER}/src/glsl/ir_clone.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_constant_expression.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_equals.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_expression_flattening.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_expression_flattening.h
	${GLSL_OPTIMIZER}/src/glsl/ir_function.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_function_can_inline.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_function_detect_recursion.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_function_inlining.h
	${GLSL_OPTIMIZER}/src/glsl/ir_hierarchical_visitor.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_hierarchical_visitor.h
	${GLSL_OPTIMIZER}/src/glsl/ir_hv_accept.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_import_prototypes.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_optimization.h
	${GLSL_OPTIMIZER}/src/glsl/ir_print_glsl_visitor.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_print_glsl_visitor.h
	${GLSL_OPTIMIZER}/src/glsl/ir_print_metal_visitor.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_print_metal_visitor.h
	${GLSL_OPTIMIZER}/src/glsl/ir_print_visitor.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_print_visitor.h
	${GLSL_OPTIMIZER}/src/glsl/ir_rvalue_visitor.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_rvalue_visitor.h
	${GLSL_OPTIMIZER}/src/glsl/ir_stats.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_stats.h
	${GLSL_OPTIMIZER}/src/glsl/ir_uniform.h
	${GLSL_OPTIMIZER}/src/glsl/ir_unused_structs.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_unused_structs.h
	${GLSL_OPTIMIZER}/src/glsl/ir_validate.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_variable_refcount.cpp
	${GLSL_OPTIMIZER}/src/glsl/ir_variable_refcount.h
	${GLSL_OPTIMIZER}/src/glsl/ir_visitor.h
	${GLSL_OPTIMIZER}/src/glsl/link_atomics.cpp
	${GLSL_OPTIMIZER}/src/glsl/link_functions.cpp
	${GLSL_OPTIMIZER}/src/glsl/link_interface_blocks.cpp
	${GLSL_OPTIMIZER}/src/glsl/link_uniform_block_active_visitor.cpp
	${GLSL_OPTIMIZER}/src/glsl/link_uniform_block_active_visitor.h
	${GLSL_OPTIMIZER}/src/glsl/link_uniform_blocks.cpp
	${GLSL_OPTIMIZER}/src/glsl/link_uniform_initializers.cpp
	${GLSL_OPTIMIZER}/src/glsl/link_uniforms.cpp
	${GLSL_OPTIMIZER}/src/glsl/link_varyings.cpp
	${GLSL_OPTIMIZER}/src/glsl/link_varyings.h
	${GLSL_OPTIMIZER}/src/glsl/linker.cpp
	${GLSL_OPTIMIZER}/src/glsl/linker.h
	${GLSL_OPTIMIZER}/src/glsl/list.h
	${GLSL_OPTIMIZER}/src/glsl/loop_analysis.cpp
	${GLSL_OPTIMIZER}/src/glsl/loop_analysis.h
	${GLSL_OPTIMIZER}/src/glsl/loop_controls.cpp
	${GLSL_OPTIMIZER}/src/glsl/loop_unroll.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_clip_distance.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_discard.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_discard_flow.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_if_to_cond_assign.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_instructions.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_jumps.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_mat_op_to_vec.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_named_interface_blocks.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_noise.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_offset_array.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_output_reads.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_packed_varyings.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_packing_builtins.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_ubo_reference.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_variable_index_to_cond_assign.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_vec_index_to_cond_assign.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_vec_index_to_swizzle.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_vector.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_vector_insert.cpp
	${GLSL_OPTIMIZER}/src/glsl/lower_vertex_id.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_algebraic.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_array_splitting.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_constant_folding.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_constant_propagation.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_constant_variable.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_copy_propagation.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_copy_propagation_elements.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_cse.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_dead_builtin_variables.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_dead_builtin_varyings.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_dead_code.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_dead_code_local.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_dead_functions.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_flatten_nested_if_blocks.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_flip_matrices.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_function_inlining.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_if_simplification.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_minmax.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_noop_swizzle.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_rebalance_tree.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_redundant_jumps.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_structure_splitting.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_swizzle_swizzle.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_tree_grafting.cpp
	${GLSL_OPTIMIZER}/src/glsl/opt_vectorize.cpp
	${GLSL_OPTIMIZER}/src/glsl/program.h
	${GLSL_OPTIMIZER}/src/glsl/s_expression.cpp
	${GLSL_OPTIMIZER}/src/glsl/s_expression.h
	${GLSL_OPTIMIZER}/src/glsl/standalone_scaffolding.cpp
	${GLSL_OPTIMIZER}/src/glsl/standalone_scaffolding.h
	${GLSL_OPTIMIZER}/src/glsl/strtod.c
	${GLSL_OPTIMIZER}/src/glsl/strtod.h
	${GLSL_OPTIMIZER}/src/mesa/main/compiler.h
	${GLSL_OPTIMIZER}/src/mesa/main/config.h
	${GLSL_OPTIMIZER}/src/mesa/main/context.h
	${GLSL_OPTIMIZER}/src/mesa/main/core.h
	${GLSL_OPTIMIZER}/src/mesa/main/dd.h
	${GLSL_OPTIMIZER}/src/mesa/main/errors.h
	${GLSL_OPTIMIZER}/src/mesa/main/glheader.h
	${GLSL_OPTIMIZER}/src/mesa/main/glminimal.h
	${GLSL_OPTIMIZER}/src/mesa/main/imports.c
	${GLSL_OPTIMIZER}/src/mesa/main/imports.h
	${GLSL_OPTIMIZER}/src/mesa/main/macros.h
	${GLSL_OPTIMIZER}/src/mesa/main/mtypes.h
	${GLSL_OPTIMIZER}/src/mesa/main/simple_list.h
	${GLSL_OPTIMIZER}/src/mesa/program/hash_table.h
	${GLSL_OPTIMIZER}/src/mesa/program/prog_hash_table.c
	${GLSL_OPTIMIZER}/src/mesa/program/prog_instruction.h
	${GLSL_OPTIMIZER}/src/mesa/program/prog_parameter.h
	${GLSL_OPTIMIZER}/src/mesa/program/prog_statevars.h
	${GLSL_OPTIMIZER}/src/mesa/program/symbol_table.c
	${GLSL_OPTIMIZER}/src/mesa/program/symbol_table.h
	${GLSL_OPTIMIZER}/src/util/hash_table.c
	${GLSL_OPTIMIZER}/src/util/hash_table.h
	${GLSL_OPTIMIZER}/src/util/macros.h
	${GLSL_OPTIMIZER}/src/util/ralloc.c
	${GLSL_OPTIMIZER}/src/util/ralloc.h
)

add_library(glsl-optimizer STATIC ${GLSL_OPTIMIZER_SOURCES})

# Put in a "bgfx" folder in Visual Studio
set_target_properties(glsl-optimizer PROPERTIES FOLDER "bgfx")

target_include_directories(
	glsl-optimizer
	PUBLIC ${GLSL_OPTIMIZER}/include #
		   ${GLSL_OPTIMIZER}/src/glsl #
	PRIVATE ${GLSL_OPTIMIZER}/src #
			${GLSL_OPTIMIZER}/src/mesa #
			${GLSL_OPTIMIZER}/src/mapi #
)

if(MSVC)
	target_compile_definitions(
		glsl-optimizer
		PRIVATE "__STDC__" #
				"__STDC_VERSION__=199901L" #
				"strdup=_strdup" #
				"alloca=_alloca" #
				"isascii=__isascii" #
	)
	target_compile_options(
		glsl-optimizer
		PRIVATE
			"/wd4100" # error C4100: '' : unreferenced formal parameter
			"/wd4127" # warning C4127: conditional expression is constant
			"/wd4132" # warning C4132: 'deleted_key_value': const object should be initialized
			"/wd4189" # warning C4189: 'interface_type': local variable is initialized but not referenced
			"/wd4204" # warning C4204: nonstandard extension used: non-constant aggregate initializer
			"/wd4244" # warning C4244: '=': conversion from 'const flex_int32_t' to 'YY_CHAR', possible loss of data
			"/wd4389" # warning C4389: '!=': signed/unsigned mismatch
			"/wd4245" # warning C4245: 'return': conversion from 'int' to 'unsigned int', signed/unsigned mismatch
			"/wd4701" # warning C4701: potentially uninitialized local variable 'lower' used
			"/wd4702" # warning C4702: unreachable code
			"/wd4706" # warning C4706: assignment within conditional expression
			"/wd4996" # warning C4996: 'strdup': The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: _strdup.
	)
else()
	target_compile_options(
		glsl-optimizer
		PRIVATE "-fno-strict-aliasing" # glsl-optimizer has bugs if strict aliasing is used.
				#
				"-Wno-implicit-fallthrough" #
				"-Wno-parentheses" #
				"-Wno-sign-compare" #
				"-Wno-unused-function" #
				"-Wno-unused-parameter" #
	)
endif()

if(XCODE)
	target_compile_options(
		glsl-optimizer PRIVATE #
							   "-Wno-deprecated-register" #
	)
endif()

if(MINGW)
	target_compile_options(
		glsl-optimizer PRIVATE #
							   "-Wno-misleading-indentation" #
	)
endif()
