set(SOURCES
    "Include/Babylon/Runtime.h"
    "Include/Babylon/Runtime${BABYLON_NATIVE_PLATFORM}.h"
    "Include/Babylon/JsRuntime.h"
    "Source/Common.h"
    "Source/Common${BABYLON_NATIVE_PLATFORM}.cpp"
    "Source/Runtime.cpp"
    "Source/RuntimeImpl.cpp"
    "Source/RuntimeImpl${NAPI_JAVASCRIPT_ENGINE}.cpp"
    "Source/RuntimeImpl.h"
    "Source/Runtime${BABYLON_NATIVE_PLATFORM}.cpp")

add_library(BabylonNativeCore ${SOURCES})

set_target_properties(BabylonNativeCore PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${BABYLON_NATIVE_PLATFORM}_${ARCH}")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_include_directories(BabylonNativeCore PRIVATE "Include/Babylon")
target_include_directories(BabylonNativeCore INTERFACE "Include")

target_link_libraries(BabylonNativeCore
    PRIVATE BabylonNativeUtils
    PRIVATE napi)

if (WIN32 OR WINDOWS_STORE)
    target_compile_definitions(BabylonNativeCore PRIVATE NOMINMAX)
endif()
