cmake_minimum_required(VERSION 3.21)
project(WebPParserOption LANGUAGES CXX)

if(NOT DEFINED EXPECTED_WEBP OR NOT DEFINED EXPECTED_WEBP_DEFINITION)
    message(FATAL_ERROR "Set EXPECTED_WEBP and EXPECTED_WEBP_DEFINITION (0, 1, or inherit)")
endif()

# Provide the bimg targets without fetching unrelated Babylon Native dependencies.
# Stop including Dependencies/CMakeLists.txt after its bimg configuration.
macro(FetchContent_MakeAvailable_With_Message dependency)
    if("${dependency}" STREQUAL "bgfx.cmake")
        foreach(target IN ITEMS bx bgfx bimg bimg_encode)
            add_library(${target} STATIC decoder.cpp)
        endforeach()
        add_library(bimg_decode STATIC decoder.cpp)
        target_compile_definitions(bimg_decode PRIVATE BIMG_CONFIG_PARSE_ENABLE=$<BOOL:${BIMG_CONFIG_PARSE_ENABLE}>)
    elseif("${dependency}" STREQUAL "glslang")
        return()
    endif()
endmacro()

if(NOT DEFINED BN_DEPENDENCIES_FILE)
    set(BN_DEPENDENCIES_FILE "${CMAKE_CURRENT_LIST_DIR}/../../CMakeLists.txt")
endif()
include("${BN_DEPENDENCIES_FILE}")

if(LATE_GENERIC_DISABLE)
    target_compile_definitions(bimg_decode PRIVATE BIMG_CONFIG_PARSE_ENABLE=0)
endif()

get_target_property(definitions bimg_decode COMPILE_DEFINITIONS)
set(webp_definitions)
foreach(definition IN LISTS definitions)
    if(definition MATCHES "^BIMG_CONFIG_PARSE_WEBP=")
        list(APPEND webp_definitions "${definition}")
    endif()
endforeach()

if(EXPECTED_WEBP_DEFINITION STREQUAL "inherit")
    if(webp_definitions)
        message(FATAL_ERROR "Expected WebP to inherit the generic setting, got ${webp_definitions}")
    endif()
elseif(NOT webp_definitions STREQUAL "BIMG_CONFIG_PARSE_WEBP=${EXPECTED_WEBP_DEFINITION}")
    message(FATAL_ERROR "Expected one BIMG_CONFIG_PARSE_WEBP=${EXPECTED_WEBP_DEFINITION}, got ${webp_definitions}")
endif()

target_compile_definitions(bimg_decode PRIVATE EXPECTED_WEBP=${EXPECTED_WEBP})
