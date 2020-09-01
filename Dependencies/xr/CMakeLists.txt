cmake_minimum_required(VERSION 3.12)

set(DYNAMIC_LOADER OFF CACHE BOOL "Build the loader as a .dll library")

set(SOURCES "Include/XR.h"
            "Source/Shared/XRHelpers.h")

if (ANDROID)
    set(SOURCES ${SOURCES}
        "Source/ARCore/XR.cpp")
elseif (IOS)
    set(SOURCES ${SOURCES}
        "Source/ARKit/XR.mm")
else()
    add_subdirectory(Dependencies/OpenXR-SDK)

    set(SOURCES ${SOURCES}
        "Source/OpenXR/XR.cpp"
        "Source/OpenXR/XrPlatformCommon.h"
        "Dependencies/OpenXR-MixedReality/shared/XrUtility/XrToString.h"
        "Dependencies/OpenXR-MixedReality/shared/XrUtility/XrError.h"
        "Dependencies/OpenXR-MixedReality/shared/XrUtility/XrExtensions.h"
        "Dependencies/OpenXR-MixedReality/shared/XrUtility/XrViewConfiguration.h"
        "Dependencies/OpenXR-MixedReality/shared/XrUtility/XrEnumerate.h"
        "Dependencies/OpenXR-MixedReality/shared/XrUtility/XrStruct.h"
        "Dependencies/OpenXR-MixedReality/shared/XrUtility/XrHandle.h"
        "Dependencies/OpenXR-MixedReality/shared/XrUtility/XrMath.h")

    if(WIN32)
        set(SOURCES ${SOURCES} "Source/OpenXR/Windows/XrPlatform.h")
    else()
        message(FATAL_ERROR "OpenXR support not available for platform ${CMAKE_SYSTEM_NAME}")
    endif()
endif()

add_library(xr ${SOURCES})

target_compile_definitions(xr PRIVATE _CRT_SECURE_NO_WARNINGS)

if (ANDROID)
    add_library(arcore SHARED IMPORTED)
    set_target_properties(arcore PROPERTIES IMPORTED_LOCATION
                          ${ARCORE_LIBPATH}/${ANDROID_ABI}/libarcore_sdk_c.so
                          INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/arcore-android-sdk/libraries/include)

    add_library(glm INTERFACE)
    set_target_properties(glm PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/arcore-android-sdk/libraries/glm)

    target_link_libraries(xr PRIVATE glm arcore GSL AndroidExtensions)
elseif (IOS)    
    target_link_libraries(xr PRIVATE "-framework ARKit")
else()
    if(WIN32)
        target_include_directories(xr PRIVATE "Source/OpenXR/Windows")
    endif()

    target_include_directories(xr PRIVATE "Dependencies/OpenXR-SDK/include")
    target_include_directories(xr PRIVATE "Source/OpenXR")

    target_link_libraries(xr PRIVATE openxr_loader)
endif()

target_link_libraries(xr
    PUBLIC arcana)

target_include_directories(xr PUBLIC "Include")
target_include_directories(xr PRIVATE "Source/Shared")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
