project(BabylonNative)

set(FRAMEWORK_NAME BabylonNativeMacOS)

set(RESOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../TestApp")
set(SCRIPTS "${RESOURCE_DIR}/Scripts/babylon.glTF2FileLoader.js"
    "${RESOURCE_DIR}/Scripts/babylon.max.js"
    "${RESOURCE_DIR}/Scripts/experience.js"
    "${RESOURCE_DIR}/Scripts/playground_runner.js"
    "${RESOURCE_DIR}/Scripts/babylonjs.materials.js")

add_library(${FRAMEWORK_NAME} "BabylonViewController.mm" ${SCRIPTS})

target_compile_definitions(${FRAMEWORK_NAME} PRIVATE HAVE_INSPECTOR=0)
target_compile_definitions(${FRAMEWORK_NAME} PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
target_compile_definitions(${FRAMEWORK_NAME} PRIVATE NODE_ADDON_API_DISABLE_NODE_SPECIFIC)
target_compile_definitions(${FRAMEWORK_NAME} PRIVATE NODE_ADDON_API_DISABLE_DEPRECATED)

target_include_directories(${FRAMEWORK_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../Library/Include"
    "${CMAKE_CURRENT_LIST_DIR}/../../Library/Dependencies/napi/include"
    "${CMAKE_CURRENT_LIST_DIR}/../../TestApp/Source")

add_dependencies(${FRAMEWORK_NAME} Library)

add_custom_command(
    TARGET ${FRAMEWORK_NAME}
    POST_BUILD
    COMMAND ${CMAKE_AR} -rcT $<TARGET_FILE:${FRAMEWORK_NAME}>
        $<TARGET_FILE:Library>
        $<TARGET_FILE:bx>
        $<TARGET_FILE:bimg>
        $<TARGET_FILE:bgfx>
        $<TARGET_FILE:libcurl>
        $<TARGET_FILE:SPIRV>
        $<TARGET_FILE:spirv-cross-msl>
        $<TARGET_FILE:spirv-cross-glsl>
        $<TARGET_FILE:spirv-cross-core>
        $<TARGET_FILE:napi>
        $<TARGET_FILE:astc>
        $<TARGET_FILE:astc-codec>
        $<TARGET_FILE:glslang>
        $<TARGET_FILE:OSDependent>
        $<TARGET_FILE:OGLCompiler>
    COMMENT "Combining libs..."
)

set_target_properties(${FRAMEWORK_NAME} PROPERTIES
  FRAMEWORK TRUE
  FRAMEWORK_VERSION C
  MACOSX_FRAMEWORK_IDENTIFIER com.babylon.native
  #MACOSX_FRAMEWORK_INFO_PLIST Info.plist
  PUBLIC_HEADER BabylonViewController.h
  #XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "MacOS Developer"
  RESOURCE "${SCRIPTS}"
)