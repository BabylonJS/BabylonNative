cmake_minimum_required(VERSION 3.18)

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------
FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG        f2757396e80bc4169f2ddb938ce25367a98ffdd0)
FetchContent_Declare(CMakeExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG ea28b7689530bfdc4905806f27ecf7e8ed4b5419)
# --------------------------------------------------

FetchContent_MakeAvailable(CMakeExtensions)

project(AndroidExtensions)

if(NOT ANDROID)
    message(FATAL_ERROR "This project can only build for Android")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_MakeAvailable_With_Message(arcana.cpp)

set(SOURCES
    "Include/AndroidExtensions/Globals.h"
    "Include/AndroidExtensions/JavaWrappers.h"
    "Include/AndroidExtensions/OpenGLHelpers.h"
    "Include/AndroidExtensions/Permissions.h"
    "Include/AndroidExtensions/StdoutLogger.h"
    "Source/Globals.cpp"
    "Source/JavaWrappers.cpp"
    "Source/OpenGLHelpers.cpp"
    "Source/Permissions.cpp"
    "Source/StdoutLogger.cpp")

add_library(AndroidExtensions ${SOURCES})

target_link_libraries(AndroidExtensions
    PUBLIC arcana)

target_include_directories(AndroidExtensions PUBLIC "Include")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
