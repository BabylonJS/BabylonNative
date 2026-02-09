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
# glslang
# --------------------------------------------------
set(BUILD_EXTERNAL OFF)
set(ENABLE_SPVREMAPPER OFF)
set(ENABLE_GLSLANG_BINARIES OFF)
set(ENABLE_HLSL OFF)
set(ENABLE_OPT OFF)
set(ENABLE_CTEST OFF)
set(SKIP_GLSLANG_INSTALL ON)
set(ENABLE_GLSLANG_WEBMIN ON)
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
# JsRuntimeHost
# --------------------------------------------------
FetchContent_MakeAvailable_With_Message(JsRuntimeHost)

# UrlLib enables ARC only through an Xcode target attribute in its own CMake.
# Add explicit ARC flags so Unix Makefiles/Ninja builds on Apple also work.
if(APPLE AND TARGET UrlLib)
    target_compile_options(UrlLib PRIVATE -fobjc-arc)
endif()

set_property(TARGET JsRuntime PROPERTY UNITY_BUILD false)
if(TARGET AppRuntime)
    set_property(TARGET AppRuntime PROPERTY UNITY_BUILD false)
endif()

if(WIN32)
    # UrlLib does not support unity build on win32
    set_property(TARGET UrlLib PROPERTY UNITY_BUILD false)
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
set(SPIRV_CROSS_ENABLE_WEBMIN ON)
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
