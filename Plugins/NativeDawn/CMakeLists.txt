set(SOURCES
    "Include/Babylon/Plugins/NativeDawn.h"
    "Source/ImageDecode.cpp"
    "Source/NativeDawn.cpp")

add_library(NativeDawn ${SOURCES})

target_include_directories(NativeDawn
    PUBLIC "Include")

target_link_libraries(NativeDawn
    PUBLIC napi
    PRIVATE JsRuntime
    PRIVATE webgpu_dawn
    PRIVATE bx
    PRIVATE bimg
    PRIVATE bimg_decode
    PRIVATE bimg_encode)

# NativeDawn exposes every WebGPU object as an N-API External wrapper. On the V8
# engine, jsruntimehost's N-API never drains its GC finalizer queue at runtime, so
# those wrappers (and the wgpu handles they hold) accumulate until the process OOMs.
# When building against V8, give the plugin access to the internal env header so it
# can drain the queue itself (see PumpJsFinalizers in NativeDawn.cpp). Other engines
# finalize through their own mechanisms and need none of this.
if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    target_compile_definitions(NativeDawn PRIVATE NATIVEDAWN_V8_FINALIZER_DRAIN)
    target_include_directories(NativeDawn PRIVATE
        $<TARGET_PROPERTY:napi,INCLUDE_DIRECTORIES>
        "${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Source")
endif()

if(WIN32)
    target_compile_definitions(NativeDawn
        PRIVATE NOMINMAX)
endif()

set_property(TARGET NativeDawn PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
