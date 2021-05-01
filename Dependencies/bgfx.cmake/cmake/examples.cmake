# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include( CMakeParseArguments )

include( cmake/util/ConfigureDebugging.cmake )

include( cmake/3rdparty/dear-imgui.cmake )
include( cmake/3rdparty/meshoptimizer.cmake )

function( add_bgfx_shader FILE FOLDER )
	get_filename_component( FILENAME "${FILE}" NAME_WE )
	string( SUBSTRING "${FILENAME}" 0 2 TYPE )
	if( "${TYPE}" STREQUAL "fs" )
		set( TYPE "FRAGMENT" )
		set( D3D_PREFIX "ps" )
	elseif( "${TYPE}" STREQUAL "vs" )
		set( TYPE "VERTEX" )
		set( D3D_PREFIX "vs" )
	elseif( "${TYPE}" STREQUAL "cs" )
		set( TYPE "COMPUTE" )
 		set( D3D_PREFIX "cs" )
	else()
		set( TYPE "" )
	endif()

	if( NOT "${TYPE}" STREQUAL "" )
		set( COMMON FILE ${FILE} ${TYPE} INCLUDES ${BGFX_DIR}/src )
		set( OUTPUTS "" )
		set( OUTPUTS_PRETTY "" )

		if( WIN32 )
			# dx9
			if( NOT "${TYPE}" STREQUAL "COMPUTE" )
				set( DX9_OUTPUT ${BGFX_DIR}/examples/runtime/shaders/dx9/${FILENAME}.bin )
				shaderc_parse( DX9 ${COMMON} WINDOWS PROFILE ${D3D_PREFIX}_3_0 O 3 OUTPUT ${DX9_OUTPUT} )
				list( APPEND OUTPUTS "DX9" )
				set( OUTPUTS_PRETTY "${OUTPUTS_PRETTY}DX9, " )
			endif()

			# dx11
			set( DX11_OUTPUT ${BGFX_DIR}/examples/runtime/shaders/dx11/${FILENAME}.bin )
			if( NOT "${TYPE}" STREQUAL "COMPUTE" )
				shaderc_parse( DX11 ${COMMON} WINDOWS PROFILE ${D3D_PREFIX}_5_0 O 3 OUTPUT ${DX11_OUTPUT} )
			else()
				shaderc_parse( DX11 ${COMMON} WINDOWS PROFILE ${D3D_PREFIX}_5_0 O 1 OUTPUT ${DX11_OUTPUT} )
			endif()
			list( APPEND OUTPUTS "DX11" )
			set( OUTPUTS_PRETTY "${OUTPUTS_PRETTY}DX11, " )
		endif()

		if( APPLE )
			# metal
			set( METAL_OUTPUT ${BGFX_DIR}/examples/runtime/shaders/metal/${FILENAME}.bin )
			shaderc_parse( METAL ${COMMON} OSX PROFILE metal OUTPUT ${METAL_OUTPUT} )
			list( APPEND OUTPUTS "METAL" )
			set( OUTPUTS_PRETTY "${OUTPUTS_PRETTY}Metal, " )
		endif()

		# essl
		if( NOT "${TYPE}" STREQUAL "COMPUTE" )
			set( ESSL_OUTPUT ${BGFX_DIR}/examples/runtime/shaders/essl/${FILENAME}.bin )
			shaderc_parse( ESSL ${COMMON} ANDROID OUTPUT ${ESSL_OUTPUT} )
			list( APPEND OUTPUTS "ESSL" )
			set( OUTPUTS_PRETTY "${OUTPUTS_PRETTY}ESSL, " )
		endif()

		# glsl
		set( GLSL_OUTPUT ${BGFX_DIR}/examples/runtime/shaders/glsl/${FILENAME}.bin )
		if( NOT "${TYPE}" STREQUAL "COMPUTE" )
			shaderc_parse( GLSL ${COMMON} LINUX PROFILE 120 OUTPUT ${GLSL_OUTPUT} )
		else()
			shaderc_parse( GLSL ${COMMON} LINUX PROFILE 430 OUTPUT ${GLSL_OUTPUT} )
		endif()
		list( APPEND OUTPUTS "GLSL" )
		set( OUTPUTS_PRETTY "${OUTPUTS_PRETTY}GLSL, " )

		# spirv
		if( NOT "${TYPE}" STREQUAL "COMPUTE" )
			set( SPIRV_OUTPUT ${BGFX_DIR}/examples/runtime/shaders/spirv/${FILENAME}.bin )
			shaderc_parse( SPIRV ${COMMON} LINUX PROFILE spirv OUTPUT ${SPIRV_OUTPUT} )
			list( APPEND OUTPUTS "SPIRV" )
			set( OUTPUTS_PRETTY "${OUTPUTS_PRETTY}SPIRV" )
			set( OUTPUT_FILES "" )
			set( COMMANDS "" )
		endif()

		foreach( OUT ${OUTPUTS} )
			list( APPEND OUTPUT_FILES ${${OUT}_OUTPUT} )
			list( APPEND COMMANDS COMMAND "$<TARGET_FILE:shaderc>" ${${OUT}} )
			get_filename_component( OUT_DIR ${${OUT}_OUTPUT} DIRECTORY )
			file( MAKE_DIRECTORY ${OUT_DIR} )
		endforeach()

		file( RELATIVE_PATH PRINT_NAME ${BGFX_DIR}/examples ${FILE} )
		add_custom_command(
			MAIN_DEPENDENCY
			${FILE}
			OUTPUT
			${OUTPUT_FILES}
			${COMMANDS}
			COMMENT "Compiling shader ${PRINT_NAME} for ${OUTPUTS_PRETTY}"
		)
	endif()
endfunction()

function( add_example ARG_NAME )
	# Parse arguments
	cmake_parse_arguments( ARG "COMMON" "" "DIRECTORIES;SOURCES" ${ARGN} )

	# Get all source files
	list( APPEND ARG_DIRECTORIES "${BGFX_DIR}/examples/${ARG_NAME}" )
	set( SOURCES "" )
	set( SHADERS "" )
	foreach( DIR ${ARG_DIRECTORIES} )
		if( APPLE )
			file( GLOB GLOB_SOURCES ${DIR}/*.mm )
			list( APPEND SOURCES ${GLOB_SOURCES} )
		endif()
		file( GLOB GLOB_SOURCES ${DIR}/*.c ${DIR}/*.cpp ${DIR}/*.h ${DIR}/*.sc )
		list( APPEND SOURCES ${GLOB_SOURCES} )
		file( GLOB GLOB_SHADERS ${DIR}/*.sc )
		list( APPEND SHADERS ${GLOB_SHADERS} )
	endforeach()

	# Add target
	if( ARG_COMMON )
		add_library( example-${ARG_NAME} STATIC EXCLUDE_FROM_ALL ${SOURCES} )
		target_include_directories( example-${ARG_NAME} PUBLIC ${BGFX_DIR}/examples/common )
		target_link_libraries( example-${ARG_NAME} PUBLIC bgfx bx bimg dear-imgui meshoptimizer )
		if( BGFX_WITH_GLFW )
			find_package( glfw3 REQUIRED )
			target_link_libraries( example-${ARG_NAME} PUBLIC glfw )
			target_compile_definitions( example-${ARG_NAME} PUBLIC ENTRY_CONFIG_USE_GLFW )
		elseif( BGFX_WITH_SDL )
			find_package( SDL2 REQUIRED )
			target_link_libraries( example-${ARG_NAME} PUBLIC ${SDL2_LIBRARIES} )
			target_compile_definitions( example-${ARG_NAME} PUBLIC ENTRY_CONFIG_USE_SDL )
		elseif( UNIX AND NOT APPLE )
			target_link_libraries( example-${ARG_NAME} PUBLIC X11 )
		endif()
	else()
		if( BGFX_INSTALL_EXAMPLES )
			add_executable( example-${ARG_NAME} WIN32 ${SOURCES} )
		else()
			add_executable( example-${ARG_NAME} WIN32 EXCLUDE_FROM_ALL ${SOURCES} )
		endif()
		target_link_libraries( example-${ARG_NAME} example-common )
		configure_debugging( example-${ARG_NAME} WORKING_DIR ${BGFX_DIR}/examples/runtime )
		if( MSVC )
			set_target_properties( example-${ARG_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
		endif()
		if( BGFX_CUSTOM_TARGETS )
			add_dependencies( examples example-${ARG_NAME} )
		endif()
		if( IOS )
			set_target_properties(example-${ARG_NAME} PROPERTIES MACOSX_BUNDLE ON
													  MACOSX_BUNDLE_GUI_IDENTIFIER example-${ARG_NAME}
													  MACOSX_BUNDLE_BUNDLE_VERSION 0
													  MACOSX_BUNDLE_SHORT_VERSION_STRING 0
													  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
		endif()
	endif()
	target_compile_definitions( example-${ARG_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" "-DENTRY_CONFIG_IMPLEMENT_MAIN=1" )

	# Configure shaders
	if( NOT ARG_COMMON AND NOT IOS AND NOT EMSCRIPTEN)
		foreach( SHADER ${SHADERS} )
			add_bgfx_shader( ${SHADER} ${ARG_NAME} )
		endforeach()
		source_group( "Shader Files" FILES ${SHADERS})
	endif()

	if (NOT ARG_COMMON AND EMSCRIPTEN)
		target_link_libraries(example-${ARG_NAME}
			"-s PRECISE_F32=1"
			"-s TOTAL_MEMORY=268435456"
			"--memory-init-file 1")
	endif()

	# Directory name
	set_target_properties( example-${ARG_NAME} PROPERTIES FOLDER "bgfx/examples" )

	if (IOS OR WIN32)
		# on iOS we need to build a bundle so have to copy the data rather than symlink
		# and on windows we can't create symlinks
		add_custom_command( TARGET example-${ARG_NAME} COMMAND ${CMAKE_COMMAND} -E copy_directory ${BGFX_DIR}/examples/runtime/ $<TARGET_FILE_DIR:example-${ARG_NAME}>)
	else()
		# For everything else symlink some folders into our output directory
		add_custom_command( TARGET example-${ARG_NAME} COMMAND ${CMAKE_COMMAND} -E create_symlink ${BGFX_DIR}/examples/runtime/font $<TARGET_FILE_DIR:example-${ARG_NAME}>/font)
		add_custom_command( TARGET example-${ARG_NAME} COMMAND ${CMAKE_COMMAND} -E create_symlink ${BGFX_DIR}/examples/runtime/images $<TARGET_FILE_DIR:example-${ARG_NAME}>/images)
		add_custom_command( TARGET example-${ARG_NAME} COMMAND ${CMAKE_COMMAND} -E create_symlink ${BGFX_DIR}/examples/runtime/meshes $<TARGET_FILE_DIR:example-${ARG_NAME}>/meshes)
		add_custom_command( TARGET example-${ARG_NAME} COMMAND ${CMAKE_COMMAND} -E create_symlink ${BGFX_DIR}/examples/runtime/shaders $<TARGET_FILE_DIR:example-${ARG_NAME}>/shaders)
		add_custom_command( TARGET example-${ARG_NAME} COMMAND ${CMAKE_COMMAND} -E create_symlink ${BGFX_DIR}/examples/runtime/text $<TARGET_FILE_DIR:example-${ARG_NAME}>/text)
		add_custom_command( TARGET example-${ARG_NAME} COMMAND ${CMAKE_COMMAND} -E create_symlink ${BGFX_DIR}/examples/runtime/textures $<TARGET_FILE_DIR:example-${ARG_NAME}>/textures)
	endif()

endfunction()

# Build all examples target
if( BGFX_CUSTOM_TARGETS )
	add_custom_target( examples )
	set_target_properties( examples PROPERTIES FOLDER "bgfx/examples" )
endif()

# Add common library for examples
add_example(
	common
	COMMON
	DIRECTORIES
	${BGFX_DIR}/examples/common/debugdraw
	${BGFX_DIR}/examples/common/entry
	${BGFX_DIR}/examples/common/font
	${BGFX_DIR}/examples/common/imgui
	${BGFX_DIR}/examples/common/nanovg
	${BGFX_DIR}/examples/common/ps
)

# Only add examples if set, otherwise we still need exmaples common for tools
if( BGFX_BUILD_EXAMPLES )
	# Add examples
	set(
		BGFX_EXAMPLES
		00-helloworld
		01-cubes
		02-metaballs
		03-raymarch
		04-mesh
		05-instancing
		06-bump
		07-callback
		08-update
		09-hdr
		10-font
		11-fontsdf
		12-lod
		13-stencil
		14-shadowvolumes
		15-shadowmaps-simple
		16-shadowmaps
		17-drawstress
		18-ibl
		19-oit
		20-nanovg
#		21-deferred
		22-windows
		23-vectordisplay
		24-nbody
		25-c99
		26-occlusion
		27-terrain
		28-wireframe
		29-debugdraw
		30-picking
		31-rsm
		32-particles
		33-pom
		34-mvs
		35-dynamic
		36-sky
#		37-gpudrivenrendering
		38-bloom
		39-assao
		40-svt
#		41-tess
		42-bunnylod
		43-denoise
		44-sss
		45-bokeh
	)

	foreach( EXAMPLE ${BGFX_EXAMPLES} )
		add_example( ${EXAMPLE} )
	endforeach()
endif()
