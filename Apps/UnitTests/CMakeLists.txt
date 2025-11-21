# Build only on Win32, not on UWP
if(NOT((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID)))
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(BABYLONJS_ASSETS
    "../node_modules/babylonjs/babylon.max.js")

set(BABYLONJS_MATERIALS_ASSETS
    "../node_modules/babylonjs-materials/babylonjs.materials.js")

set(TEST_ASSETS
    "JavaScript/dist/tests.javaScript.all.js"
    "JavaScript/dist/tests.nativeEngine.shaderCache.js")

set(SOURCES
    "Shared/Tests.h"
    "Shared/Tests.cpp"
    "Shared/Tests.JavaScript.cpp"
    "Shared/Tests.NativeEngine.cpp")

if(APPLE)
    find_library(JAVASCRIPTCORE_LIBRARY JavaScriptCore)
    set(ADDITIONAL_LIBRARIES PRIVATE ${JAVASCRIPTCORE_LIBRARY})
    set(SOURCES ${SOURCES} "Apple/App.mm")
elseif(UNIX AND NOT ANDROID)
    set(SOURCES ${SOURCES} "X11/App.cpp")
elseif(WIN32)
    set(SOURCES ${SOURCES} "Win32/App.cpp")
endif()

add_executable(UnitTests ${BABYLONJS_ASSETS} ${BABYLONJS_MATERIALS_ASSETS} ${TEST_ASSETS} ${SOURCES})
set_property(TARGET UnitTests PROPERTY UNITY_BUILD false)

target_link_libraries(UnitTests
    PRIVATE AppRuntime
    PRIVATE Blob
    PRIVATE Canvas
    PRIVATE Console
    PRIVATE GraphicsDevice
    PRIVATE NativeEngine
    PRIVATE DataStream
    PRIVATE NativeEncoding
    PRIVATE ScriptLoader
    PRIVATE UrlLib
    PRIVATE Window
    PRIVATE XMLHttpRequest
    PRIVATE gtest_main
    ${ADDITIONAL_LIBRARIES})

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
