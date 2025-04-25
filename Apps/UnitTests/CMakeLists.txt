# Build only on Win32, not on UWP
if(NOT((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID)))
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(SCRIPTS
    "Scripts/tests.js")

set(EXTERNAL_SCRIPTS
    "../node_modules/babylonjs/babylon.max.js"
    "../node_modules/babylonjs-materials/babylonjs.materials.js"
    "../node_modules/chai/chai.umd.js"
    "../node_modules/mocha/mocha.umd.js")

set(SOURCES
    "Shared/Shared.h"
    "Shared/Shared.cpp")

if(APPLE)
    find_library(JAVASCRIPTCORE_LIBRARY JavaScriptCore)
    set(ADDITIONAL_LIBRARIES PRIVATE ${JAVASCRIPTCORE_LIBRARY})
    set(SOURCES ${SOURCES} "Apple/App.mm")
elseif(UNIX AND NOT ANDROID)
    set(SOURCES ${SOURCES} "X11/App.cpp")
elseif(WIN32)
    set(SOURCES ${SOURCES} "Win32/App.cpp")
endif()

add_executable(UnitTests ${SCRIPTS} ${EXTERNAL_SCRIPTS} ${SOURCES})
set_property(TARGET UnitTests PROPERTY UNITY_BUILD false)

target_link_libraries(UnitTests
    PRIVATE AppRuntime
    PRIVATE Canvas
    PRIVATE Console
    PRIVATE GraphicsDevice
    PRIVATE NativeEngine
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

foreach(SCRIPT ${SCRIPTS} ${EXTERNAL_SCRIPTS})
    get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMENT "Copying ${SCRIPT_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
endforeach()

set_property(TARGET UnitTests PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules PREFIX node_modules FILES ${EXTERNAL_SCRIPTS})
