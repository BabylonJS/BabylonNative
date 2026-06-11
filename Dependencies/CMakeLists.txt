set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# --------------------------------------------------
# AndroidExtensions
# --------------------------------------------------
if(ANDROID OR BABYLON_NATIVE_BUILD_SOURCETREE)
    FetchContent_MakeAvailable_With_Message(AndroidExtensions)
endif()

# --------------------------------------------------
# arcana.cpp
# --------------------------------------------------
FetchContent_MakeAvailable_With_Message(arcana.cpp)

# --------------------------------------------------
# arcore-android-sdk
# --------------------------------------------------
if(ANDROID OR BABYLON_NATIVE_BUILD_SOURCETREE)
    FetchContent_MakeAvailable_With_Message(arcore-android-sdk)
endif()

# --------------------------------------------------
# base-n
# --------------------------------------------------
FetchContent_MakeAvailable_With_Message(base-n)

add_library(base-n INTERFACE)
target_include_directories(base-n INTERFACE "${base-n_SOURCE_DIR}/include")

# --------------------------------------------------
# bgfx.cmake
# --------------------------------------------------
set(BGFX_BUILD_EXAMPLES OFF)
set(BGFX_BUILD_TOOLS OFF)
set(BGFX_CONFIG_MULTITHREADED ON)
set(BGFX_CUSTOM_TARGETS OFF)
set(BGFX_INSTALL OFF)
set(BGFX_OPENGL_USE_EGL ON)
set(BGFX_USE_DEBUG_SUFFIX OFF)

FetchContent_MakeAvailable_With_Message(bgfx.cmake)

# Turn off debug annotations as it causes an access violation in D3D12.
# This flag is set using compile definitions because the bgfx.cmake option is ignored in debug configuration.
# See https://github.com/BabylonJS/bgfx.cmake/blob/0af3c9865a66aff1748a51bb466b24f05a123043/cmake/bgfx/bgfx.cmake#L126.
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_DEBUG_ANNOTATION=0)

target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_DEFAULT_MAX_ENCODERS=2)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MAX_VERTEX_STREAMS=18)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MIN_RESOURCE_COMMAND_BUFFER_SIZE=16)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MIN_UNIFORM_BUFFER_SIZE=4096)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_UNIFORM_BUFFER_RESIZE_THRESHOLD_SIZE=256)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_UNIFORM_BUFFER_RESIZE_INCREMENT_SIZE=1024)
target_compile_definitions(bgfx PUBLIC BGFX_PLATFORM_SUPPORTS_WGSL=0)

# Canvas plugin allocates one bgfx framebuffer per JS Canvas object and per
# text-rendering operation; combined with V8 GC pacing this can exceed the
# default 128 limit during long playground sweeps. We enforce a floor of 512:
# CI workflows pass -DBGFX_CONFIG_MAX_FRAME_BUFFERS, and any value below 512
# (including a smaller CI override) is clamped up so the pool never regresses
# below the size the Canvas sweeps need.
if(NOT BGFX_CONFIG_MAX_FRAME_BUFFERS OR BGFX_CONFIG_MAX_FRAME_BUFFERS LESS 512)
    set(BGFX_CONFIG_MAX_FRAME_BUFFERS 512)
endif()
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MAX_FRAME_BUFFERS=${BGFX_CONFIG_MAX_FRAME_BUFFERS})

# Temporary disable uniform debug.
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_DEBUG_UNIFORM=0)

if(GRAPHICS_API STREQUAL "D3D11")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_DIRECT3D11=1)
elseif(GRAPHICS_API STREQUAL "D3D12")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_DIRECT3D12=1)
elseif(GRAPHICS_API STREQUAL "Metal")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_METAL=1)
elseif(GRAPHICS_API STREQUAL "OpenGL")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_OPENGLES=30)
    target_compile_definitions(bgfx PRIVATE BGFX_GL_CONFIG_BLIT_EMULATION=1)
    target_compile_definitions(bgfx PRIVATE BGFX_GL_CONFIG_TEXTURE_READ_BACK_EMULATION=1)
elseif(GRAPHICS_API STREQUAL "Vulkan")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_VULKAN=1)
endif()
set_property(TARGET bimg PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET bimg_encode PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET bimg_decode PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET minz PROPERTY FOLDER Dependencies/bgfx/3rdparty)
if(TARGET tinyexr)
    set_property(TARGET tinyexr PROPERTY FOLDER Dependencies/bgfx/3rdparty)
endif()
set_property(TARGET bgfx PROPERTY FOLDER Dependencies/bgfx)
set_property(TARGET bimg PROPERTY FOLDER Dependencies/bgfx)
set_property(TARGET bx PROPERTY FOLDER Dependencies/bgfx)
target_compile_definitions(bx PRIVATE _CRT_SECURE_NO_WARNINGS)

# The updated tinyexr (vendored by bimg) uses fopen in its file I/O helpers,
# which MSVC flags as C4996 and fails the warnings-as-errors UWP build. Mirror
# the bx workaround above and silence the deprecation for the bimg targets that
# compile the tinyexr implementation.
foreach(_bimg_target IN ITEMS bimg_decode bimg_encode tinyexr)
    if(TARGET ${_bimg_target})
        target_compile_definitions(${_bimg_target} PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
endforeach()

# WebP parsing is provided by bimg's bundled simplewebp decoder. Toggle it
# through bimg's own config macro so disabling the option compiles the parser
# out of bimg_decode instead of pulling in a separate libwebp dependency.
if(NOT BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP AND TARGET bimg_decode)
    target_compile_definitions(bimg_decode PRIVATE BIMG_CONFIG_PARSE_WEBP=0)
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID AND CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|amd64|AMD64)")
    # The new bx requires SSE4.2 on x86_64 (see bx scripts/toolchain.lua and
    # include/bx/inline/simd128_sse.inl which uses _mm_round_ps / _mm_blendv_ps
    # under "SSE4.1 - always available with SSE4.2 minspec"). bgfx.cmake does
    # not propagate this flag, so add it explicitly on Linux x86_64 where
    # GCC/Clang don't enable SSE4.x by default. MSVC and Apple Silicon are
    # unaffected.
    foreach(_bx_target IN ITEMS bx bgfx bimg bimg_decode bimg_encode)
        if(TARGET ${_bx_target})
            target_compile_options(${_bx_target} PRIVATE -msse4.2)
        endif()
    endforeach()
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID)
    # Use GLVND libraries for EGL support in bgfx
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

# --------------------------------------------------
# glslang
# --------------------------------------------------
set(BUILD_EXTERNAL OFF)
set(ENABLE_SPVREMAPPER OFF)
set(ENABLE_GLSLANG_BINARIES OFF)
set(ENABLE_HLSL OFF)
set(ENABLE_OPT OFF)
set(ENABLE_CTEST OFF)
set(SKIP_GLSLANG_INSTALL ON)
# BABYLON_NATIVE_DISABLE_WEBMIN is used to disable WebMin in both glslang and SPIRV-Cross.
# This flag should be removed when the code coverage in these library corresponds to exactly what we use in Babylon.js
# and webGL2 glsl. 
if(BABYLON_NATIVE_DISABLE_WEBMIN)
    set(ENABLE_GLSLANG_WEBMIN OFF)
else()
    set(ENABLE_GLSLANG_WEBMIN ON)
endif()
set(ENABLE_GLSLANG_WEBMIN_DEVEL OFF)

if(NOT TARGET glslang)
    FetchContent_MakeAvailable_With_Message(glslang)

    set_property(TARGET GenericCodeGen PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET glslang PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET glslang-default-resource-limits PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET OGLCompiler PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET OSDependent PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET MachineIndependent PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET SPIRV PROPERTY FOLDER Dependencies/glslang)
endif()

# --------------------------------------------------
# googletest
# --------------------------------------------------
if(BABYLON_NATIVE_BUILD_APPS AND (WIN32 OR (APPLE AND NOT IOS AND NOT VISIONOS) OR (UNIX AND NOT ANDROID AND NOT APPLE)))
    if(WIN32)
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        # Default build type for my test projects are /MDd (MultiThreaded DLL) but GTests default to /MTd (MultiTreaded)
        # see https://github.com/google/googletest/blob/main/googletest/README.md
        # "Enabling this option will make gtest link the runtimes dynamically too, and match the project in which it is included."
        set(gtest_force_shared_crt OFF)
    endif()

    FetchContent_MakeAvailable_With_Message(googletest)

    set_property(TARGET gmock PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gmock_main PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gtest PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gtest_main PROPERTY FOLDER Dependencies/GoogleTest)
endif()

# --------------------------------------------------
# JsRuntimeHost
# --------------------------------------------------
FetchContent_MakeAvailable_With_Message(JsRuntimeHost)

# --------------------------------------------------
# metal-cpp
# --------------------------------------------------
if(APPLE OR BABYLON_NATIVE_BUILD_SOURCETREE)
    FetchContent_MakeAvailable_With_Message(metal-cpp)
endif()

if(APPLE)
    # Generate a single header.
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(METAL_CPP_GENERATED_DIR "${CMAKE_BINARY_DIR}/metal-cpp")
    execute_process(
        COMMAND ${Python3_EXECUTABLE}
            "${metal-cpp_SOURCE_DIR}/SingleHeader/MakeSingleHeader.py"
            "${metal-cpp_SOURCE_DIR}/Foundation/Foundation.hpp"
            "${metal-cpp_SOURCE_DIR}/Metal/Metal.hpp"
            "${metal-cpp_SOURCE_DIR}/QuartzCore/QuartzCore.hpp"
            -o "${METAL_CPP_GENERATED_DIR}/Include/Metal/Metal.hpp"
        RESULT_VARIABLE METAL_CPP_RESULT)
    if(NOT METAL_CPP_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to generate metal-cpp single header")
    endif()

    # Generate source that includes the single header with the necessary implementation defines.
    file(WRITE "${METAL_CPP_GENERATED_DIR}/Source/Metal.cpp"
        "#define NS_PRIVATE_IMPLEMENTATION\n"
        "#define MTL_PRIVATE_IMPLEMENTATION\n"
        "#define CA_PRIVATE_IMPLEMENTATION\n"
        "#include <Metal/Metal.hpp>\n")

    set(SOURCES
        "${METAL_CPP_GENERATED_DIR}/Include/Metal/Metal.hpp"
        "${METAL_CPP_GENERATED_DIR}/Source/Metal.cpp")

    add_library(metal-cpp ${SOURCES})
    target_include_directories(metal-cpp SYSTEM PUBLIC "${METAL_CPP_GENERATED_DIR}/Include")
    target_link_libraries(metal-cpp
        PRIVATE "-framework Foundation"
        PRIVATE "-framework Metal"
        PRIVATE "-framework QuartzCore")

    source_group(TREE "${metal-cpp_SOURCE_DIR}" FILES ${METAL_CPP_HEADERS})
    source_group(TREE "${METAL_CPP_GENERATED_DIR}" FILES ${SOURCES})
    set_property(TARGET metal-cpp PROPERTY FOLDER Dependencies)
endif()

# --------------------------------------------------
# SPIRV-Cross
# --------------------------------------------------
set(SPIRV_CROSS_CLI OFF)
set(SPIRV_CROSS_ENABLE_TESTS OFF)
set(SPIRV_CROSS_ENABLE_CPP OFF)
set(SPIRV_CROSS_ENABLE_REFLECT OFF)
set(SPIRV_CROSS_ENABLE_C_API OFF)
set(SPIRV_CROSS_ENABLE_UTIL OFF)
set(SPIRV_CROSS_SKIP_INSTALL ON)
# BABYLON_NATIVE_DISABLE_WEBMIN : see comment in section above.
if(BABYLON_NATIVE_DISABLE_WEBMIN)
    set(SPIRV_CROSS_ENABLE_WEBMIN OFF)
else()
    set(SPIRV_CROSS_ENABLE_WEBMIN ON)
endif()
if(NOT GRAPHICS_API STREQUAL "OpenGL")
    set(SPIRV_CROSS_ENABLE_GLSL OFF)
endif()
if(NOT GRAPHICS_API STREQUAL "Metal")
    set(SPIRV_CROSS_ENABLE_MSL OFF)
endif()
if(NOT GRAPHICS_API STREQUAL "D3D11" AND NOT GRAPHICS_API STREQUAL "D3D12")
    set(SPIRV_CROSS_ENABLE_HLSL OFF)
endif()
FetchContent_MakeAvailable_With_Message(SPIRV-Cross)
set_property(TARGET spirv-cross-core PROPERTY FOLDER Dependencies/SPIRV-Cross)
if(TARGET spirv-cross-glsl)
    set_property(TARGET spirv-cross-glsl PROPERTY FOLDER Dependencies/SPIRV-Cross)
endif()
if(TARGET spirv-cross-msl)
    set_property(TARGET spirv-cross-msl PROPERTY FOLDER Dependencies/SPIRV-Cross)
endif()
if(TARGET spirv-cross-hlsl)
    set_property(TARGET spirv-cross-hlsl PROPERTY FOLDER Dependencies/SPIRV-Cross)
endif()

# HACK: Re-enabled disabled warnings in SPIRV-Cross as they cause alerts in BinSkim
foreach(target spirv-cross-core spirv-cross-glsl spirv-cross-msl spirv-cross-hlsl)
    if(TARGET ${target})
        get_target_property(compileOptions ${target} COMPILE_OPTIONS)
        list(REMOVE_ITEM compileOptions /wd4267 /wd4996)
        set_target_properties(${target} PROPERTIES COMPILE_OPTIONS "${compileOptions}")
    endif()
endforeach()

# Disable warnings until unused parameter [-Wunused-parameter] get fixed.
if(TARGET SPIRV)
    disable_warnings(SPIRV)
endif()
if(TARGET MachineIndependent)
    disable_warnings(MachineIndependent)
endif()

# --------------------------------------------------
# WindowsAppSDK
# --------------------------------------------------
if(WINDOWS_STORE)
    add_subdirectory(WindowsAppSDK)
endif()

# --------------------------------------------------
# xr
# --------------------------------------------------
# Currently supported on Android via ARCore, and iOS via ARKit.
if((BABYLON_NATIVE_PLUGIN_NATIVEXR AND (ANDROID OR IOS)) OR BABYLON_NATIVE_BUILD_SOURCETREE)
    add_subdirectory(xr)
    set_property(TARGET xr PROPERTY FOLDER Dependencies/xr)
    warnings_as_errors(xr)
endif()
