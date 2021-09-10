# Build only on Win32, not on UWP
if(NOT((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS)))
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(SCRIPTS
    "Scripts/tests.js")

set(BABYLON_SCRIPTS
    "../node_modules/babylonjs/babylon.max.js"
    "../node_modules/babylonjs/babylon.max.js.map")

set(TEST_APP "Shared/UnitTests.cpp")

set(ADDITIONAL_LIBRARIES "")

if(APPLE)
    find_library(JSCORE_LIBRARY JavaScriptCore)
    set(ADDITIONAL_LIBRARIES ${JSCORE_LIBRARY})
    set(TEST_APP "Apple/UnitTests.mm")
endif()

add_executable(UnitTests ${SCRIPTS} ${BABYLON_SCRIPTS} ${TEST_APP})

target_link_libraries(UnitTests
    UrlLib
    AppRuntime
    XMLHttpRequest
    NativeEngine
    ScriptLoader
    Console
    Window
    ${ADDITIONAL_LIBRARIES})

add_test(NAME UnitTests COMMAND UnitTests)

foreach(SCRIPT ${SCRIPTS} ${BABYLON_SCRIPTS})
    get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMENT "Copying ${SCRIPT_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
endforeach()

set_property(TARGET UnitTests PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules PREFIX Scripts FILES ${BABYLON_SCRIPTS})