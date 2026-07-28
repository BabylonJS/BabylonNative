if((WIN32 AND NOT WINDOWS_STORE) AND GRAPHICS_API STREQUAL D3D11)
    add_subdirectory(HeadlessScreenshotApp)
    add_subdirectory(StyleTransferApp)
endif()

if(NOT ANDROID)
    add_subdirectory(Playground)
endif()

if((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS AND NOT VISIONOS) OR (UNIX AND NOT ANDROID AND NOT APPLE))
    add_subdirectory(ModuleLoadTest)
endif()

# The unit tests build on every platform. Whether they include the Babylon
# Native graphics suites on top of the JsRuntimeHost suites is decided by
# BABYLON_NATIVE_UNIT_TESTS_GRAPHICS. Android drives them through the Gradle
# harness in UnitTests/Android instead of an executable target.
if(NOT ANDROID AND NOT VISIONOS)
    add_subdirectory(UnitTests)
endif()

if((WIN32 AND NOT WINDOWS_STORE AND GRAPHICS_API STREQUAL D3D11) OR (APPLE AND NOT IOS AND NOT VISIONOS))
    if(BABYLON_NATIVE_PLUGIN_SHADERCACHE AND BABYLON_NATIVE_PLUGIN_SHADERTOOL AND NOT BABYLON_NATIVE_PLUGIN_NATIVEENGINE_COMPILESHADERS)
        add_subdirectory(PrecompiledShaderTest)
    endif()
endif()

npm(install)
