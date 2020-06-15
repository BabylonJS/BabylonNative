# Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    "include/jsi/instrumentation.h"
    "include/jsi/jsi.h"
    "include/jsi/jsi-inl.h"
    "source/jsi.cpp")

add_library(jsi ${SOURCES})

target_include_directories(jsi
    PRIVATE "source"
    PUBLIC "include")

set(jsi_compile_flags "")
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR
    "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  list(APPEND jsi_compile_flags "-Wno-non-virtual-dtor")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
  # Turn on Error Handling in MSVC, otherwise objects are not destructed
  # when they go out of scope due to exceptions.
  list(APPEND jsi_compile_flags "/EHsc")
endif()
target_compile_options(jsi PUBLIC ${jsi_compile_flags})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
