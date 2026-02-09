set(SOURCES
    "Source/Device.cpp"
    "Source/DeviceContext.cpp"
    "Source/DeviceImpl.cpp"
    "Source/DeviceImpl.h"
    "Source/SafeTimespanGuarantor.cpp"
    "Source/WgpuNative.cpp"
    "Source/WgpuNative.h"
    "InternalInclude/Babylon/Graphics/continuation_scheduler.h"
    "InternalInclude/Babylon/Graphics/DeviceContext.h"
    "InternalInclude/Babylon/Graphics/SafeTimespanGuarantor.h")

set(CARGO_COMMAND)
find_program(CARGO_EXECUTABLE cargo)
if(CARGO_EXECUTABLE)
    set(CARGO_COMMAND "${CARGO_EXECUTABLE}")
else()
    find_program(RUSTUP_EXECUTABLE rustup)
    if(NOT RUSTUP_EXECUTABLE)
        message(FATAL_ERROR "WGPU backend requires cargo in PATH or rustup in PATH.")
    endif()

    execute_process(
        COMMAND "${RUSTUP_EXECUTABLE}" show active-toolchain
        OUTPUT_VARIABLE RUSTUP_ACTIVE_TOOLCHAIN_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RUSTUP_ACTIVE_TOOLCHAIN_RESULT)

    if(NOT RUSTUP_ACTIVE_TOOLCHAIN_RESULT EQUAL 0 OR RUSTUP_ACTIVE_TOOLCHAIN_RAW STREQUAL "")
        message(FATAL_ERROR "Failed to resolve active rustup toolchain.")
    endif()

    string(REGEX MATCH "^[^ ]+" RUSTUP_ACTIVE_TOOLCHAIN "${RUSTUP_ACTIVE_TOOLCHAIN_RAW}")
    if(RUSTUP_ACTIVE_TOOLCHAIN STREQUAL "")
        message(FATAL_ERROR "Failed to parse active rustup toolchain from: ${RUSTUP_ACTIVE_TOOLCHAIN_RAW}")
    endif()

    execute_process(
        COMMAND "${RUSTUP_EXECUTABLE}" which --toolchain "${RUSTUP_ACTIVE_TOOLCHAIN}" cargo
        OUTPUT_VARIABLE RUSTUP_CARGO_EXECUTABLE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RUSTUP_CARGO_RESULT)
    execute_process(
        COMMAND "${RUSTUP_EXECUTABLE}" which --toolchain "${RUSTUP_ACTIVE_TOOLCHAIN}" rustc
        OUTPUT_VARIABLE RUSTUP_RUSTC_EXECUTABLE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RUSTUP_RUSTC_RESULT)

    if(NOT RUSTUP_CARGO_RESULT EQUAL 0 OR RUSTUP_CARGO_EXECUTABLE STREQUAL "")
        message(FATAL_ERROR "Failed to resolve cargo path for toolchain ${RUSTUP_ACTIVE_TOOLCHAIN}.")
    endif()
    if(NOT RUSTUP_RUSTC_RESULT EQUAL 0 OR RUSTUP_RUSTC_EXECUTABLE STREQUAL "")
        message(FATAL_ERROR "Failed to resolve rustc path for toolchain ${RUSTUP_ACTIVE_TOOLCHAIN}.")
    endif()

    get_filename_component(RUSTUP_TOOLCHAIN_BIN_DIR "${RUSTUP_RUSTC_EXECUTABLE}" DIRECTORY)
    if(WIN32)
        set(RUSTUP_ENV_PATH "${RUSTUP_TOOLCHAIN_BIN_DIR};$ENV{PATH}")
    else()
        set(RUSTUP_ENV_PATH "${RUSTUP_TOOLCHAIN_BIN_DIR}:$ENV{PATH}")
    endif()

    set(CARGO_COMMAND
        "${CMAKE_COMMAND}"
        "-E"
        "env"
        "PATH=${RUSTUP_ENV_PATH}"
        "RUSTC=${RUSTUP_RUSTC_EXECUTABLE}"
        "${RUSTUP_CARGO_EXECUTABLE}")
endif()

set(RUST_CRATE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Rust")
set(RUST_TARGET_DIR "${CMAKE_BINARY_DIR}/cargo")
set(RUST_PROFILE_DIR "debug")
set(RUST_BUILD_FLAG "")

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
    set(RUST_PROFILE_DIR "release")
    set(RUST_BUILD_FLAG "--release")
endif()

if(WIN32)
    set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_PROFILE_DIR}/babylon_graphics_backend.lib")
else()
    set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_PROFILE_DIR}/libbabylon_graphics_backend.a")
endif()

add_custom_command(
    OUTPUT "${RUST_OUTPUT_LIB}"
    COMMAND ${CARGO_COMMAND} build --manifest-path "${RUST_CRATE_DIR}/Cargo.toml" --target-dir "${RUST_TARGET_DIR}" ${RUST_BUILD_FLAG}
    WORKING_DIRECTORY "${RUST_CRATE_DIR}"
    DEPENDS
        "${RUST_CRATE_DIR}/Cargo.toml"
        "${RUST_CRATE_DIR}/src/lib.rs"
    COMMENT "Building Rust WebGPU backend"
    VERBATIM)

add_custom_target(babylon_graphics_backend_rust DEPENDS "${RUST_OUTPUT_LIB}")

add_library(babylon_graphics_backend STATIC IMPORTED GLOBAL)
set_target_properties(babylon_graphics_backend PROPERTIES IMPORTED_LOCATION "${RUST_OUTPUT_LIB}")
add_dependencies(babylon_graphics_backend babylon_graphics_backend_rust)

add_library(Graphics ${SOURCES})
warnings_as_errors(Graphics)

# Reuse existing public graphics contracts and platform type headers.
target_include_directories(Graphics
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../Graphics/Include/Shared"
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../Graphics/Include/Platform/${BABYLON_NATIVE_PLATFORM}"
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../Graphics/Include/RendererType/${GRAPHICS_API}"
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/InternalInclude/Babylon/Graphics")

target_link_libraries(Graphics
    PRIVATE arcana
    PRIVATE JsRuntimeInternal
    PRIVATE babylon_graphics_backend)

target_compile_definitions(Graphics
    PRIVATE NOMINMAX)

set_property(TARGET Graphics PROPERTY FOLDER Core)
set_property(TARGET Graphics PROPERTY UNITY_BUILD false)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

if(APPLE)
    # The shared platform headers use Objective-C declarations on Apple targets.
    target_compile_options(Graphics PUBLIC "SHELL:-x objective-c++")
endif()

add_library(GraphicsDevice INTERFACE)
target_include_directories(GraphicsDevice
    INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../Graphics/Include/Shared"
    INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../Graphics/Include/Platform/${BABYLON_NATIVE_PLATFORM}"
    INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../Graphics/Include/RendererType/${GRAPHICS_API}")
target_link_libraries(GraphicsDevice
    INTERFACE Graphics
    INTERFACE JsRuntime)

add_library(GraphicsDeviceContext INTERFACE)
target_include_directories(GraphicsDeviceContext
    INTERFACE "InternalInclude")
target_link_libraries(GraphicsDeviceContext
    INTERFACE Graphics
    INTERFACE JsRuntimeInternal
    INTERFACE arcana)

target_compile_definitions(GraphicsDeviceContext
    INTERFACE NOMINMAX)
