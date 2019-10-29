# cmake 3.15.3+ because of a c++ regression when swift is enable
cmake_minimum_required(VERSION 3.15.3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(BabylonNative)

if (APPLE)
    # without this option on azure pipelines, there is a mismatch with math.h giving a lot of undefined functions on macOS.
    # only enabled for Apple as there is no issue for UWP/WIN32
    set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)
endif()

set(SUPPORTED_PLATFORMS "Android" "Apple" "UWP" "Win32")
if(ANDROID)
    set(BABYLON_NATIVE_PLATFORM "Android" CACHE STRING "Target platform for Babylon Native.")
elseif(APPLE)
    set(BABYLON_NATIVE_PLATFORM "Apple" CACHE STRING "Target platform for Babylon Native.")
elseif(WINDOWS_STORE)
    set(BABYLON_NATIVE_PLATFORM "UWP" CACHE STRING "Target platform for Babylon Native.")
elseif(WIN32)
    set(BABYLON_NATIVE_PLATFORM "Win32" CACHE STRING "Target platform for Babylon Native.")
endif()

if(NOT(BABYLON_NATIVE_PLATFORM IN_LIST SUPPORTED_PLATFORMS))
    message(FATAL_ERROR "Unrecognized platform: ${BABYLON_NATIVE_PLATFORM}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Library)
if(NOT ANDROID)
add_subdirectory(TestApp)
endif()