set(SOURCES
    "Include/Babylon/Plugins/NativeWebGPU.h"
    "Source/NativeWebGPU.cpp")

add_library(NativeWebGPU STATIC ${SOURCES})
warnings_as_errors(NativeWebGPU)

# NativeWebGPU provides the JS-facing `navigator.gpu` object surface and routes
# commands/interop helpers through the shared GraphicsWgpu Rust ABI.
target_include_directories(NativeWebGPU
    PUBLIC "Include"
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../Core/GraphicsWgpu/InternalInclude")

target_link_libraries(NativeWebGPU
    PUBLIC napi
    PRIVATE JsRuntimeInternal)

if(TARGET babylon_graphics_backend)
    target_link_libraries(NativeWebGPU PRIVATE babylon_graphics_backend)
endif()

if(BABYLON_NATIVE_ENABLE_WEBGPU_DEVELOPER_FEATURES)
    target_compile_definitions(NativeWebGPU PRIVATE BABYLON_NATIVE_ENABLE_WEBGPU_DEVELOPER_FEATURES=1)
endif()

if(BABYLON_NATIVE_ENABLE_UNSAFE_WEBGPU)
    target_compile_definitions(NativeWebGPU PRIVATE BABYLON_NATIVE_ENABLE_UNSAFE_WEBGPU=1)
endif()

set_property(TARGET NativeWebGPU PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

if(BABYLON_NATIVE_BUILD_WEBGPU_TESTS AND ((APPLE AND NOT IOS AND NOT VISIONOS) OR (UNIX AND NOT ANDROID) OR (WIN32 AND NOT WINDOWS_STORE)))
    target_compile_definitions(NativeWebGPU PRIVATE BABYLON_NATIVE_WEBGPU_TEST_HOOKS=1)
    add_subdirectory(Tests)
endif()
