
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
                _bgfx_shaderc_parse(
                    DX11 ${COMMON} WINDOWS
                    PROFILE s_5_0
                    O 3
                    OUTPUT ${DX11_OUTPUT}
                    BIN2C "${FILENAME}_dx11"
                )
            else()
                _bgfx_shaderc_parse(
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
        _bgfx_shaderc_parse(METAL ${COMMON} OSX PROFILE metal OUTPUT ${METAL_OUTPUT} BIN2C "${FILENAME}_mtl")
        list(APPEND OUTPUTS "METAL")
        set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}Metal, ")

        # essl
        if(NOT "${TYPE}" STREQUAL "COMPUTE")
            set(ESSL_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders/essl/${FILENAME}.h)
            _bgfx_shaderc_parse(ESSL ${COMMON} ANDROID PROFILE 100_es OUTPUT ${ESSL_OUTPUT} BIN2C "${FILENAME}_essl")
            list(APPEND OUTPUTS "ESSL")
            set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}ESSL, ")
        endif()

        # glsl
        set(GLSL_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders/glsl/${FILENAME}.h)
        if(NOT "${TYPE}" STREQUAL "COMPUTE")
            _bgfx_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 140 OUTPUT ${GLSL_OUTPUT} BIN2C "${FILENAME}_glsl")
        else()
            _bgfx_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 430 OUTPUT ${GLSL_OUTPUT} BIN2C "${FILENAME}_glsl")
        endif()
        list(APPEND OUTPUTS "GLSL")
        set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}GLSL, ")

        # spirv
        if(NOT "${TYPE}" STREQUAL "COMPUTE")
            set(SPIRV_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders/spirv/${FILENAME}.h)
            _bgfx_shaderc_parse(SPIRV ${COMMON} LINUX PROFILE spirv OUTPUT ${SPIRV_OUTPUT} BIN2C "${FILENAME}_spv")
            list(APPEND OUTPUTS "SPIRV")
            set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}SPIRV")
            set(OUTPUT_FILES "")
            set(COMMANDS "")
        endif()

        foreach(OUT ${OUTPUTS})
            list(APPEND OUTPUT_FILES ${${OUT}_OUTPUT})
            list(APPEND COMMANDS COMMAND "bgfx::shaderc" ${${OUT}})
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

set(SOURCES
    "Include/Babylon/Polyfills/Canvas.h"
    "Source/Canvas.cpp"
    "Source/Canvas.h"
    "Source/Colors.h"
    "Source/Image.cpp"
    "Source/Image.h"
    "Source/ImageData.cpp"
    "Source/ImageData.h"
    "Source/Path2D.cpp"
    "Source/Path2D.h"
    "Source/LineCaps.h"
    "Source/Context.cpp"
    "Source/Context.h"
    "Source/MeasureText.cpp"
    "Source/MeasureText.h"
    "Source/Gradient.cpp"
    "Source/Gradient.h"
    "Source/nanosvg.h"
    "Source/nanovg/nanovg.cpp"
    "Source/nanovg/nanovg.h"
    "Source/nanovg/nanovg_babylon.cpp"
    "Source/nanovg/nanovg_babylon.h"
    "Source/nanovg/nanovg_filterstack.cpp"
    "Source/nanovg/nanovg_filterstack.h"
    )

file(GLOB SHADERS "Source/Shaders/*.sc" "Source/Shaders/*.sh")

add_library(Canvas ${SOURCES} ${SHADERS})

target_include_directories(Canvas
    PUBLIC "Include"
    PRIVATE "Source"
    PRIVATE "${BGFX_DIR}/3rdparty")

target_link_libraries(Canvas
    PUBLIC napi
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDeviceContext
    PRIVATE UrlLib
    PRIVATE base-n)

if(BGFX_BUILD_TOOLS AND BGFX_BUILD_TOOLS_SHADER)
    if(NOT IOS AND NOT VISIONOS AND NOT ANDROID)
        foreach(SHADER ${SHADERS})
            add_bgfx_shader(${SHADER} "")
        endforeach()
    endif()
    add_dependencies(Canvas shaderc)
endif()

set_property(TARGET Canvas PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group("3rd party Sources" ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FONT_SOURCES} ${ATLAS_SOURCES})
source_group("Shaders" ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHADERS})
target_compile_definitions(Canvas PRIVATE _CRT_SECURE_NO_WARNINGS)
