# If bgfx.cmake was compiled without tools or cross compiled without host having tools,
# then don't provide helper functions
if(TARGET bgfx::bin2c)
	# _bgfx_bin2c_parse(
	#	INPUT_FILE filename
	#	OUTPUT_FILE filename
	#	ARRAY_NAME name
	# )
	# Usage: bin2c -f <in> -o <out> -n <name>
	function(_bgfx_bin2c_parse ARG_OUT)
		set(options "")
		set(oneValueArgs INPUT_FILE;OUTPUT_FILE;ARRAY_NAME)
		set(multiValueArgs "")
		cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")
		set(CLI "")

		# -f
		if(ARG_INPUT_FILE)
			list(APPEND CLI "-f" "${ARG_INPUT_FILE}")
		else()
			message(SEND_ERROR "Call to _bgfx_bin2c_parse() must have an INPUT_FILE")
		endif()

		# -o
		if(ARG_OUTPUT_FILE)
			list(APPEND CLI "-o" "${ARG_OUTPUT_FILE}")
		else()
			message(SEND_ERROR "Call to _bgfx_bin2c_parse() must have an OUTPUT_FILE")
		endif()

		# -n
		if(ARG_ARRAY_NAME)
			list(APPEND CLI "-n" "${ARG_ARRAY_NAME}")
		else()
			message(SEND_ERROR "Call to _bgfx_bin2c_parse() must have an ARRAY_NAME")
		endif()

		set(${ARG_OUT} ${CLI} PARENT_SCOPE)
	endfunction()

	# bgfx_compile_binary_to_header(
	#	INPUT_FILE filename
	#	OUTPUT_FILE filename
	#	ARRAY_NAME name
	# )
	#
	function(bgfx_compile_binary_to_header)
		set(options "")
		set(oneValueArgs INPUT_FILE;OUTPUT_FILE;ARRAY_NAME)
		set(multiValueArgs "")
		cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")
		_bgfx_bin2c_parse(
			CLI
			INPUT_FILE ${ARG_INPUT_FILE}
			OUTPUT_FILE ${ARG_OUTPUT_FILE}
			ARRAY_NAME ${ARG_ARRAY_NAME}
		)
		add_custom_command(
			OUTPUT ${ARG_OUTPUT_FILE} #
			COMMAND bgfx::bin2c ${CLI} #
			MAIN_DEPENDENCY ${ARG_INPUT_FILE} #
		)
	endfunction()
endif()

# If bgfx.cmake was compiled without tools or cross compiled without host having tools,
# then don't provide helper functions
if(TARGET bgfx::texturec)
	# _bgfx_texturec_parse(
	#	FILE filename
	#	OUTPUT filename
	#	[FORMAT format]
	#	[QUALITY default|fastest|highest]
	#	[MIPS]
	#	[MIPSKIP N]
	#	[NORMALMAP]
	#	[EQUIRECT]
	#	[STRIP]
	#	[SDF]
	#	[REF alpha]
	#	[IQA]
	#	[PMA]
	#	[LINEAR]
	#	[MAX max size]
	#	[RADIANCE model]
	#	[AS extension]
	# )
	function(_bgfx_texturec_parse ARG_OUT)
		cmake_parse_arguments(
			ARG #
			"MIPS;NORMALMAP;EQUIRECT;STRIP;SDF;IQA;PMA;LINEAR" #
			"FILE;OUTPUT;FORMAT;QUALITY;MIPSKIP;REF;MAX;RADIANCE;AS" #
			"" #
			${ARGN} #
		)
		set(CLI "")

		# -f
		if(ARG_FILE)
			list(APPEND CLI "-f" "${ARG_FILE}")
		endif()

		# -o
		if(ARG_OUTPUT)
			list(APPEND CLI "-o" "${ARG_OUTPUT}")
		endif()

		# -t
		if(ARG_FORMAT)
			list(APPEND CLI "-t" "${ARG_FORMAT}")
		endif()

		# -q
		if(ARG_QUALITY)
			list(APPEND CLI "-q" "${ARG_QUALITY}")
		endif()

		# --mips
		if(ARG_MIPS)
			list(APPEND CLI "--mips")
		endif()

		# --mipskip
		if(ARG_MIPSKIP)
			list(APPEND CLI "--mipskip" "${ARG_MIPSKIP}")
		endif()

		# --normalmap
		if(ARG_NORMALMAP)
			list(APPEND CLI "--normalmap")
		endif()

		# --equirect
		if(ARG_EQUIRECT)
			list(APPEND CLI "--equirect")
		endif()

		# --strip
		if(ARG_STRIP)
			list(APPEND CLI "--strip")
		endif()

		# --sdf
		if(ARG_SDF)
			list(APPEND CLI "--sdf")
		endif()

		# --ref
		if(ARG_REF)
			list(APPEND CLI "--ref" "${ARG_REF}")
		endif()

		# --iqa
		if(ARG_IQA)
			list(APPEND CLI "--iqa")
		endif()

		# --pma
		if(ARG_PMA)
			list(APPEND CLI "--pma")
		endif()

		# --linear
		if(ARG_LINEAR)
			list(APPEND CLI "--linear")
		endif()

		# --max
		if(ARG_MAX)
			list(APPEND CLI "--max" "${ARG_MAX}")
		endif()

		# --radiance
		if(ARG_RADIANCE)
			list(APPEND CLI "--radiance" "${ARG_RADIANCE}")
		endif()

		# --as
		if(ARG_AS)
			list(APPEND CLI "--as" "${ARG_AS}")
		endif()

		set(${ARG_OUT} ${CLI} PARENT_SCOPE)
	endfunction()

	# bgfx_compile_texture(
	#	FILE filename
	#	OUTPUT filename
	#	[FORMAT format]
	#	[QUALITY default|fastest|highest]
	#	[MIPS]
	#	[MIPSKIP N]
	#	[NORMALMAP]
	#	[EQUIRECT]
	#	[STRIP]
	#	[SDF]
	#	[REF alpha]
	#	[IQA]
	#	[PMA]
	#	[LINEAR]
	#	[MAX max size]
	#	[RADIANCE model]
	#	[AS extension]
	# )
	#
	function(bgfx_compile_texture)
		cmake_parse_arguments(
			ARG #
			"MIPS;NORMALMAP;EQUIRECT;STRIP;SDF;IQA;PMA;LINEAR" #
			"FILE;OUTPUT;FORMAT;QUALITY;MIPSKIP;REF;MAX;RADIANCE;AS" #
			"" #
			${ARGN} #
		)
		_bgfx_texturec_parse(CLI ${ARGV})
		add_custom_command(
			OUTPUT ${ARG_OUTPUT} #
			COMMAND bgfx::texturec ${CLI} #
			MAIN_DEPENDENCY ${ARG_FILE} #
		)
	endfunction()
endif()

# If bgfx.cmake was compiled without tools or cross compiled without host having tools,
# then don't provide helper functions
if(TARGET bgfx::geometryc)
	# _bgfx_geometryc_parse(
	#	FILE filename
	#	OUTPUT filename
	#	[SCALE scale]
	#	[CCW]
	#	[FLIPV]
	#	[OBB num steps]
	#	[PACKNORMAL 0|1]
	#	[PACKUV 0|1]
	#	[TANGENT]
	#	[BARYCENTRIC]
	#	[COMPRESS]
	#	[LH_UP_Y|LH_UP_Z|RH_UP_Y|RH_UP_Z]
	# )
	function(_bgfx_geometryc_parse ARG_OUT)
		cmake_parse_arguments(
			ARG #
			"CCW;FLIPV;TANGENT;BARYCENTRIC;COMPRESS;LH_UP_Y;LH_UP_Z;RH_UP_Y;RH_UP_Z" #
			"FILE;OUTPUT;SCALE;OBB;PACKNORMAL;PACKUV" #
			"" #
			${ARGN} #
		)
		set(CLI "")

		# -f
		if(ARG_FILE)
			list(APPEND CLI "-f" "${ARG_FILE}")
		endif()

		# -o
		if(ARG_OUTPUT)
			list(APPEND CLI "-o" "${ARG_OUTPUT}")
		endif()

		# -s
		if(ARG_SCALE)
			list(APPEND CLI "-s" "${ARG_SCALE}")
		endif()

		# --cw
		if(ARG_QUALITY)
			list(APPEND CLI "--cw")
		endif()

		# --flipv
		if(ARG_FLIPV)
			list(APPEND CLI "--flipv")
		endif()

		# --obb
		if(ARG_OBB)
			list(APPEND CLI "--mipskip" "${ARG_OBB}")
		endif()

		# --packnormal
		if(ARG_PACKNORMAL)
			list(APPEND CLI "--packnormal" "${ARG_PACKNORMAL}")
		endif()

		# --packuv
		if(ARG_PACKUV)
			list(APPEND CLI "--packuv" "${ARG_PACKUV}")
		endif()

		# --tangent
		if(ARG_TANGENT)
			list(APPEND CLI "--tangent")
		endif()

		# --barycentric
		if(ARG_BARYCENTRIC)
			list(APPEND CLI "--barycentric")
		endif()

		# --compress
		if(ARG_REF)
			list(APPEND CLI "--compress" "${ARG_COMPRESS}")
		endif()

		# --lh-up+y
		if(ARG_LH_UP_Y)
			list(APPEND CLI "--lh-up+y")
		endif()

		# --lh-up+z
		if(ARG_LH_UP_Z)
			list(APPEND CLI "--lh-up+z")
		endif()

		# --rh-up+y
		if(ARG_RH_UP_Y)
			list(APPEND CLI "--rh-up+y")
		endif()

		# --rh-up+z
		if(ARG_RH_UP_Z)
			list(APPEND CLI "--rh-up+z")
		endif()

		set(${ARG_OUT} ${CLI} PARENT_SCOPE)
	endfunction()

	# bgfx_compile_geometry(
	#	FILE filename
	#	OUTPUT filename
	#	[SCALE scale]
	#	[CCW]
	#	[FLIPV]
	#	[OBB num steps]
	#	[PACKNORMAL 0|1]
	#	[PACKUV 0|1]
	#	[TANGENT]
	#	[BARYCENTRIC]
	#	[COMPRESS]
	#	[LH_UP_Y|LH_UP_Z|RH_UP_Y|RH_UP_Z]
	# )
	#
	function(bgfx_compile_geometry)
		cmake_parse_arguments(
			ARG #
			"CCW;FLIPV;TANGENT;BARYCENTRIC;COMPRESS;LH_UP_Y;LH_UP_Z;RH_UP_Y;RH_UP_Z" #
			"FILE;OUTPUT;SCALE;OBB;PACKNORMAL;PACKUV" #
			"" #
			${ARGN} #
		)
		_bgfx_geometryc_parse(CLI ${ARGV})
		add_custom_command(
			OUTPUT ${ARG_OUTPUT} #
			COMMAND bgfx::geometryc ${CLI} #
			MAIN_DEPENDENCY ${ARG_FILE} #
		)
	endfunction()
endif()

# If bgfx.cmake was compiled without tools or cross compiled without host having tools,
# then don't provide helper functions
if(TARGET bgfx::shaderc)
	# _bgfx_shaderc_parse(
	#	FILE filename
	#	OUTPUT filename
	#	FRAGMENT|VERTEX|COMPUTE
	#	ANDROID|ASM_JS|IOS|LINUX|OSX|WINDOWS|ORBIS
	#	PROFILE profile
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
	function(_bgfx_shaderc_parse ARG_OUT)
		cmake_parse_arguments(
			ARG
			"DEPENDS;ANDROID;ASM_JS;IOS;LINUX;OSX;WINDOWS;ORBIS;PREPROCESS;RAW;FRAGMENT;VERTEX;COMPUTE;VERBOSE;DEBUG;DISASM;WERROR"
			"FILE;OUTPUT;VARYINGDEF;BIN2C;PROFILE;O"
			"INCLUDES;DEFINES"
			${ARGN}
		)
		set(CLI "")

		# -f
		if(ARG_FILE)
			list(APPEND CLI "-f" "${ARG_FILE}")
		else()
			message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have an input file path specified.")
		endif()

		# -i
		if(ARG_INCLUDES)
			foreach(INCLUDE ${ARG_INCLUDES})
				list(APPEND CLI "-i")
				list(APPEND CLI "${INCLUDE}")
			endforeach()
		endif()

		# -o
		if(ARG_OUTPUT)
			list(APPEND CLI "-o" "${ARG_OUTPUT}")
		else()
			message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have an output file path specified.")
		endif()

		# --bin2c
		if(ARG_BIN2C)
			list(APPEND CLI "--bin2c" "${ARG_BIN2C}")
		endif()

		# --depends
		if(ARG_DEPENDS)
			list(APPEND CLI "--depends")
		endif()

		# --platform
		set(PLATFORM "")
		set(PLATFORMS "ANDROID;ASM_JS;IOS;LINUX;OSX;WINDOWS;ORBIS")
		foreach(P ${PLATFORMS})
			if(ARG_${P})
				if(PLATFORM)
					message(SEND_ERROR "Call to _bgfx_shaderc_parse() cannot have both flags ${PLATFORM} and ${P}.")
					return()
				endif()
				set(PLATFORM "${P}")
			endif()
		endforeach()
		if(PLATFORM STREQUAL "")
			message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have a platform flag: ${PLATFORMS}")
			return()
		elseif(PLATFORM STREQUAL "ANDROID")
			list(APPEND CLI "--platform" "android")
		elseif(PLATFORM STREQUAL "ASM_JS")
			list(APPEND CLI "--platform" "asm.js")
		elseif(PLATFORM STREQUAL "IOS")
			list(APPEND CLI "--platform" "ios")
		elseif(PLATFORM STREQUAL "OSX")
			list(APPEND CLI "--platform" "osx")
		elseif(PLATFORM STREQUAL "LINUX")
			list(APPEND CLI "--platform" "linux")
		elseif(PLATFORM STREQUAL "WINDOWS")
			list(APPEND CLI "--platform" "windows")
		elseif(PLATFORM STREQUAL "ORBIS")
			list(APPEND CLI "--platform" "orbis")
		endif()

		# --preprocess
		if(ARG_PREPROCESS)
			list(APPEND CLI "--preprocess")
		endif()

		# --define
		if(ARG_DEFINES)
			list(APPEND CLI "--defines")
			set(DEFINES "")
			foreach(DEFINE ${ARG_DEFINES})
				if(NOT "${DEFINES}" STREQUAL "")
					set(DEFINES "${DEFINES}\\\\;${DEFINE}")
				else()
					set(DEFINES "${DEFINE}")
				endif()
			endforeach()
			list(APPEND CLI "${DEFINES}")
		endif()

		# --raw
		if(ARG_RAW)
			list(APPEND CLI "--raw")
		endif()

		# --type
		set(TYPE "")
		set(TYPES "FRAGMENT;VERTEX;COMPUTE")
		foreach(T ${TYPES})
			if(ARG_${T})
				if(TYPE)
					message(SEND_ERROR "Call to _bgfx_shaderc_parse() cannot have both flags ${TYPE} and ${T}.")
					return()
				endif()
				set(TYPE "${T}")
			endif()
		endforeach()
		if("${TYPE}" STREQUAL "")
			message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have a type flag: ${TYPES}")
			return()
		elseif("${TYPE}" STREQUAL "FRAGMENT")
			list(APPEND CLI "--type" "fragment")
		elseif("${TYPE}" STREQUAL "VERTEX")
			list(APPEND CLI "--type" "vertex")
		elseif("${TYPE}" STREQUAL "COMPUTE")
			list(APPEND CLI "--type" "compute")
		endif()

		# --varyingdef
		if(ARG_VARYINGDEF)
			list(APPEND CLI "--varyingdef" "${ARG_VARYINGDEF}")
		endif()

		# --verbose
		if(ARG_VERBOSE)
			list(APPEND CLI "--verbose")
		endif()

		# --debug
		if(ARG_DEBUG)
			list(APPEND CLI "--debug")
		endif()

		# --disasm
		if(ARG_DISASM)
			list(APPEND CLI "--disasm")
		endif()

		# --profile
		if(ARG_PROFILE)
			list(APPEND CLI "--profile" "${ARG_PROFILE}")
		else()
			message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have a shader profile.")
		endif()

		# -O
		if(ARG_O)
			list(APPEND CLI "-O" "${ARG_O}")
		endif()

		# --Werror
		if(ARG_WERROR)
			list(APPEND CLI "--Werror")
		endif()

		set(${ARG_OUT} ${CLI} PARENT_SCOPE)
	endfunction()

	# extensions consistent with those listed under bgfx/runtime/shaders
	function(_bgfx_get_profile_path_ext PROFILE PROFILE_PATH_EXT)
		string(REPLACE 300_es essl PROFILE ${PROFILE})
		string(REPLACE 120 glsl PROFILE ${PROFILE})
		string(REPLACE s_4_0 dx10 PROFILE ${PROFILE})
		string(REPLACE s_5_0 dx11 PROFILE ${PROFILE})
		set(${PROFILE_PATH_EXT} ${PROFILE} PARENT_SCOPE)
	endfunction()

	# extensions consistent with embedded_shader.h
	function(_bgfx_get_profile_ext PROFILE PROFILE_EXT)
		string(REPLACE 300_es essl PROFILE ${PROFILE})
		string(REPLACE 120 glsl PROFILE ${PROFILE})
		string(REPLACE spirv spv PROFILE ${PROFILE})
		string(REPLACE metal mtl PROFILE ${PROFILE})
		string(REPLACE s_4_0 dx10 PROFILE ${PROFILE})
		string(REPLACE s_5_0 dx11 PROFILE ${PROFILE})
		set(${PROFILE_EXT} ${PROFILE} PARENT_SCOPE)
	endfunction()

	# bgfx_compile_shaders(
	# 	TYPE VERTEX|FRAGMENT|COMPUTE
	# 	SHADERS filenames
	# 	VARYING_DEF filename
	# 	OUTPUT_DIR directory
	# 	OUT_FILES_VAR variable name
	# 	INCLUDE_DIRS directories
	# 	[AS_HEADERS]
	# )
	#
	function(bgfx_compile_shaders)
		set(options AS_HEADERS)
		set(oneValueArgs TYPE VARYING_DEF OUTPUT_DIR OUT_FILES_VAR)
		set(multiValueArgs SHADERS INCLUDE_DIRS)
		cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

		set(PROFILES 120 300_es spirv)
		if(IOS)
			set(PLATFORM IOS)
			list(APPEND PROFILES metal)
		elseif(ANDROID)
			set(PLATFORM ANDROID)
		elseif(UNIX AND NOT APPLE)
			set(PLATFORM LINUX)
		elseif(EMSCRIPTEN)
			set(PLATFORM ASM_JS)
		elseif(APPLE)
			set(PLATFORM OSX)
			list(APPEND PROFILES metal)
		elseif(
			WIN32
			OR MINGW
			OR MSYS
			OR CYGWIN
		)
			set(PLATFORM WINDOWS)
			list(APPEND PROFILES s_4_0)
			list(APPEND PROFILES s_5_0)
		elseif(ORBIS) # ORBIS should be defined by a PS4 CMake toolchain
			set(PLATFORM ORBIS)
			list(APPEND PROFILES pssl)
		else()
			# pssl for Agc and Gnm renderers
			# nvn for Nvn renderer
			message(error "shaderc: Unsupported platform")
		endif()

		set(ALL_OUTPUTS "")
		foreach(SHADER_FILE ${ARGS_SHADERS})
			source_group("Shaders" FILES "${SHADER}")
			get_filename_component(SHADER_FILE_BASENAME ${SHADER_FILE} NAME)
			get_filename_component(SHADER_FILE_NAME_WE ${SHADER_FILE} NAME_WE)
			get_filename_component(SHADER_FILE_ABSOLUTE ${SHADER_FILE} ABSOLUTE)

			# Build output targets and their commands
			set(OUTPUTS "")
			set(COMMANDS "")
			set(MKDIR_COMMANDS "")
			foreach(PROFILE ${PROFILES})
				_bgfx_get_profile_path_ext(${PROFILE} PROFILE_PATH_EXT)
				_bgfx_get_profile_ext(${PROFILE} PROFILE_EXT)
				if(ARGS_AS_HEADERS)
					set(HEADER_PREFIX .h)
				endif()
				set(OUTPUT ${ARGS_OUTPUT_DIR}/${PROFILE_PATH_EXT}/${SHADER_FILE_BASENAME}.bin${HEADER_PREFIX})
				set(PLATFORM_I ${PLATFORM})
				if(PROFILE STREQUAL "spirv")
					set(PLATFORM_I LINUX)
				endif()
				set(BIN2C_PART "")
				if(ARGS_AS_HEADERS)
					set(BIN2C_PART BIN2C ${SHADER_FILE_NAME_WE}_${PROFILE_EXT})
				endif()
				_bgfx_shaderc_parse(
					CLI #
					${BIN2C_PART} #
					${ARGS_TYPE} ${PLATFORM_I} WERROR "$<$<CONFIG:debug>:DEBUG>$<$<CONFIG:relwithdebinfo>:DEBUG>"
					FILE ${SHADER_FILE_ABSOLUTE}
					OUTPUT ${OUTPUT}
					PROFILE ${PROFILE}
					O "$<$<CONFIG:debug>:0>$<$<CONFIG:release>:3>$<$<CONFIG:relwithdebinfo>:3>$<$<CONFIG:minsizerel>:3>"
					VARYINGDEF ${ARGS_VARYING_DEF}
					INCLUDES ${BGFX_SHADER_INCLUDE_PATH} ${ARGS_INCLUDE_DIRS}
				)
				list(APPEND OUTPUTS ${OUTPUT})
				list(APPEND ALL_OUTPUTS ${OUTPUT})
				list(
					APPEND
					MKDIR_COMMANDS
					COMMAND
					${CMAKE_COMMAND}
					-E
					make_directory
					${ARGS_OUTPUT_DIR}/${PROFILE_PATH_EXT}
				)
				list(APPEND COMMANDS COMMAND bgfx::shaderc ${CLI})
			endforeach()

			add_custom_command(
				OUTPUT ${OUTPUTS}
				COMMAND ${MKDIR_COMMANDS} ${COMMANDS}
				MAIN_DEPENDENCY ${SHADER_FILE_ABSOLUTE}
				DEPENDS ${ARGS_VARYING_DEF}
			)
		endforeach()

		if(DEFINED ARGS_OUT_FILES_VAR)
			set(${ARGS_OUT_FILES_VAR} ${ALL_OUTPUTS} PARENT_SCOPE)
		endif()
	endfunction()
endif()
