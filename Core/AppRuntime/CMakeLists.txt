set(SOURCES
    "Include/Babylon/AppRuntime.h"
    "Source/AppRuntime.cpp"
    "Source/AppRuntime${NAPI_JAVASCRIPT_ENGINE}.cpp"
    "Source/AppRuntime${BABYLON_NATIVE_PLATFORM}.cpp"
    "Source/WorkQueue.cpp"
    "Source/WorkQueue.h")

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    set(SOURCES ${SOURCES}
        "Source/ChakraJSI/Common/unicode.cpp"
        "Source/ChakraJSI/Common/unicode.h"
        "Source/ChakraJSI/Common/utilities.h"
        "Source/ChakraJSI/JSI/Shared/ByteArrayBuffer.h"
        "Source/ChakraJSI/JSI/Shared/ChakraObjectRef.cpp"
        "Source/ChakraJSI/JSI/Shared/ChakraObjectRef.h"
        "Source/ChakraJSI/JSI/Shared/ChakraRuntime.cpp"
        "Source/ChakraJSI/JSI/Shared/ChakraRuntime.h"
        "Source/ChakraJSI/JSI/Shared/ChakraRuntimeArgs.h"
        "Source/ChakraJSI/JSI/Shared/ChakraRuntimeFactory.h"
        "Source/ChakraJSI/JSI/Shared/RuntimeHolder.h"
        "Source/ChakraJSI/JSI/Shared/ScriptStore.h")
endif()

add_library(AppRuntime ${SOURCES})

target_include_directories(AppRuntime
    PRIVATE "Include/Babylon"
    INTERFACE "Include")

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    target_include_directories(AppRuntime
        PRIVATE "Source/ChakraJSI/Common"
        PRIVATE "Source/ChakraJSI/JSI/Shared")

    target_compile_definitions(napi PUBLIC USE_EDGEMODE_JSRT)

    target_link_to_dependencies(AppRuntime
        INTERFACE "chakrart.lib")
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID)
    target_include_directories(AppRuntime INTERFACE "/usr/include/webkitgtk-4.0/")
endif()

target_link_to_dependencies(AppRuntime
    PRIVATE arcana
    PUBLIC JsRuntime)

target_compile_definitions(AppRuntime
    PRIVATE NOMINMAX)

set_property(TARGET AppRuntime PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
