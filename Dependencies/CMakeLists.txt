set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# --------------------------------------------------
# AndroidExtensions
# --------------------------------------------------
if(ANDROID OR BABYLON_NATIVE_BUILD_SOURCETREE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/AndroidExtensions)
endif()

# --------------------------------------------------
# arcana.cpp
# --------------------------------------------------
FetchContent_MakeAvailable_With_Message(arcana.cpp)
set_property(TARGET arcana PROPERTY FOLDER Dependencies)

# --------------------------------------------------
# arcore-android-sdk
# --------------------------------------------------
if(ANDROID OR BABYLON_NATIVE_BUILD_SOURCETREE)
    FetchContent_MakeAvailable_With_Message(arcore-android-sdk)
endif()

# --------------------------------------------------
# UrlLib
# --------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/UrlLib)
set_property(TARGET UrlLib PROPERTY FOLDER Dependencies)

# --------------------------------------------------
# base-n
# --------------------------------------------------
add_library(base-n INTERFACE)
target_include_directories(base-n INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/base-n/include")
set_property(TARGET base-n PROPERTY FOLDER Dependencies)

# --------------------------------------------------
# JavaScript engine dependencies (JsRuntimeHost)
# --------------------------------------------------
# These are only needed for the non-default engines / the V8 inspector, so they
# are still fetched on demand rather than vendored in the monorepo.
if(NAPI_JAVASCRIPT_ENGINE STREQUAL "QuickJS")
    option(QJS_BUILD_LIBC "Build QuickJS with libc support." ON)
    FetchContent_MakeAvailable_With_Message(quickjs-ng)
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "Hermes")
    # Hermes support is experimental and limited to Windows, Android and Linux.
    # Bail out early rather than failing deep inside Hermes' Apple framework
    # machinery.
    if(APPLE OR IOS)
        message(FATAL_ERROR
            "NAPI_JAVASCRIPT_ENGINE=Hermes is not supported on Apple platforms.")
    endif()

    # HERMES_ENABLE_TOOLS must stay ON: even with HERMES_ENABLE_TEST_SUITE OFF,
    # Hermes unconditionally adds external/node-api-cts and
    # external/node-api-tests whenever HERMES_ENABLE_NAPI is on, and those refer
    # to the `hermes` CLI target via $<TARGET_FILE:hermes>. Nothing ships in our
    # binaries because Hermes is declared EXCLUDE_FROM_ALL.
    set(HERMES_ENABLE_TOOLS ON CACHE BOOL "" FORCE)
    set(HERMES_ENABLE_TEST_SUITE OFF CACHE BOOL "" FORCE)
    set(HERMES_ENABLE_DEBUGGER OFF CACHE BOOL "" FORCE)
    set(HERMES_BUILD_APPLE_FRAMEWORK OFF CACHE BOOL "" FORCE)
    set(HERMES_ENABLE_NAPI ON CACHE BOOL "" FORCE)
    # Hermes defaults HERMES_SLOW_DEBUG=ON, which compiles internal sanity
    # checks in even for optimized builds; they can SIGABRT during Runtime
    # teardown. Embedders don't need them.
    set(HERMES_SLOW_DEBUG OFF CACHE BOOL "" FORCE)
    if(ANDROID)
        # Hermes only skips its ICU lookup on Android when HERMES_IS_ANDROID is
        # set, which is part of Hermes' React Native / fbjni packaging that we
        # don't use. Unicode-lite avoids pulling ICU and fbjni into this
        # standalone NDK build.
        set(HERMES_UNICODE_LITE ON CACHE BOOL "" FORCE)
    endif()
    if(WIN32)
        # The vendored Boost.Context assembler backend breaks under MSBuild; the
        # Win32 Fiber implementation needs no assembler and is functionally
        # equivalent for Hermes' usage.
        set(BOOST_CONTEXT_IMPLEMENTATION "winfib" CACHE STRING "" FORCE)
    endif()

    # ------------------------------------------------------------------
    # Cross-compilation: bootstrap the Hermes host compilers automatically.
    #
    # `hermesc`/`shermes` are HOST tools Hermes runs at build time. When
    # cross-compiling (the Android NDK build) the in-tree tool targets are built
    # for the *target* and can't run on the build host, so Hermes has to be
    # handed prebuilt host compilers through IMPORT_HOST_COMPILERS.
    #
    # This has to happen at configure time, before Hermes itself is configured,
    # because Hermes consumes IMPORT_HOST_COMPILERS during its own configure. An
    # explicit -DIMPORT_HOST_COMPILERS=... still wins and skips the bootstrap.
    set(_bn_hermes_populated_early FALSE)
    if(CMAKE_CROSSCOMPILING AND NOT IMPORT_HOST_COMPILERS)
        # Source-relative (not ${CMAKE_SOURCE_DIR}-relative: the Android build
        # adds BabylonNative as a subdirectory of the Gradle project) and shared
        # across target ABIs so the host tools are built once. /Build is
        # git-ignored.
        set(_bn_hermes_host_dir "${CMAKE_CURRENT_LIST_DIR}/../Build/HermesHostTools")
        set(_bn_hermes_host_import "${_bn_hermes_host_dir}/ImportHostCompilers.cmake")

        if(NOT EXISTS "${_bn_hermes_host_import}")
            message(STATUS "Hermes: bootstrapping host compilers (hermesc/shermes) for cross-compilation")

            # Download Hermes now so the host build can be configured from the
            # same sources the target build will use.
            FetchContent_GetProperties(hermes)
            if(NOT hermes_POPULATED)
                FetchContent_Populate(hermes)
                set(_bn_hermes_populated_early TRUE)
            endif()

            find_program(_bn_host_ninja ninja)
            if(_bn_host_ninja)
                set(_bn_host_generator -G Ninja)
            else()
                set(_bn_host_generator "")
            endif()

            # No toolchain file is passed, so CMake picks the host compiler.
            execute_process(
                COMMAND "${CMAKE_COMMAND}"
                    ${_bn_host_generator}
                    -S "${hermes_SOURCE_DIR}"
                    -B "${_bn_hermes_host_dir}"
                    -D CMAKE_BUILD_TYPE=Release
                    -D HERMES_UNICODE_LITE=ON
                    -D HERMES_ENABLE_TEST_SUITE=OFF
                    -D HERMES_ENABLE_NAPI=OFF
                RESULT_VARIABLE _bn_host_cfg_result)
            if(NOT _bn_host_cfg_result EQUAL 0)
                message(FATAL_ERROR
                    "Hermes host-compiler bootstrap: configure failed (${_bn_host_cfg_result}).")
            endif()

            execute_process(
                COMMAND "${CMAKE_COMMAND}" --build "${_bn_hermes_host_dir}"
                    --target hermesc shermes --config Release
                RESULT_VARIABLE _bn_host_build_result)
            if(NOT _bn_host_build_result EQUAL 0)
                message(FATAL_ERROR
                    "Hermes host-compiler bootstrap: build failed (${_bn_host_build_result}).")
            endif()
        endif()

        if(NOT EXISTS "${_bn_hermes_host_import}")
            message(FATAL_ERROR
                "Hermes host-compiler bootstrap did not produce ${_bn_hermes_host_import}.")
        endif()

        set(IMPORT_HOST_COMPILERS "${_bn_hermes_host_import}" CACHE FILEPATH
            "Imported Hermes host compilers (hermesc/shermes) for cross-compilation" FORCE)
        message(STATUS "Hermes: using bootstrapped host compilers at ${IMPORT_HOST_COMPILERS}")
    endif()

    if(_bn_hermes_populated_early)
        # FetchContent_MakeAvailable is a no-op once the content is populated,
        # so add the subdirectory ourselves (mirroring the EXCLUDE_FROM_ALL on
        # the declaration).
        message(STATUS "Fetching hermes - done")
        add_subdirectory("${hermes_SOURCE_DIR}" "${hermes_BINARY_DIR}" EXCLUDE_FROM_ALL)
    else()
        FetchContent_MakeAvailable_With_Message(hermes)
    endif()

    # FetchContent only defines `hermes_SOURCE_DIR` in this directory scope, but
    # Core/Node-API needs it to reach Hermes' non-installed `API/napi` headers
    # (upstream JsRuntimeHost fetched Hermes from its own root, where the
    # variable propagated down automatically).
    set(hermes_SOURCE_DIR "${hermes_SOURCE_DIR}" CACHE INTERNAL "Hermes source directory")
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8" AND JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR)
    FetchContent_MakeAvailable_With_Message(asio)
    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE "${asio_SOURCE_DIR}/asio/include")
    set_property(TARGET asio PROPERTY FOLDER Dependencies)

    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
    FetchContent_MakeAvailable_With_Message(llhttp)
    set_property(TARGET llhttp_static PROPERTY FOLDER Dependencies)

    # Core/AppRuntime/V8Inspector consumes `${llhttp_SOURCE_DIR}/include`, which
    # FetchContent only defines in this directory scope.
    set(llhttp_SOURCE_DIR "${llhttp_SOURCE_DIR}" CACHE INTERNAL "llhttp source directory")
endif()

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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bgfx.cmake)

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

# Disable video decoding support (not used by Babylon Native).
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_VIDEO=0)

# Disable the C99 API (Babylon Native uses the C++ API only); saves binary size.
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_C99_API=0)

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

if(NOT APPLE AND NOT ANDROID AND CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|amd64|AMD64)" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # The new bx requires SSE4.2 on x86_64 (see bx scripts/toolchain.lua and
    # include/bx/inline/simd128_sse.inl which uses _mm_round_ps / _mm_blendv_ps
    # under "SSE4.1 - always available with SSE4.2 minspec"). bgfx.cmake does
    # not propagate this flag, so add it explicitly for any GCC/Clang frontend on
    # x86_64 (e.g. Linux, or Clang targeting Windows), none of which enable SSE4.x
    # by default. MSVC (cl.exe) allows the intrinsics regardless and Apple Silicon
    # is unaffected.
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
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glslang)

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
if(BABYLON_NATIVE_BUILD_APPS AND NOT ANDROID AND NOT VISIONOS)
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
# metal-cpp
# --------------------------------------------------
if(APPLE)
    # metal-cpp is consumed as a header-only library, and the header comes from
    # bgfx rather than from a separate copy of the metal-cpp repo.
    #
    # The metal-cpp implementation (the NS / MTL / CA `*_PRIVATE_IMPLEMENTATION`
    # globals) is already compiled into bgfx's Metal renderer, which embeds its
    # own single-header copy of metal-cpp (see bgfx's renderer_mtl.cpp).
    # Compiling a second copy here would define those globals in both
    # libmetal-cpp.a and libbgfx.a; whether that produces ~1900 "duplicate
    # symbol" link errors then depends purely on the (fragile) order in which
    # the two static archives appear on the link line. So we expose headers
    # only and let the single bgfx-provided implementation satisfy the symbols
    # at link time. Graphics — the only consumer — always links bgfx on Apple,
    # so the implementation symbols always resolve.
    #
    # Using bgfx's own header (instead of vendoring the metal-cpp repo and
    # re-running its MakeSingleHeader.py) keeps declarations and implementation
    # at the exact same metal-cpp version, and drops both the second copy of
    # the sources and the configure-time Python dependency.
    set(METAL_CPP_GENERATED_DIR "${CMAKE_BINARY_DIR}/metal-cpp")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/bgfx.cmake/bgfx/3rdparty/metal-cpp/metal.hpp"
        "${METAL_CPP_GENERATED_DIR}/Include/Metal/Metal.hpp"
        COPYONLY)

    add_library(metal-cpp INTERFACE)
    target_include_directories(metal-cpp SYSTEM INTERFACE "${METAL_CPP_GENERATED_DIR}/Include")
    target_link_libraries(metal-cpp
        INTERFACE "-framework Foundation"
        INTERFACE "-framework Metal"
        INTERFACE "-framework QuartzCore")
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
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Cross)
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
