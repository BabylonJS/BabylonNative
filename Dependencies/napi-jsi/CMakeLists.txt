cmake_minimum_required(VERSION 3.12)

project(napi)

set(NAPI_JAVASCRIPT_ENGINE "Chakra" CACHE STRING "JavaScript engine for N-API.")

add_subdirectory(ReactCommon/jsi)

# --- chakra-jsi --- START
add_library(chakra-jsi
    "chakra-jsi/Common/unicode.cpp"
    "chakra-jsi/Common/unicode.h"
    "chakra-jsi/Common/utilities.h"
    "chakra-jsi/JSI/Shared/ByteArrayBuffer.h"
    "chakra-jsi/JSI/Shared/ChakraObjectRef.cpp"
    "chakra-jsi/JSI/Shared/ChakraObjectRef.h"
    "chakra-jsi/JSI/Shared/ChakraRuntime.cpp"
    "chakra-jsi/JSI/Shared/ChakraRuntime.h"
    "chakra-jsi/JSI/Shared/ChakraRuntimeArgs.h"
    "chakra-jsi/JSI/Shared/ChakraRuntimeFactory.h"
    "chakra-jsi/JSI/Shared/RuntimeHolder.h"
    "chakra-jsi/JSI/Shared/ScriptStore.h")

target_include_directories(chakra-jsi
    PRIVATE "ReactCommon"
    PUBLIC "chakra-jsi/Common"
    PUBLIC "chakra-jsi/JSI/Shared")

target_link_libraries(chakra-jsi
    PRIVATE jsi
    "chakrart.lib")
# --- chakra-jsi --- END

set(SOURCES
    "include/napi/env.h"
    "include/napi/napi.h"
    "include/napi/napi-inl.h"
    "source/env.cc")

add_library(napi ${SOURCES})

# --- chakra-jsi --- START
target_compile_definitions(napi PUBLIC USE_EDGEMODE_JSRT)
# --- chakra-jsi --- END

target_include_directories(napi
    PUBLIC "ReactCommon"
    PUBLIC "include")

target_link_libraries(napi
# --- chakra-jsi --- START
    PUBLIC chakra-jsi
# --- chakra-jsi --- END
    PRIVATE jsi)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
