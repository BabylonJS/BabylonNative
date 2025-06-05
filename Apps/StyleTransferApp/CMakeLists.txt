set(WIN32_EXECUTABLE TRUE)

set(BABYLON_SCRIPTS
    "../node_modules/babylonjs/babylon.max.js"
    "../node_modules/babylonjs-loaders/babylonjs.loaders.js")

set(SCRIPTS
    "Scripts/index.js")

set(MODELS
    "Models/candy.onnx"
    "Models/la_muse.onnx"
    "Models/mosaic.onnx"
    "Models/udnie.onnx")

set(SOURCES
    "Win32/App.cpp"
    
    "Win32/App.ico"
    "Win32/App.rc"
    "Win32/Resource.h"
    "Win32/small.ico"
    "Win32/targetver.h")

add_executable(StyleTransferApp WIN32 ${BABYLON_SCRIPTS} ${SCRIPTS} ${MODELS} ${SOURCES})

target_compile_definitions(StyleTransferApp
    PRIVATE UNICODE
    PRIVATE _UNICODE)

target_link_libraries(StyleTransferApp
    PRIVATE AppRuntime
    PRIVATE Console
    PRIVATE ExternalTexture
    PRIVATE NativeEngine
    PRIVATE NativeInput
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

foreach(MODEL ${MODELS})
    get_filename_component(MODEL_NAME "${MODEL}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Models/${MODEL_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${MODEL}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Models/${MODEL_NAME}"
        COMMENT "Copying ${MODEL_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${MODEL}")
endforeach()

set_property(TARGET StyleTransferApp PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules PREFIX Scripts FILES ${BABYLON_SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MODELS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
