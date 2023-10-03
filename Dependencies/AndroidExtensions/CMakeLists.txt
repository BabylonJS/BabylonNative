cmake_minimum_required(VERSION 3.18)

project(AndroidExtensions)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Modules declaration
FetchContent_Declare(arcana
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG        f2757396e80bc4169f2ddb938ce25367a98ffdd0)
    
if(NOT ANDROID)
    message(FATAL_ERROR "This project can only build for Android")
endif()

if(NOT TARGET arcana)
    set(${CONTENT_TO_FETCH} arcana)
    message(STATUS "Fetching dependencies for ${PROJECT_NAME} (${CONTENT_TO_FETCH})")
    FetchContent_MakeAvailable(${CONTENT_TO_FETCH})
    message(STATUS "Fetching dependencies for ${PROJECT_NAME} - done")
endif()

set(SOURCES
    "Include/AndroidExtensions/Globals.h"
    "Include/AndroidExtensions/JavaWrappers.h"
    "Include/AndroidExtensions/OpenGLHelpers.h"
    "Include/AndroidExtensions/Permissions.h"
    "Source/Globals.cpp"
    "Source/JavaWrappers.cpp"
    "Source/OpenGLHelpers.cpp"
    "Source/Permissions.cpp")

add_library(AndroidExtensions ${SOURCES})

target_link_libraries(AndroidExtensions
    PUBLIC arcana)

target_include_directories(AndroidExtensions PUBLIC "Include")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
