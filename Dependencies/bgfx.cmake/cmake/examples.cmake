# Copyright (c) 2016 Joshua Brookover

include( CMakeParseArguments )

include( cmake/util/ConfigureDebugging.cmake )

include( cmake/3rdparty/ib-compress.cmake )
include( cmake/3rdparty/ocornut-imgui.cmake )

function( add_bgfx_shader FILE FOLDER )
	get_filename_component( FILENAME "${FILE}" NAME_WE )
	string( SUBSTRING "${FILENAME}" 0 2 TYPE )
	if( "${TYPE}" STREQUAL "fs" )
		set( TYPE "FRAGMENT" )
		set( D3D_PREFIX "ps" )
	elseif( "${TYPE}" STREQUAL "vs" )
		set( TYPE "VERTEX" )
		set( D3D_PREFIX "vs" )
	else()
		set( TYPE "" )
	endif()
	if( NOT "${TYPE}" STREQUAL "" )
		set( COMMON FILE ${FILE} OUTPUT ${BGFX_DIR}/examples/runtime/shaders/dx11/${FILENAME}.bin ${TYPE} INCLUDES ${BGFX_DIR}/src )
		shaderc( ${COMMON} WINDOWS PROFILE ${D3D_PREFIX}_4_0 )
		#shaderc( ${COMMON} LINUX   PROFILE 120 )
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
		file( GLOB GLOB_SOURCES ${DIR}/*.c ${DIR}/*.cpp ${DIR}/*.h ${DIR}/*.sc )
		list( APPEND SOURCES ${GLOB_SOURCES} )
		file( GLOB GLOB_SHADERS ${DIR}/*.sc )
		list( APPEND SHADERS ${GLOB_SHADERS} )
	endforeach()

	# Add target
	if( ARG_COMMON )
		add_library( example-${ARG_NAME} STATIC ${SOURCES} )
		target_compile_definitions( example-${ARG_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" )
		target_include_directories( example-${ARG_NAME} PUBLIC ${BGFX_DIR}/examples/common )
		target_link_libraries( example-${ARG_NAME} PUBLIC bgfx ib-compress ocornut-imgui )
	else()
		add_executable( example-${ARG_NAME} WIN32 ${SOURCES} )
		target_compile_definitions( example-${ARG_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" )
		target_link_libraries( example-${ARG_NAME} example-common )
		configure_debugging( example-${ARG_NAME} WORKING_DIR ${BGFX_DIR}/examples/runtime )
		if( MSVC )
			set_target_properties( example-${ARG_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
		endif()
	endif()

	# Configure shaders
	if( NOT ARG_COMMON )
		foreach( SHADER ${SHADERS} )
			add_bgfx_shader( ${SHADER} ${ARG_NAME} )
		endforeach()
		source_group( "Shader Files" FILES ${SHADERS})
	endif()

	# Directory name
	set_target_properties( example-${ARG_NAME} PROPERTIES FOLDER "examples" )
endfunction()

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
)

# Add examples
add_example( 00-helloworld )
add_example( 01-cubes )
add_example( 02-metaballs )
add_example( 03-raymarch )
add_example( 04-mesh )
add_example( 05-instancing )
add_example( 06-bump )
add_example( 07-callback )
add_example( 08-update )
add_example( 09-hdr )
add_example( 10-font )
add_example( 11-fontsdf )
add_example( 12-lod )
add_example( 13-stencil )
add_example( 14-shadowvolumes )
add_example( 15-shadowmaps-simple )
add_example( 16-shadowmaps )
add_example( 17-drawstress )
add_example( 18-ibl )
add_example( 19-oit )
add_example( 20-nanovg )
add_example( 21-deferred )
add_example( 22-windows )
add_example( 23-vectordisplay )
add_example( 24-nbody )
add_example( 25-c99 )
add_example( 26-occlusion )
add_example( 27-terrain )
add_example( 28-wireframe )
add_example( 29-debugdraw )
add_example( 30-picking )
add_example( 31-rsm )
