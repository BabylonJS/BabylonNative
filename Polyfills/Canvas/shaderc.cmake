# Copy of bgfx _bgfx_shaderc_parse function adapted for Babylon Native
# Function is not guarded behind BGFX_TOOLS_SHADERC and shaderx path is set by SHADERC_PATH variable.
# Usage:
# _bn_shaderc_parse(
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
function(_bn_shaderc_parse ARG_OUT)
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
        message(SEND_ERROR "Call to _bn_shaderc_parse() must have an input file path specified.")
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
        message(SEND_ERROR "Call to _bn_shaderc_parse() must have an output file path specified.")
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
                message(SEND_ERROR "Call to _bn_shaderc_parse() cannot have both flags ${PLATFORM} and ${P}.")
                return()
            endif()
            set(PLATFORM "${P}")
        endif()
    endforeach()
    if(PLATFORM STREQUAL "")
        message(SEND_ERROR "Call to _bn_shaderc_parse() must have a platform flag: ${PLATFORMS}")
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
                message(SEND_ERROR "Call to _bn_shaderc_parse() cannot have both flags ${TYPE} and ${T}.")
                return()
            endif()
            set(TYPE "${T}")
        endif()
    endforeach()
    if("${TYPE}" STREQUAL "")
        message(SEND_ERROR "Call to _bn_shaderc_parse() must have a type flag: ${TYPES}")
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
        message(SEND_ERROR "Call to _bn_shaderc_parse() must have a shader profile.")
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

function(add_bgfx_shader FILE FOLDER)
    get_filename_component(FILENAME "${FILE}" NAME_WE)
    string(SUBSTRING "${FILENAME}" 0 2 TYPE)
    if("${TYPE}" STREQUAL "fs")
        set(TYPE "FRAGMENT")
    elseif("${TYPE}" STREQUAL "vs")
        set(TYPE "VERTEX")
    elseif("${TYPE}" STREQUAL "cs")
        set(TYPE "COMPUTE")
    else()
        set(TYPE "")
    endif()

    if(NOT "${TYPE}" STREQUAL "")
        set(COMMON FILE ${FILE} ${TYPE} INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders ${BGFX_DIR}/src)
        set(OUTPUTS "")
        set(OUTPUTS_PRETTY "")

        # dx11
        if(WIN32)
            set(DX11_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders/dx11/${FILENAME}.h)
            if(NOT "${TYPE}" STREQUAL "COMPUTE")
                _bn_shaderc_parse(
                    DX11 ${COMMON} WINDOWS
                    PROFILE s_5_0
                    O 3
                    OUTPUT ${DX11_OUTPUT}
                    BIN2C "${FILENAME}_dx11"
                )
            else()
                _bn_shaderc_parse(
                    DX11 ${COMMON} WINDOWS
                    PROFILE s_5_0
                    O 1
                    OUTPUT ${DX11_OUTPUT}
                    BIN2C "${FILENAME}_dx11"
                )
            endif()
            list(APPEND OUTPUTS "DX11")
            set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}DX11, ")
        endif()
        
        # metal
        set(METAL_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders/metal/${FILENAME}.h)
        _bn_shaderc_parse(METAL ${COMMON} OSX PROFILE metal OUTPUT ${METAL_OUTPUT} BIN2C "${FILENAME}_mtl")
        list(APPEND OUTPUTS "METAL")
        set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}Metal, ")

        # essl
        if(NOT "${TYPE}" STREQUAL "COMPUTE")
            set(ESSL_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders/essl/${FILENAME}.h)
            _bn_shaderc_parse(ESSL ${COMMON} ANDROID PROFILE 100_es OUTPUT ${ESSL_OUTPUT} BIN2C "${FILENAME}_essl")
            list(APPEND OUTPUTS "ESSL")
            set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}ESSL, ")
        endif()

        # glsl
        set(GLSL_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders/glsl/${FILENAME}.h)
        if(NOT "${TYPE}" STREQUAL "COMPUTE")
            _bn_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 140 OUTPUT ${GLSL_OUTPUT} BIN2C "${FILENAME}_glsl")
        else()
            _bn_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 430 OUTPUT ${GLSL_OUTPUT} BIN2C "${FILENAME}_glsl")
        endif()
        list(APPEND OUTPUTS "GLSL")
        set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}GLSL, ")

        # spirv
        if(NOT "${TYPE}" STREQUAL "COMPUTE")
            set(SPIRV_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders/spirv/${FILENAME}.h)
            _bn_shaderc_parse(SPIRV ${COMMON} LINUX PROFILE spirv OUTPUT ${SPIRV_OUTPUT} BIN2C "${FILENAME}_spv")
            list(APPEND OUTPUTS "SPIRV")
            set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}SPIRV")
            set(OUTPUT_FILES "")
            set(COMMANDS "")
        endif()

        foreach(OUT ${OUTPUTS})
            list(APPEND OUTPUT_FILES ${${OUT}_OUTPUT})
            list(APPEND COMMANDS COMMAND ${SHADERC_PATH} ${${OUT}})
            get_filename_component(OUT_DIR ${${OUT}_OUTPUT} DIRECTORY)
            file(MAKE_DIRECTORY ${OUT_DIR})
        endforeach()

        file(RELATIVE_PATH PRINT_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders ${FILE})
        add_custom_command(
            MAIN_DEPENDENCY ${FILE} OUTPUT ${OUTPUT_FILES} ${COMMANDS}
            COMMENT "Compiling shader ${PRINT_NAME} for ${OUTPUTS_PRETTY}"
        )
    endif()
endfunction()
