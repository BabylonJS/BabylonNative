set(BABYLON_SCRIPTS
    "../node_modules/babylonjs-loaders/babylonjs.loaders.js"
    "../node_modules/babylonjs-loaders/babylonjs.loaders.js.map"
    "../node_modules/babylonjs/babylon.max.js"
    "../node_modules/babylonjs/babylon.max.js.map"
    "../node_modules/babylonjs-materials/babylonjs.materials.js"
    "../node_modules/babylonjs-materials/babylonjs.materials.js.map"
    "../node_modules/babylonjs-gui/babylon.gui.js"
    "../node_modules/babylonjs-gui/babylon.gui.js.map")

set(DEPENDENCIES
    "../Dependencies/ammo.js"
    "../Dependencies/meshwriter.min.js"
    "../Dependencies/recast.js")

set(SCRIPTS
    "Scripts/experience.js"
    "Scripts/playground_runner.js"
    "Scripts/slideShow_experience.js")

set(RESOUCES
    "Resources/external.png")

set(SOURCES
    ${SOURCES}
    "App.cpp"
    "App.h"
    "Utility.h"
    "App.ico"
    "App.rc"
    "Resource.h"
    "small.ico"
    "targetver.h"
    "stb_image.h")

set(ADDITIONAL_LIBRARIES PRIVATE NativeXr NativeCamera)

set(WIN32_EXECUTABLE TRUE)
add_executable(NativeTextureApp WIN32 ${BABYLON_SCRIPTS} ${DEPENDENCIES} ${SCRIPTS} ${SOURCES} ${RESOURCE_FILES} ${RESOUCES})

warnings_as_errors(NativeTextureApp)
target_compile_definitions(NativeTextureApp PRIVATE UNICODE)
target_compile_definitions(NativeTextureApp PRIVATE _UNICODE)

if(WINDOWS_STORE)
    target_compile_options(NativeTextureApp PRIVATE /ZW)
    target_compile_options(NativeTextureApp PRIVATE /await)
endif()

target_include_directories(NativeTextureApp PRIVATE "Source" ".")

target_link_to_dependencies(NativeTextureApp
    PRIVATE AppRuntime
    PRIVATE NativeCapture
    PRIVATE ChromeDevTools
    PRIVATE NativeEngine
    PRIVATE NativeInput
    PRIVATE NativeOptimizations
    PRIVATE Console
    PRIVATE Window
    PRIVATE ScriptLoader
    PRIVATE XMLHttpRequest
    PRIVATE Canvas
    PRIVATE GraphicsInterops
    ${ADDITIONAL_LIBRARIES}
    ${BABYLON_NATIVE_PLAYGROUND_EXTENSION_LIBRARIES})

if(WIN32)
    target_link_to_dependencies(NativeTextureApp
        PRIVATE "shlwapi.lib")
endif()

if (UNIX AND NOT APPLE AND NOT ANDROID)
    # Ubuntu mixes old experimental header and new runtime libraries
    # Resulting in crash at runtime for std::filesystem
    # https://stackoverflow.com/questions/56738708/c-stdbad-alloc-on-stdfilesystempath-append
    target_link_libraries(NativeTextureApp 
        PRIVATE stdc++fs)
endif()

foreach(SCRIPT ${SCRIPTS} ${BABYLON_SCRIPTS} ${DEPENDENCIES})
    get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMENT "Copying ${SCRIPT_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
endforeach()

foreach(RESOURCE_FILE ${RESOUCES})
    get_filename_component(RESOURCE_FILE_NAME "${RESOURCE_FILE}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Resources/${RESOURCE_FILE_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Resources/${RESOURCE_FILE_NAME}"
        COMMENT "Copying ${RESOURCE_FILE_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_FILE}")
endforeach()

set_property(TARGET NativeTextureApp PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules PREFIX Scripts FILES ${BABYLON_SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies PREFIX Scripts FILES ${DEPENDENCIES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RESOUCES})
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NativeTextureApp)
