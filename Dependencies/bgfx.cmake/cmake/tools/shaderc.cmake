# Copyright (c) 2016 Joshua Brookover

include( CMakeParseArguments )

include( cmake/3rdparty/fcpp.cmake )
include( cmake/3rdparty/glsl-optimizer.cmake )

add_executable( shaderc bgfx/tools/shaderc/shaderc.cpp bgfx/tools/shaderc/shaderc.h bgfx/tools/shaderc/shaderc_glsl.cpp bgfx/tools/shaderc/shaderc_hlsl.cpp )
target_compile_definitions( shaderc PRIVATE "-D_CRT_SECURE_NO_WARNINGS" )
set_target_properties( shaderc PROPERTIES FOLDER "bgfx/tools" )
target_link_libraries( shaderc bx bgfx fcpp glsl-optimizer )

# shaderc( FILE file OUTPUT file ... )
# See shaderc_parse() below for inputs
function( shaderc )
	cmake_parse_arguments( ARG "" "FILE;OUTPUT" "" ${ARGN} )
	shaderc_parse( CLI ${ARGN} )
	add_custom_command( OUTPUT ${ARG_OUTPUT} COMMAND "$<TARGET_FILE:shaderc>" ${CLI} MAIN_DEPENDENCY ${ARG_FILE} COMMENT "Compiling shader ${ARG_FILE}" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" )
endfunction()

# shaderc_parse(
#	FILE filename
#	OUTPUT filename
#	FRAGMENT|VERTEX
#	ANDROID|ASM_JS|IOS|LINUX|NACL|OSX|WINDOWS
#	[PROFILE profile]
#	[O 0|1|2|3]
#	[VARYINGDEF filename]
#	[BIN2C filename]
#	[INCLUDES include;include]
#	[DEFINES include;include]
#	[DEPENDS]
#	[PREPROCESS]
#	[RAW]
#	[VERBOSE]
#	[DEBUG]
#	[DISASM]
#	[WERROR]
# )
function( shaderc_parse ARG_OUT )
	cmake_parse_arguments( ARG "DEPENDS;ANDROID;ASM_JS;IOS;LINUX;NACL;OSX;WINDOWS;PREPROCESS;RAW;FRAGMENT;VERTEX;VERBOSE;DEBUG;DISASM;WERROR" "FILE;OUTPUT;VARYINGDEF;BIN2C;PROFILE;O" "INCLUDES;DEFINES" ${ARGN} )
	set( CLI "" )

	# -f
	if( ARG_FILE )
		list( APPEND CLI "-f" "${ARG_FILE}" )
	endif()

	# -i
	if( ARG_INCLUDES )
		list( APPEND CLI "-i" )
		set( INCLUDES "" )
		foreach( INCLUDE ${ARG_INCLUDES} )
			if( NOT "${INCLUDES}" STREQUAL "" )
				set( INCLUDES "${INCLUDES}\\\\;${INCLUDE}" )
			else()
				set( INCLUDES "${INCLUDE}" )
			endif()
		endforeach()
		list( APPEND CLI "${INCLUDES}" )
	endif()

	# -o
	if( ARG_OUTPUT )
		list( APPEND CLI "-o" "${ARG_OUTPUT}" )
	endif()

	# --bin2c
	if( ARG_BIN2C )
		list( APPEND CLI "--bin2c" "${ARG_BIN2C}" )
	endif()

	# --depends
	if( ARG_DEPENDS )
		list( APPEND CLI "--depends" )
	endif()

	# --platform
	set( PLATFORM "" )
	set( PLATFORMS "ANDROID;ASM_JS;IOS;LINUX;NACL;OSX;WINDOWS" )
	foreach( P ${PLATFORMS} )
		if( ARG_${P} )
			if( PLATFORM )
				message( SEND_ERROR "Call to shaderc_parse() cannot have both flags ${PLATFORM} and ${P}." )
				return()
			endif()
			set( PLATFORM "${P}" )
		endif()
	endforeach()
	if( "${PLATFORM}" STREQUAL "" )
		message( SEND_ERROR "Call to shaderc_parse() must have a platform flag: ${PLATFORMS}" )
		return()
	elseif( "${PLATFORM}" STREQUAL "ANDROID" )
		list( APPEND CLI "--platform" "android" )
	elseif( "${PLATFORM}" STREQUAL "ASM_JS" )
		list( APPEND CLI "--platform" "asm.js" )
	elseif( "${PLATFORM}" STREQUAL "IOS" )
		list( APPEND CLI "--platform" "ios" )
	elseif( "${PLATFORM}" STREQUAL "LINUX" )
		list( APPEND CLI "--platform" "linux" )
	elseif( "${PLATFORM}" STREQUAL "NACL" )
		list( APPEND CLI "--platform" "nacl" )
	elseif( "${PLATFORM}" STREQUAL "OSX" )
		list( APPEND CLI "--platform" "osx" )
	elseif( "${PLATFORM}" STREQUAL "WINDOWS" )
		list( APPEND CLI "--platform" "windows" )
	endif()

	# --preprocess
	if( ARG_PREPROCESS )
		list( APPEND CLI "--preprocess" )
	endif()

	# --define
	if( ARG_DEFINES )
		list( APPEND CLI "--defines" )
		set( DEFINES "" )
		foreach( DEFINE ${ARG_DEFINES} )
			if( NOT "${DEFINES}" STREQUAL "" )
				set( DEFINES "${DEFINES}\\\\;${DEFINE}" )
			else()
				set( DEFINES "${DEFINE}" )
			endif()
		endforeach()
		list( APPEND CLI "${DEFINES}" )
	endif()

	# --raw
	if( ARG_RAW )
		list( APPEND CLI "--raw" )
	endif()

	# --type
	set( TYPE "" )
	set( TYPES "FRAGMENT;VERTEX" )
	foreach( T ${TYPES} )
		if( ARG_${T} )
			if( TYPE )
				message( SEND_ERROR "Call to shaderc_parse() cannot have both flags ${TYPE} and ${T}." )
				return()
			endif()
			set( TYPE "${T}" )
		endif()
	endforeach()
	if( "${TYPE}" STREQUAL "" )
		message( SEND_ERROR "Call to shaderc_parse() must have a type flag: ${TYPES}" )
		return()
	elseif( "${TYPE}" STREQUAL "FRAGMENT" )
		list( APPEND CLI "--type" "fragment" )
	elseif( "${TYPE}" STREQUAL "VERTEX" )
		list( APPEND CLI "--type" "vertex" )
	endif()

	# --varyingdef
	if( ARG_VARYINGDEF )
		list( APPEND CLI "--varyingdef" "${ARG_VARYINGDEF}" )
	endif()

	# --verbose
	if( ARG_VERBOSE )
		list( APPEND CLI "--verbose" )
	endif()

	# --debug
	if( ARG_DEBUG )
		list( APPEND CLI "--debug" )
	endif()

	# --disasm
	if( ARG_DISASM )
		list( APPEND CLI "--disasm" )
	endif()

	# --profile
	if( ARG_PROFILE )
		list( APPEND CLI "--profile" "${ARG_PROFILE}" )
	endif()

	# -O
	if( ARG_O )
		list( APPEND CLI "-O" "${ARG_O}" )
	endif()

	# --Werror
	if( ARG_WERROR )
		list( APPEND CLI "--Werror" )
	endif()

	set( ${ARG_OUT} ${CLI} PARENT_SCOPE )
endfunction()
