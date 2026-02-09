set(SOURCES
    "Include/Babylon/Polyfills/Canvas.h"
    "Source/Canvas.cpp"
    "Source/Canvas.h"
    "Source/Colors.h"
    "Source/Font.cpp"
    "Source/Font.h"
    "Source/Gradient.cpp"
    "Source/Gradient.h"
    "Source/Image.cpp"
    "Source/Image.h"
    "Source/ImageData.cpp"
    "Source/ImageData.h"
    "Source/LineCaps.h"
    "Source/MeasureText.cpp"
    "Source/MeasureText.h"
    "Source/Path2D.cpp"
    "Source/Path2D.h"
    "Source/Context.cpp"
    "Source/Context.h"
    "Source/nanosvg.h"
    "Source/nanovg/nanovg.h"
    "Source/nanovg/nanovg_filterstack.h")

set(CARGO_COMMAND)
find_program(CARGO_EXECUTABLE cargo)
if(CARGO_EXECUTABLE)
    set(CARGO_COMMAND "${CARGO_EXECUTABLE}")
else()
    find_program(RUSTUP_EXECUTABLE rustup)
    if(NOT RUSTUP_EXECUTABLE)
        message(FATAL_ERROR "CanvasWgpu backend requires cargo in PATH or rustup in PATH.")
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
set(RUST_TARGET_TRIPLE "")
set(RUST_TARGET_FLAG "")
set(RUST_PLATFORM_ENV)

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
    set(RUST_PROFILE_DIR "release")
    set(RUST_BUILD_FLAG "--release")
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
        set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_PROFILE_DIR}/babylon_canvas_wgpu_backend.lib")
    else()
        set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_TARGET_TRIPLE}/${RUST_PROFILE_DIR}/babylon_canvas_wgpu_backend.lib")
    endif()
else()
    if(RUST_TARGET_TRIPLE STREQUAL "")
        set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_PROFILE_DIR}/libbabylon_canvas_wgpu_backend.a")
    else()
        set(RUST_OUTPUT_LIB "${RUST_TARGET_DIR}/${RUST_TARGET_TRIPLE}/${RUST_PROFILE_DIR}/libbabylon_canvas_wgpu_backend.a")
    endif()
endif()

add_custom_command(
    OUTPUT "${RUST_OUTPUT_LIB}"
    COMMAND ${CMAKE_COMMAND} -E env ${RUST_PLATFORM_ENV} ${CARGO_COMMAND} build --manifest-path "${RUST_CRATE_DIR}/Cargo.toml" --target-dir "${RUST_TARGET_DIR}" ${RUST_BUILD_FLAG} ${RUST_TARGET_FLAG}
    WORKING_DIRECTORY "${RUST_CRATE_DIR}"
    DEPENDS
        "${BABYLON_NATIVE_REPO_ROOT}/Cargo.toml"
        "${RUST_CRATE_DIR}/Cargo.toml"
        "${RUST_CRATE_DIR}/src/lib.rs"
    COMMENT "Building Rust CanvasWgpu (femtovg) backend"
    VERBATIM)

add_custom_target(babylon_canvas_wgpu_backend_rust DEPENDS "${RUST_OUTPUT_LIB}")
if(TARGET babylon_graphics_backend_rust)
    # Both Rust backends share the same workspace + target-dir. Serialize
    # builds to avoid concurrent cargo writes into the shared target tree.
    add_dependencies(babylon_canvas_wgpu_backend_rust babylon_graphics_backend_rust)
endif()

add_library(babylon_canvas_wgpu_backend STATIC IMPORTED GLOBAL)
set_target_properties(babylon_canvas_wgpu_backend PROPERTIES IMPORTED_LOCATION "${RUST_OUTPUT_LIB}")
add_dependencies(babylon_canvas_wgpu_backend babylon_canvas_wgpu_backend_rust)

add_library(Canvas ${SOURCES})
warnings_as_errors(Canvas)
target_compile_options(Canvas PRIVATE -Wno-unused-parameter -Wno-unused-variable)

target_include_directories(Canvas
    PUBLIC "Include"
    PRIVATE "Source")

target_link_libraries(Canvas
    PUBLIC napi
    PRIVATE base-n
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDeviceContext
    PRIVATE UrlLib
    PRIVATE babylon_canvas_wgpu_backend)

set_property(TARGET Canvas PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
