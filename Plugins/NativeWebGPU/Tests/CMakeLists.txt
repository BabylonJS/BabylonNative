if(NOT TARGET gtest_main)
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(googletest)
endif()

set(SOURCES
    "Source/Tests.AsyncBridge.cpp")

add_executable(NativeWebGPUAsyncTests ${SOURCES})
set_property(TARGET NativeWebGPUAsyncTests PROPERTY UNITY_BUILD false)
warnings_as_errors(NativeWebGPUAsyncTests)

target_link_libraries(NativeWebGPUAsyncTests
    PRIVATE AppRuntime
    PRIVATE ScriptLoader
    PRIVATE Window
    PRIVATE NativeWebGPU
    PRIVATE gtest_main)

if(APPLE)
    find_library(APPKIT_FRAMEWORK AppKit)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(METAL_FRAMEWORK Metal)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore)

    target_link_libraries(NativeWebGPUAsyncTests
        PRIVATE ${APPKIT_FRAMEWORK}
        PRIVATE ${COREFOUNDATION_FRAMEWORK}
        PRIVATE ${FOUNDATION_FRAMEWORK}
        PRIVATE ${METAL_FRAMEWORK}
        PRIVATE ${QUARTZCORE_FRAMEWORK})
endif()

set_property(TARGET NativeWebGPUAsyncTests PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
