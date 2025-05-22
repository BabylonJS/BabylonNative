FetchContent_Declare(DirectXTK
    GIT_REPOSITORY https://github.com/microsoft/DirectXTK.git
    GIT_TAG e7d9b64ca6bb8977d4e3006727deaf57a7c1de84)

FetchContent_MakeAvailable(CMakeExtensions)

set(BUILD_TESTING OFF)
set(BUILD_TOOLS OFF)
set(BUILD_XAUDIO_WIN8 OFF)
set(DIRECTX_ARCH ${CMAKE_VS_PLATFORM_NAME})

FetchContent_MakeAvailable_With_Message(DirectXTK)

set_property(TARGET DirectXTK PROPERTY UNITY_BUILD false)
set_property(TARGET DirectXTK PROPERTY FOLDER Apps/Dependencies)

set(BABYLON_SCRIPTS
    "../node_modules/babylonjs/babylon.max.js"
    "../node_modules/babylonjs-loaders/babylonjs.loaders.js")

set(SCRIPTS
    "Scripts/index.js")

set(SOURCES
    "Win32/RenderDoc.h"
    "Win32/RenderDoc.cpp"
    "Win32/App.cpp")

add_executable(HeadlessScreenshotApp ${BABYLON_SCRIPTS} ${SCRIPTS} ${SOURCES})

target_compile_definitions(HeadlessScreenshotApp
    PRIVATE UNICODE
    PRIVATE _UNICODE)

target_link_libraries(HeadlessScreenshotApp
    PRIVATE AppRuntime
    PRIVATE Console
    PRIVATE DirectXTK
    PRIVATE ExternalTexture
    PRIVATE NativeEngine
    PRIVATE ScriptLoader
    PRIVATE Window
    PRIVATE XMLHttpRequest)

foreach(SCRIPT ${BABYLON_SCRIPTS} ${SCRIPTS})
    get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMENT "Copying ${SCRIPT_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
endforeach()

set_property(TARGET HeadlessScreenshotApp PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules PREFIX Scripts FILES ${BABYLON_SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
