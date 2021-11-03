# Build only on Win32, not on UWP
if(NOT((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID)))
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(LOCAL_SCRIPTS
    "Scripts/tests.js")

set(NPM_SCRIPTS
    "../node_modules/babylonjs/babylon.max.js"
    "../node_modules/babylonjs/babylon.max.js.map"
    "../node_modules/babylonjs-materials/babylonjs.materials.js"
    "../node_modules/babylonjs-materials/babylonjs.materials.js.map"
    "../node_modules/chai/chai.js"
    "../node_modules/mocha/mocha.js")

set(ADDITIONAL_LIBRARIES "")

if(APPLE)
    find_library(JSCORE_LIBRARY JavaScriptCore)
    set(ADDITIONAL_LIBRARIES ${JSCORE_LIBRARY})
    set(TEST_APP "Apple/App.mm")
elseif(UNIX AND NOT ANDROID)
    set(TEST_APP "X11/App.cpp")
elseif(WIN32)
    set(TEST_APP "Win32/App.cpp")
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
    Canvas
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
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules PREFIX Scripts FILES ${NPM_SCRIPTS})