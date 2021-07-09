# cmake 3.18+ to have the ARCHIVE_EXTRACT sub-command for files
cmake_minimum_required(VERSION 3.18)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(BabylonNative)

if (APPLE)
    # without this option on azure pipelines, there is a mismatch with math.h giving a lot of undefined functions on macOS.
    # only enabled for Apple as there is no issue for UWP/WIN32
    set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)
endif()

# Setting Platform
if(ANDROID)
    set(BABYLON_NATIVE_PLATFORM "Android")
elseif(APPLE)
    set(BABYLON_NATIVE_PLATFORM "Apple")
elseif(WINDOWS_STORE)
    set(BABYLON_NATIVE_PLATFORM "UWP")
elseif(WIN32)
    set(BABYLON_NATIVE_PLATFORM "Win32")
elseif(UNIX)
    set(BABYLON_NATIVE_PLATFORM "Unix") 
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

#setting Graphics API
if(APPLE)
    set(GRAPHICS_API Metal)
elseif(ANDROID)
    set(GRAPHICS_API OpenGL)
elseif(UNIX)
    set(GRAPHICS_API OpenGL)
elseif(WIN32)
    if(VULKAN)
        set(GRAPHICS_API Vulkan)
    else()
        set(GRAPHICS_API D3D)
    endif()
else()
    message(FATAL_ERROR "Unrecognized platform: graphics API could not be deduced")
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Dependencies EXCLUDE_FROM_ALL)
add_subdirectory(Core EXCLUDE_FROM_ALL)
add_subdirectory(Plugins EXCLUDE_FROM_ALL)
add_subdirectory(Polyfills EXCLUDE_FROM_ALL)

if(DEFINED EXTENSIONS_DIRS)
    foreach(extensionDir IN LISTS EXTENSIONS_DIRS)
        add_subdirectory(${extensionDir} "Extensions" EXCLUDE_FROM_ALL)
    endforeach()
endif()

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    add_subdirectory(Apps EXCLUDE_FROM_ALL)
else()
    add_subdirectory(Apps)
endif()
