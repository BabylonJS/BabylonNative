if(NOT TARGET jsi)
    message(FATAL_ERROR "jsi target is required")
endif()

set(SOURCES
    "include/napi/env.h"
    "include/napi/napi.h"
    "include/napi/napi-inl.h"
    "source/env.cc")

add_library(napi ${SOURCES})

target_include_directories(napi
    PUBLIC "include")

target_link_to_dependencies(napi
    PUBLIC jsi)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
