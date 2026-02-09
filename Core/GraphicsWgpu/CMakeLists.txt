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
get_filename_component(BABYLON_NATIVE_REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(RUST_PROFILE_DIR "debug")
set(RUST_BUILD_FLAG "")
set(RUST_FEATURE_FLAGS "")
set(RUST_TARGET_TRIPLE "")
set(RUST_TARGET_FLAG "")
set(RUST_PLATFORM_ENV)
set(RUST_COMMON_ENV "CARGO_NET_GIT_FETCH_WITH_CLI=true")

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
    set(RUST_PROFILE_DIR "release")
    set(RUST_BUILD_FLAG "--release")
endif()

if(BABYLON_NATIVE_WGPU_USE_UPSTREAM_NATIVE)
    set(RUST_FEATURE_FLAGS "--features" "upstream_wgpu_native")
endif()

if(ANDROID)
    if(ANDROID_ABI STREQUAL "arm64-v8a")
        set(RUST_TARGET_TRIPLE "aarch64-linux-android")
    elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
        set(RUST_TARGET_TRIPLE "armv7-linux-androideabi")
    elseif(ANDROID_ABI STREQUAL "x86")
        set(RUST_TARGET_TRIPLE "i686-linux-android")
    elseif(ANDROID_ABI STREQUAL "x86_64")
        set(RUST_TARGET_TRIPLE "x86_64-linux-android")
    endif()
elseif(IOS)
    if(PLATFORM MATCHES "SIMULATOR")
        if(CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
            set(RUST_TARGET_TRIPLE "aarch64-apple-ios-sim")
        else()
            set(RUST_TARGET_TRIPLE "x86_64-apple-ios")
        endif()
        list(APPEND RUST_PLATFORM_ENV
            "IPHONEOS_DEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
            "RUSTFLAGS=-C link-arg=-mios-simulator-version-min=${DEPLOYMENT_TARGET}")
    else()
        set(RUST_TARGET_TRIPLE "aarch64-apple-ios")
        list(APPEND RUST_PLATFORM_ENV
            "IPHONEOS_DEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
            "RUSTFLAGS=-C link-arg=-miphoneos-version-min=${DEPLOYMENT_TARGET}")
    endif()
elseif(VISIONOS)
    if(PLATFORM MATCHES "SIMULATOR")
        set(RUST_TARGET_TRIPLE "aarch64-apple-xros-sim")
    else()
        set(RUST_TARGET_TRIPLE "aarch64-apple-xros")
    endif()
elseif(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(RUST_TARGET_TRIPLE "x86_64-pc-windows-msvc")
    else()
        set(RUST_TARGET_TRIPLE "i686-pc-windows-msvc")
    endif()
endif()

if(NOT RUST_TARGET_TRIPLE STREQUAL "")
    set(RUST_TARGET_FLAG "--target" "${RUST_TARGET_TRIPLE}")
endif()

if(WIN32)
    if(RUST_TARGET_TRIPLE STREQUAL "")
        set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_PROFILE_DIR}/babylon_graphics_backend.lib")
    else()
        set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_TARGET_TRIPLE}/${RUST_PROFILE_DIR}/babylon_graphics_backend.lib")
    endif()
else()
    if(RUST_TARGET_TRIPLE STREQUAL "")
        set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_PROFILE_DIR}/libbabylon_graphics_backend.a")
    else()
        set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_TARGET_TRIPLE}/${RUST_PROFILE_DIR}/libbabylon_graphics_backend.a")
    endif()
endif()

set(UPSTREAM_WGPU_NATIVE_TARGET "")
set(UPSTREAM_WGPU_NATIVE_OUTPUT_LIB "")
set(UPSTREAM_WGPU_NATIVE_LIB_DIR "")

if(BABYLON_NATIVE_WGPU_USE_UPSTREAM_NATIVE)
    if(NOT DEFINED wgpu_native_SOURCE_DIR OR wgpu_native_SOURCE_DIR STREQUAL "")
        message(FATAL_ERROR "BABYLON_NATIVE_WGPU_USE_UPSTREAM_NATIVE requires wgpu_native_SOURCE_DIR.")
    endif()

    set(UPSTREAM_WGPU_NATIVE_TARGET_DIR "${RUST_TARGET_DIR}/upstream-wgpu-native")
    set(UPSTREAM_WGPU_NATIVE_FEATURES "wgsl")
    if(APPLE)
        list(APPEND UPSTREAM_WGPU_NATIVE_FEATURES "metal")
    elseif(WIN32)
        # Keep Vulkan available for diagnostics, but default runtime selection remains DX12-first.
        list(APPEND UPSTREAM_WGPU_NATIVE_FEATURES "dx12" "vulkan")
    else()
        list(APPEND UPSTREAM_WGPU_NATIVE_FEATURES "vulkan")
    endif()
    string(JOIN "," UPSTREAM_WGPU_NATIVE_FEATURES_ARG ${UPSTREAM_WGPU_NATIVE_FEATURES})

    if(WIN32)
        if(RUST_TARGET_TRIPLE STREQUAL "")
            set(UPSTREAM_WGPU_NATIVE_OUTPUT_LIB "${UPSTREAM_WGPU_NATIVE_TARGET_DIR}/${RUST_PROFILE_DIR}/wgpu_native.lib")
        else()
            set(UPSTREAM_WGPU_NATIVE_OUTPUT_LIB "${UPSTREAM_WGPU_NATIVE_TARGET_DIR}/${RUST_TARGET_TRIPLE}/${RUST_PROFILE_DIR}/wgpu_native.lib")
        endif()
    else()
        if(RUST_TARGET_TRIPLE STREQUAL "")
            set(UPSTREAM_WGPU_NATIVE_OUTPUT_LIB "${UPSTREAM_WGPU_NATIVE_TARGET_DIR}/${RUST_PROFILE_DIR}/libwgpu_native.a")
        else()
            set(UPSTREAM_WGPU_NATIVE_OUTPUT_LIB "${UPSTREAM_WGPU_NATIVE_TARGET_DIR}/${RUST_TARGET_TRIPLE}/${RUST_PROFILE_DIR}/libwgpu_native.a")
        endif()
    endif()

    get_filename_component(UPSTREAM_WGPU_NATIVE_LIB_DIR "${UPSTREAM_WGPU_NATIVE_OUTPUT_LIB}" DIRECTORY)

    add_custom_command(
        OUTPUT "${UPSTREAM_WGPU_NATIVE_OUTPUT_LIB}"
        COMMAND ${CMAKE_COMMAND} -E env ${RUST_COMMON_ENV} ${RUST_PLATFORM_ENV} ${CARGO_COMMAND} build --manifest-path "${wgpu_native_SOURCE_DIR}/Cargo.toml" --target-dir "${UPSTREAM_WGPU_NATIVE_TARGET_DIR}" ${RUST_BUILD_FLAG} ${RUST_TARGET_FLAG} --no-default-features --features "${UPSTREAM_WGPU_NATIVE_FEATURES_ARG}"
        WORKING_DIRECTORY "${wgpu_native_SOURCE_DIR}"
        DEPENDS
            "${wgpu_native_SOURCE_DIR}/Cargo.toml"
            "${wgpu_native_SOURCE_DIR}/src/lib.rs"
            "${wgpu_native_SOURCE_DIR}/src/logging.rs"
        COMMENT "Building upstream wgpu-native static library"
        VERBATIM)

    set(UPSTREAM_WGPU_NATIVE_TARGET wgpu_native_upstream_backend_rust)
    add_custom_target(${UPSTREAM_WGPU_NATIVE_TARGET} DEPENDS "${UPSTREAM_WGPU_NATIVE_OUTPUT_LIB}")
endif()

set(RUST_BACKEND_ENV ${RUST_COMMON_ENV} ${RUST_PLATFORM_ENV})
if(BABYLON_NATIVE_WGPU_USE_UPSTREAM_NATIVE)
    list(APPEND RUST_BACKEND_ENV
        "BABYLON_WGPU_NATIVE_LIB_DIR=${UPSTREAM_WGPU_NATIVE_LIB_DIR}"
        "BABYLON_WGPU_NATIVE_LIB_NAME=wgpu_native")
endif()

add_custom_command(
    OUTPUT "${RUST_OUTPUT_LIB}"
    COMMAND ${CMAKE_COMMAND} -E env ${RUST_BACKEND_ENV} ${CARGO_COMMAND} build --manifest-path "${RUST_CRATE_DIR}/Cargo.toml" --target-dir "${RUST_TARGET_DIR}" ${RUST_BUILD_FLAG} ${RUST_TARGET_FLAG} ${RUST_FEATURE_FLAGS}
    WORKING_DIRECTORY "${RUST_CRATE_DIR}"
    DEPENDS
        "${BABYLON_NATIVE_REPO_ROOT}/Cargo.toml"
        "${RUST_CRATE_DIR}/Cargo.toml"
        "${RUST_CRATE_DIR}/build.rs"
        "${RUST_CRATE_DIR}/src/lib.rs"
    COMMENT "Building Rust WebGPU backend"
    VERBATIM)

add_custom_target(babylon_graphics_backend_rust DEPENDS "${RUST_OUTPUT_LIB}")
if(UPSTREAM_WGPU_NATIVE_TARGET)
    add_dependencies(babylon_graphics_backend_rust ${UPSTREAM_WGPU_NATIVE_TARGET})
endif()

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
