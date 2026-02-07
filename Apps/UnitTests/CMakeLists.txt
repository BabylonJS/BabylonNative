if(NOT((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID)))
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

option(BABYLON_NATIVE_TESTS_USE_NOOP_METAL_DEVICE "Use a noop Metal device for Apple platforms." OFF)

set(BABYLONJS_ASSETS
    "../node_modules/babylonjs/babylon.max.js")

set(BABYLONJS_MATERIALS_ASSETS
    "../node_modules/babylonjs-materials/babylonjs.materials.js")

set(TEST_ASSETS
    "JavaScript/dist/tests.javaScript.all.js"
    "JavaScript/dist/tests.shaderCache.basicScene.js")

set(SOURCES
    "Source/App.h"
    "Source/App.cpp"
    "Source/Tests.ExternalTexture.cpp"
    "Source/Tests.JavaScript.cpp"
    "Source/Tests.ShaderCache.cpp"
    "Source/Utils.h"
    "Source/Utils.${GRAPHICS_API}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}")

if(GRAPHICS_API STREQUAL "D3D11")
    set(SOURCES ${SOURCES}
        "Source/Tests.Device.${GRAPHICS_API}.cpp"
        "Source/Tests.ExternalTexture.${GRAPHICS_API}.cpp")
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
    set(ADDITIONAL_COMPILE_DEFINITIONS PRIVATE SKIP_EXTERNAL_TEXTURE_TESTS)
elseif(WIN32)
    set(SOURCES ${SOURCES} "Source/App.Win32.cpp")
endif()

add_executable(UnitTests ${BABYLONJS_ASSETS} ${BABYLONJS_MATERIALS_ASSETS} ${TEST_ASSETS} ${SOURCES})
set_property(TARGET UnitTests PROPERTY UNITY_BUILD false)

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
    PRIVATE UrlLib
    PRIVATE Window
    PRIVATE XMLHttpRequest
    PRIVATE gtest_main
    ${ADDITIONAL_LIBRARIES})

target_compile_definitions(UnitTests PRIVATE ${ADDITIONAL_COMPILE_DEFINITIONS})

add_test(NAME UnitTests COMMAND UnitTests)

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET UnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:UnitTests>>,copy,true> $<TARGET_RUNTIME_DLLS:UnitTests> $<TARGET_FILE_DIR:UnitTests> COMMAND_EXPAND_LISTS)

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
