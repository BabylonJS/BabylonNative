# Build only on Win32, not on UWP
if(NOT((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID)))
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(LOCAL_SCRIPTS
    "Scripts/tests.js")

set(NPM_SCRIPTS
    "../Dependencies/node_modules/babylonjs/babylon.max.js"
    "../Dependencies/node_modules/babylonjs/babylon.max.js.map"
    "../Dependencies/node_modules/chai/chai.js"
    "../Dependencies/node_modules/mocha/mocha.js")

set(TEST_APP "Shared/UnitTests.cpp")

set(ADDITIONAL_LIBRARIES "")

if(APPLE)
    find_library(JSCORE_LIBRARY JavaScriptCore)
    set(ADDITIONAL_LIBRARIES ${JSCORE_LIBRARY})
    set(TEST_APP "Apple/UnitTests.mm")
endif()

add_executable(UnitTests ${LOCAL_SCRIPTS} ${NPM_SCRIPTS} ${TEST_APP})

target_link_to_dependencies(UnitTests
    UrlLib
    AppRuntime
    XMLHttpRequest
    NativeEngine
    ScriptLoader
    Console
    Window
    ${ADDITIONAL_LIBRARIES})

add_test(NAME UnitTests COMMAND UnitTests)

foreach(SCRIPT ${LOCAL_SCRIPTS} ${NPM_SCRIPTS})
    get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMENT "Copying ${SCRIPT_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
endforeach()

set_property(TARGET UnitTests PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LOCAL_SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/node_modules PREFIX Scripts FILES ${NPM_SCRIPTS})