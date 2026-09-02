cmake_minimum_required(VERSION 3.21)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------

# CMakeExtensions is required before project() for its helper functions.
include("${CMAKE_CURRENT_LIST_DIR}/Dependencies/CMakeExtensions/CMakeLists.txt")

# EXCLUDE_FROM_ALL prevents fetched dependencies from automatically adding
# targets to the ALL_BUILD target.
FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG fe41b17dedea47b5dead98f0271ca284e94ac6a8
    EXCLUDE_FROM_ALL)
FetchContent_Declare(arcore-android-sdk
    GIT_REPOSITORY https://github.com/google-ar/arcore-android-sdk.git
    GIT_TAG 98cb803de5482fb2b36effe8be3b5a0d3b726976
    EXCLUDE_FROM_ALL)
FetchContent_Declare(base-n
    GIT_REPOSITORY https://github.com/azawadzki/base-n.git
    GIT_TAG 7573e77c0b9b0e8a5fb63d96dbde212c921993b4
    EXCLUDE_FROM_ALL)
FetchContent_Declare(draco
    GIT_REPOSITORY https://github.com/google/draco.git
    GIT_TAG 1.5.7
    EXCLUDE_FROM_ALL)
FetchContent_Declare(meshoptimizer
    GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
    GIT_TAG v0.22
    EXCLUDE_FROM_ALL)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz"
    EXCLUDE_FROM_ALL)
FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.5.0
    EXCLUDE_FROM_ALL)
FetchContent_Declare(metal-cpp
    GIT_REPOSITORY https://github.com/bkaradzic/metal-cpp.git
    GIT_TAG metal-cpp_26
    EXCLUDE_FROM_ALL)
# --------------------------------------------------

# BABYLON_NATIVE_BUILD_SOURCETREE flag is used by BabylonReactNative NPM build process.
# CMake script is run twice. At package creation, BABYLON_NATIVE_BUILD_SOURCETREE is ON
# which makes all dependencies to be fetched and packaged.
# At build time, cmake is run again on BabylonNative root but FETCHCONTENT_FULLY_DISCONNECTED is ON
# and FetchContent_MakeAvailable_With_Message will use provided local folder provided to cmake command line
# like '-DFETCHCONTENT_SOURCE_DIR_BGFX.CMAKE=../shared/Babylon/....'

if(VISIONOS OR IOS OR BABYLON_NATIVE_BUILD_SOURCETREE)
    FetchContent_MakeAvailable_With_Message(ios-cmake)
    if(NOT BABYLON_NATIVE_BUILD_SOURCETREE)
        set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    endif()
    set(ENABLE_ARC OFF CACHE STRING "Enables or disables ARC support.")
endif()

if(VISIONOS)
    set(PLATFORM "VISIONOSCOMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "1.0" CACHE STRING "")
elseif(IOS)
    set(PLATFORM "OS64COMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "13" CACHE STRING "")
endif()

project(BabylonNative)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress "Performing Test ..." output from check_c_compiler_flag() and similar macros.
set(CMAKE_REQUIRED_QUIET TRUE)

# --------------------------------------------------
# Options
# --------------------------------------------------

# General
option(BABYLON_NATIVE_BUILD_APPS "Build Babylon Native apps." ${PROJECT_IS_TOP_LEVEL})
option(BABYLON_NATIVE_INSTALL "Include the install target." ${PROJECT_IS_TOP_LEVEL})
option(BABYLON_DEBUG_TRACE "Enable debug trace." OFF)

# WARNING: This is experimental. Only use it if you can ensure that your application will properly handle thread affinity.
option(BABYLON_NATIVE_CHECK_THREAD_AFFINITY "Checks thread safety in the graphics device calls. It can be removed if hosting application ensures thread coherence." ON)

# Plugins
option(BABYLON_NATIVE_PLUGIN_EXTERNALTEXTURE "Include Babylon Native Plugin ExternalTexture." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAMERA "Include Babylon Native Plugin NativeCamera." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAPTURE "Include Babylon Native Plugin NativeCapture." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEDRACO "Include Babylon Native Plugin NativeDraco." OFF)
option(BABYLON_NATIVE_PLUGIN_NATIVEENCODING "Include Babylon Native Plugin NativeEncoding." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE "Include Babylon Native Plugin NativeEngine." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP "Enable WebP image parsing in NativeEngine (via bimg)." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES "Enable image loading in NativeEngine (LoadTexture, image bitmap, bimg)." ON)
if(NOT BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
    # WebP support relies on image loading code paths; force it off when image loading is disabled.
    set(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP OFF CACHE BOOL "Enable WebP image parsing in NativeEngine (via bimg)." FORCE)
endif()
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_COMPILESHADERS "Include Babylon Native Plugin NativeEngine - Compile Shaders." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEINPUT "Include Babylon Native Plugin NativeInput." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEMESHOPT "Include Babylon Native Plugin NativeMeshopt." OFF)
option(BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS "Include Babylon Native Plugin NativeOptimizations." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVETRACING "Include Babylon Native Plugin NativeTracing." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEXR "Include Babylon Native Plugin XR." ON)
option(BABYLON_NATIVE_PLUGIN_SHADERCACHE "Include Babylon Native Plugin ShaderCache." ON)
option(BABYLON_NATIVE_PLUGIN_SHADERCOMPILER "Include Babylon Native Plugin ShaderCompiler." ON)
option(BABYLON_NATIVE_PLUGIN_SHADERTOOL "Include Babylon Native Plugin ShaderTool." ON)
option(BABYLON_NATIVE_PLUGIN_TESTUTILS "Include Babylon Native Plugin TestUtils." ON)

# Polyfills
option(BABYLON_NATIVE_POLYFILL_ABORTCONTROLLER "Include Babylon Native Polyfill AbortController." ON)
option(BABYLON_NATIVE_POLYFILL_CANVAS "Include Babylon Native Polyfill Canvas." ON)
option(BABYLON_NATIVE_POLYFILL_SCHEDULING "Include Babylon Native Polyfill Scheduling." ON)
option(BABYLON_NATIVE_POLYFILL_URL "Include Babylon Native Polyfill URL." ON)
option(BABYLON_NATIVE_POLYFILL_WEBSOCKET "Include Babylon Native Polyfill WebSocket." ON)
option(BABYLON_NATIVE_POLYFILL_WINDOW "Include Babylon Native Polyfill Window." ON)

# Embedding
option(BABYLON_NATIVE_EMBEDDING "Build the cross-platform Babylon::Embedding facade (Runtime + View)." ON)
option(BABYLON_NATIVE_EMBEDDING_ANDROID "Build the Android JNI interop layer for Babylon::Embedding." OFF)
# Default ON for Apple platforms — the iOS Playground (and future
# macOS / visionOS Playground migrations) consume it directly. Apple
# hosts that don't want the interop layer can explicitly disable.
option(BABYLON_NATIVE_EMBEDDING_APPLE "Build the Apple (iOS / macOS / visionOS) Obj-C++ interop layer for Babylon::Embedding." ${APPLE})

# Sanitizers
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)

# RTTI
# Babylon Native does not use dynamic_cast or typeid anywhere in Core, Plugins,
# Polyfills, Embedding or Apps, but compilers emit RTTI records for every
# polymorphic class by default. Disabling it is a pure size win. UBSan's vptr
# check is the only thing that needs RTTI, so it is derived from the sanitizer
# switch rather than exposed as an option of its own.
#
# Deliberately a plain variable and not a cache entry: a cached ENABLE_RTTI
# would persist ON in any build directory that was configured with
# -DENABLE_SANITIZERS=ON even once, so re-configuring that directory without
# sanitizers would silently keep emitting RTTI and lose the size win.
set(ENABLE_RTTI ${ENABLE_SANITIZERS})

if(ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(SANITIZERS "address,undefined")
        # Check for Clang since vptr and fdsan are Clang-specific
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # vptr is not supported on Windows.
            if(NOT WIN32)
                list(APPEND SANITIZERS "vptr")
            endif()
            # FDSan only works on Android builds with Clang
            if (ANDROID)
                list(APPEND SANITIZERS "fdsan")
            endif()
        endif()

        string(JOIN "," SANITIZER_FLAGS ${SANITIZERS})

        # -fno-sanitize-recover=all makes UBSan findings fatal. Without it, UBSan
        # only prints a diagnostic to stderr and execution continues, so CI stays
        # green while latent UB goes unreported.
        add_compile_options(-fsanitize=${SANITIZER_FLAGS} -fno-omit-frame-pointer -fno-sanitize-recover=all)
        add_link_options(-fsanitize=${SANITIZER_FLAGS})
    elseif(MSVC)
        # MSVC only supports AddressSanitizer
        add_compile_options(/fsanitize=address /Zi /Od)
    else()
        message(FATAL_ERROR "Sanitizers not supported on this compiler.")
    endif()
endif()

# --------------------------------------------------

if(ANDROID)
    set(BABYLON_NATIVE_PLATFORM "Android")
elseif(IOS)
    set(BABYLON_NATIVE_PLATFORM "iOS")
elseif (VISIONOS)
    set(BABYLON_NATIVE_PLATFORM "visionOS")
elseif(APPLE)
    set(BABYLON_NATIVE_PLATFORM "macOS")
elseif(WINDOWS_STORE)
    set(BABYLON_NATIVE_PLATFORM "WinRT")
elseif(WIN32)
    set(BABYLON_NATIVE_PLATFORM "Win32")
elseif(UNIX)
    set(BABYLON_NATIVE_PLATFORM "Unix")
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(APPLE)
    set(GRAPHICS_API Metal)
elseif(UNIX)
    if(NOT GRAPHICS_API)
        set(GRAPHICS_API OpenGL)
    else()
        if(NOT GRAPHICS_API STREQUAL Vulkan AND NOT GRAPHICS_API STREQUAL OpenGL)
            message(FATAL_ERROR "Unrecognized/Unsupported render API: ${GRAPHICS_API}")
        endif()
    endif()
elseif(WIN32)
    if(NOT GRAPHICS_API)
        set(GRAPHICS_API D3D11)
    elseif(GRAPHICS_API STREQUAL "OpenGLWindowsDevOnly")
        # OpenGLES via ANGLE from a Chromium-based browser (dev-only).
        set(GRAPHICS_API OpenGL)
        set(BABYLON_NATIVE_OPENGLES_FROM_BROWSER ON)
    else()
        if(NOT GRAPHICS_API STREQUAL Vulkan AND NOT GRAPHICS_API STREQUAL D3D11 AND NOT GRAPHICS_API STREQUAL D3D12)
            message(FATAL_ERROR "Unrecognized/Unsupported render API: ${GRAPHICS_API}")
        endif()
    endif()
endif()

# Find ANGLE libraries from Edge or Chrome when using OpenGLWindowsDevOnly.
if(BABYLON_NATIVE_OPENGLES_FROM_BROWSER)
    foreach(_browser_exe "msedge.exe" "chrome.exe")
        if(ANGLE_LIBEGL)
            break()
        endif()
        foreach(_reg_root "HKLM" "HKCU")
            if(ANGLE_LIBEGL)
                break()
            endif()
            execute_process(
                COMMAND reg query "${_reg_root}\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\${_browser_exe}" /ve
                OUTPUT_VARIABLE _reg_output
                ERROR_QUIET
                RESULT_VARIABLE _reg_result
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(NOT _reg_result EQUAL 0)
                continue()
            endif()
            string(REGEX MATCH "REG_SZ[ \t]+([^\r\n]+)" _ "${_reg_output}")
            string(STRIP "${CMAKE_MATCH_1}" _browser_path)
            if(NOT _browser_path)
                continue()
            endif()
            cmake_path(GET _browser_path PARENT_PATH _browser_dir)
            file(GLOB _version_dirs LIST_DIRECTORIES true "${_browser_dir}/[0-9]*")
            list(SORT _version_dirs ORDER DESCENDING)
            foreach(_version_dir ${_version_dirs})
                if(IS_DIRECTORY "${_version_dir}" AND EXISTS "${_version_dir}/libEGL.dll")
                    set(ANGLE_LIBEGL "${_version_dir}/libEGL.dll")
                    set(ANGLE_LIBGLESV2 "${_version_dir}/libGLESv2.dll")
                    message(STATUS "Found ANGLE libraries in: ${_version_dir}")
                    break()
                endif()
            endforeach()
        endforeach()
    endforeach()

    if(NOT ANGLE_LIBEGL)
        message(FATAL_ERROR "OpenGLWindowsDevOnly requires ANGLE libraries (libEGL.dll, libGLESv2.dll) from Edge or Chrome, but none were found.")
    endif()

    # The browser ships ANGLE as DLLs with no import libraries. bgfx resolves its GL entry
    # points dynamically, but code that calls GL ES directly needs something to link against,
    # so synthesize an import library from the DLL's export table.
    get_filename_component(_msvc_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    find_program(ANGLE_DUMPBIN_EXECUTABLE NAMES dumpbin dumpbin.exe HINTS "${_msvc_bin_dir}")
    find_program(ANGLE_LIB_EXECUTABLE NAMES lib lib.exe HINTS "${_msvc_bin_dir}")
    if(NOT ANGLE_DUMPBIN_EXECUTABLE OR NOT ANGLE_LIB_EXECUTABLE)
        message(FATAL_ERROR "OpenGLWindowsDevOnly requires the MSVC dumpbin and lib tools to generate an ANGLE import library.")
    endif()

    set(_angle_gen_dir "${CMAKE_BINARY_DIR}/ANGLE")
    set(ANGLE_GLESV2_IMPORT_LIB "${_angle_gen_dir}/libGLESv2.lib")

    if(NOT EXISTS "${ANGLE_GLESV2_IMPORT_LIB}" OR "${ANGLE_LIBGLESV2}" IS_NEWER_THAN "${ANGLE_GLESV2_IMPORT_LIB}")
        execute_process(
            COMMAND "${ANGLE_DUMPBIN_EXECUTABLE}" /nologo /exports "${ANGLE_LIBGLESV2}"
            OUTPUT_VARIABLE _angle_exports
            RESULT_VARIABLE _angle_exports_result
            ERROR_VARIABLE _angle_exports_error
        )
        if(NOT _angle_exports_result EQUAL 0)
            message(FATAL_ERROR "Failed to read the exports of ${ANGLE_LIBGLESV2}: ${_angle_exports_error}")
        endif()

        # Export table rows look like "  1422   58D 00023330 glIsTexture"; keep the names.
        string(REGEX MATCHALL "[0-9A-F]+ +gl[A-Za-z0-9_]+" _angle_export_rows "${_angle_exports}")
        set(_angle_def_exports "")
        foreach(_angle_row IN LISTS _angle_export_rows)
            string(REGEX REPLACE "^.+ +" "" _angle_symbol "${_angle_row}")
            string(APPEND _angle_def_exports "    ${_angle_symbol}\n")
        endforeach()
        if(NOT _angle_def_exports)
            message(FATAL_ERROR "Found no GL ES entry points exported by ${ANGLE_LIBGLESV2}.")
        endif()

        if(CMAKE_VS_PLATFORM_NAME STREQUAL "Win32")
            set(_angle_machine "x86")
        elseif(CMAKE_VS_PLATFORM_NAME)
            set(_angle_machine "${CMAKE_VS_PLATFORM_NAME}")
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(_angle_machine "x86")
        else()
            set(_angle_machine "x64")
        endif()

        file(MAKE_DIRECTORY "${_angle_gen_dir}")
        file(WRITE "${_angle_gen_dir}/libGLESv2.def" "LIBRARY libGLESv2\nEXPORTS\n${_angle_def_exports}")
        execute_process(
            COMMAND "${ANGLE_LIB_EXECUTABLE}" /nologo "/def:${_angle_gen_dir}/libGLESv2.def" "/machine:${_angle_machine}" "/out:${ANGLE_GLESV2_IMPORT_LIB}"
            RESULT_VARIABLE _angle_lib_result
            OUTPUT_QUIET
            ERROR_VARIABLE _angle_lib_error
        )
        if(NOT _angle_lib_result EQUAL 0)
            message(FATAL_ERROR "Failed to generate an ANGLE import library: ${_angle_lib_error}")
        endif()
        message(STATUS "Generated ANGLE import library: ${ANGLE_GLESV2_IMPORT_LIB}")
    endif()
endif()

if(APPLE)
    set(BABYLON_NATIVE_PLATFORM_IMPL_EXT "mm")
else()
    set(BABYLON_NATIVE_PLATFORM_IMPL_EXT "cpp")
endif()

if(MSVC)
    # __cplusplus value is not in sync with language version used. MVSC needs this flag to update it accordingly
    # https://gitlab.kitware.com/cmake/cmake/-/issues/18837
    add_compile_options(/Zc:__cplusplus)

    # https://devblogs.microsoft.com/cppblog/announcing-full-support-for-a-c-c-conformant-preprocessor-in-msvc/
    add_compile_options(/Zc:preprocessor)

    # default to /W3
    add_compile_options(/W3)

    # Enable multiprocessor compilation for faster builds
    add_compile_options(/MP)

    # Strip unreferenced code from RelWithDebInfo binaries.
    #
    # MSVC turns /OPT:REF and /OPT:ICF off whenever /DEBUG is passed, and CMake's
    # default RelWithDebInfo link flags are "/debug /INCREMENTAL". The effect is that
    # RelWithDebInfo keeps every unreferenced COMDAT that made it into the static
    # libraries.
    #
    # /OPT:REF alone captures the entire win (/OPT:ICF measured zero additional
    # bytes), so identical functions are left unfolded and stack traces stay
    # unambiguous. Incremental linking must be off because it is incompatible with
    # /OPT:REF. Debug is left untouched, and Release/MinSizeRel already get /OPT:REF
    # implicitly because they do not link with /DEBUG.
    add_link_options($<$<CONFIG:RelWithDebInfo>:/INCREMENTAL:NO> $<$<CONFIG:RelWithDebInfo>:/OPT:REF>)
endif()

if(NOT ENABLE_RTTI)
    # Applied to C++ only: /GR- and -fno-rtti are not valid for C sources.
    # OBJCXX is listed explicitly so Objective-C++ (.mm) sources are covered on
    # Apple regardless of whether CMake classifies them as CXX or OBJCXX.
    if(MSVC)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX,OBJCXX>:/GR->)
    else()
        add_compile_options($<$<COMPILE_LANGUAGE:CXX,OBJCXX>:-fno-rtti>)
    endif()
endif()

# Re-enables RTTI for a single target. Needed by dependencies that genuinely
# require it; see the call sites for the specific reason in each case.
# Call sites gate on the configuration that provides the target, so an
# unresolved name is a mistake and target_compile_options reports it.
function(babylon_native_force_rtti target)
    if(MSVC)
        target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX,OBJCXX>:/GR>)
    else()
        target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX,OBJCXX>:-frtti>)
    endif()
endfunction()

if(APPLE)
    # Create scheme for installation and other targets
    set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
endif()

if(BABYLON_DEBUG_TRACE)
    add_compile_definitions(BABYLON_DEBUG_TRACE)
endif()

add_subdirectory(Dependencies)
add_subdirectory(Core)
add_subdirectory(Plugins)
add_subdirectory(Polyfills)

if(BABYLON_NATIVE_EMBEDDING)
    add_subdirectory(Embedding)
endif()

if(BABYLON_NATIVE_BUILD_APPS)
    add_subdirectory(Apps)
endif()

# no install for glslang compiled for shaderc
if(BABYLON_NATIVE_INSTALL)
    include(Install/Install.cmake)
endif()
