if(NOT((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID)))
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

option(BABYLON_NATIVE_TESTS_USE_NOOP_METAL_DEVICE "Use a noop Metal device for Apple platforms." OFF)

set(BABYLONJS_ASSETS
    "../node_modules/babylonjs/babylon.max.js")

set(BABYLONJS_MATERIALS_ASSETS
    "../node_modules/babylonjs-materials/babylonjs.materials.js")

set(TEST_ASSETS
    "JavaScript/dist/tests.externalTexture.deviceLoss.js"
    "JavaScript/dist/tests.externalTexture.msaa.js"
    "JavaScript/dist/tests.externalTexture.render.js"
    "JavaScript/dist/tests.javaScript.all.js"
    "JavaScript/dist/tests.shaderCache.basicScene.js"
    "JavaScript/dist/tests.shaderCompilation.comprehensiveGLSL.js")

file(GLOB TEST_SCRIPTS "JavaScript/src/*.ts")
set_source_files_properties(${TEST_SCRIPTS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(SOURCES
    "Source/App.h"
    "Source/App.cpp"
    "Source/Tests.ExternalTexture.cpp"
    "Source/Tests.ExternalTexture.DeviceLoss.cpp"
    "Source/Tests.ExternalTexture.Msaa.cpp"
    "Source/Tests.ExternalTexture.Render.cpp"
    "Source/Tests.JavaScript.cpp"
    "Source/Tests.NativeEngine.Teardown.cpp"
    "Source/Tests.ShaderCache.cpp"
    "Source/Tests.ShaderCompilation.cpp"
    "Source/Tests.UniformPadding.cpp"
    "Source/Helpers.h"
    "Source/Helpers.${GRAPHICS_API}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}")

if(GRAPHICS_API STREQUAL "D3D11" OR GRAPHICS_API STREQUAL "D3D12")
    set(SOURCES ${SOURCES}
        "Source/Tests.Device.cpp")
endif()

if(GRAPHICS_API STREQUAL "D3D11")
    set(SOURCES ${SOURCES}
        "Source/Tests.Device.${GRAPHICS_API}.cpp")
endif()

if(APPLE)
    set(SOURCES ${SOURCES} "Source/App.Apple.mm")
    if(BABYLON_NATIVE_TESTS_USE_NOOP_METAL_DEVICE)
        set(ADDITIONAL_COMPILE_DEFINITIONS
            PRIVATE USE_NOOP_METAL_DEVICE
            PRIVATE SKIP_EXTERNAL_TEXTURE_TESTS)
    endif()
    find_library(JAVASCRIPTCORE_LIBRARY JavaScriptCore)
    set(ADDITIONAL_LIBRARIES PRIVATE ${JAVASCRIPTCORE_LIBRARY})
elseif(UNIX AND NOT ANDROID)
    set(SOURCES ${SOURCES} "Source/App.X11.cpp")
elseif(WIN32)
    set(SOURCES ${SOURCES}
        "Source/App.Win32.cpp"
        "Source/RenderDoc.h"
        "Source/RenderDoc.cpp")
    set(ADDITIONAL_COMPILE_DEFINITIONS ${ADDITIONAL_COMPILE_DEFINITIONS} PRIVATE HAS_RENDERDOC)
endif()

add_executable(UnitTests ${BABYLONJS_ASSETS} ${BABYLONJS_MATERIALS_ASSETS} ${TEST_ASSETS} ${TEST_SCRIPTS} ${SOURCES})
target_link_libraries(UnitTests
    PRIVATE AppRuntime
    PRIVATE Blob
    PRIVATE Canvas
    PRIVATE Console
    PRIVATE GraphicsDevice
    PRIVATE ExternalTexture
    PRIVATE NativeEngine
    PRIVATE NativeEncoding
    PRIVATE ScriptLoader
    PRIVATE ShaderCache
    PRIVATE Window
    PRIVATE XMLHttpRequest
    PRIVATE gtest_main
    ${ADDITIONAL_LIBRARIES})

target_compile_definitions(UnitTests PRIVATE ${ADDITIONAL_COMPILE_DEFINITIONS})

# NativeDraco and NativeMeshopt default to OFF, so link and exercise them only when the
# consuming build opted in. CI turns both on for the jobs that run UnitTests.
if(BABYLON_NATIVE_PLUGIN_NATIVEDRACO)
    target_link_libraries(UnitTests PRIVATE NativeDraco)
    target_compile_definitions(UnitTests PRIVATE HAS_NATIVE_DRACO)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEMESHOPT)
    target_link_libraries(UnitTests PRIVATE NativeMeshopt)
    target_compile_definitions(UnitTests PRIVATE HAS_NATIVE_MESHOPT)
endif()

if(GRAPHICS_API STREQUAL "D3D12")
    target_compile_definitions(UnitTests PRIVATE SKIP_RENDER_TESTS)
endif()

if(GRAPHICS_API STREQUAL "OpenGL")
    # Multisample: bgfx backs an MSAA render target with a renderbuffer it allocates itself,
    # so an imported texture never receives the resolved pixels. Rendering into the caller's
    # texture instead needs GL_TEXTURE_2D_MULTISAMPLE, which is ES 3.1, while Babylon Native
    # pins bgfx to ES 3.0 (BGFX_CONFIG_RENDERER_OPENGLES=30).
    # See https://github.com/BabylonJS/BabylonNative/issues/1809.
    #
    # Device loss: the test creates textures on a device handle while bgfx is torn down. On
    # OpenGL the "device" is a context bgfx created, and a bare context carries no display or
    # surface, so Helpers::CreateDevice cannot make one current to allocate against.
    # See https://github.com/BabylonJS/BabylonNative/issues/1810.
    target_compile_definitions(UnitTests PRIVATE
        SKIP_MULTISAMPLE_TESTS
        SKIP_DEVICE_LOSS_TESTS)
endif()

add_test(NAME UnitTests COMMAND UnitTests)

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET UnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:UnitTests>>,copy,true> $<TARGET_RUNTIME_DLLS:UnitTests> $<TARGET_FILE_DIR:UnitTests> COMMAND_EXPAND_LISTS)

if(ANGLE_LIBEGL)
    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ANGLE_LIBEGL}" "$<TARGET_FILE_DIR:UnitTests>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ANGLE_LIBGLESV2}" "$<TARGET_FILE_DIR:UnitTests>"
        COMMENT "Copying ANGLE libraries to UnitTests output directory"
    )
endif()

foreach(ASSET ${BABYLONJS_ASSETS} ${BABYLONJS_MATERIALS_ASSETS} ${TEST_ASSETS})
    get_filename_component(ASSET_NAME "${ASSET}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Assets/${ASSET_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${ASSET}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Assets/${ASSET_NAME}"
        COMMENT "Copying ${ASSET_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${ASSET}")
endforeach()

set_property(TARGET UnitTests PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules/babylonjs PREFIX Assets FILES ${BABYLONJS_ASSETS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules/babylonjs-materials PREFIX Assets FILES ${BABYLONJS_MATERIALS_ASSETS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/JavaScript/dist PREFIX Assets FILES ${TEST_ASSETS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/JavaScript/src PREFIX TypeScript FILES ${TEST_SCRIPTS})
