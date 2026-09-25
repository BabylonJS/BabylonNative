set(TEST_BUNDLE "${CMAKE_CURRENT_BINARY_DIR}/Assets/tests.js")
set(SCRIPTS "${TEST_BUNDLE}")

file(GLOB_RECURSE TEST_JAVASCRIPT CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scripts/*.ts")
find_program(NPM_EXECUTABLE NAMES npm npm.cmd REQUIRED)
add_custom_command(
    OUTPUT "${TEST_BUNDLE}"
    COMMAND "${NPM_EXECUTABLE}" run build -- --output-path "${CMAKE_CURRENT_BINARY_DIR}/Assets" --no-output-compare-before-emit
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.."
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scripts/tests.javaScript.all.ts"
    DEPENDS ${TEST_JAVASCRIPT}
        "${CMAKE_CURRENT_SOURCE_DIR}/../webpack.config.js"
        "${CMAKE_CURRENT_SOURCE_DIR}/../babel-plugin-replace-bigint.js"
        "${CMAKE_CURRENT_SOURCE_DIR}/../package.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/../package-lock.json"
    VERBATIM)

set_source_files_properties(${TEST_JAVASCRIPT} PROPERTIES HEADER_FILE_ONLY TRUE)

file(GLOB ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*")

set(SOURCES
    "Source/App.cpp"
    "Source/App.h"
    "Source/Tests.AppRuntime.cpp"
    "Source/Tests.Console.cpp"
    "Source/Tests.DelayedTaskScheduler.cpp"
    "Source/Tests.JavaScript.cpp"
    "Source/Tests.NodeApi.cpp"
    "Source/Tests.Scheduling.cpp"
    "Source/Tests.StandardStreamLogger.cpp"
    "Source/Tests.TimeoutDispatcher.cpp")

if(APPLE)
    if(IOS)
        set(SOURCES ${SOURCES}
            "Source/iOS/App.mm")

        set(PLIST_FILE
            "${CMAKE_CURRENT_LIST_DIR}/Source/iOS/Info.plist")

        set(STORYBOARD
            "${CMAKE_CURRENT_LIST_DIR}/Source/iOS/Base.lproj/LaunchScreen.storyboard"
            "${CMAKE_CURRENT_LIST_DIR}/Source/iOS/Base.lproj/Main.storyboard")

        set_source_files_properties(
            ${SCRIPTS}
            PROPERTIES MACOSX_PACKAGE_LOCATION "Assets")

        set_source_files_properties(
            ${ASSETS}
            PROPERTIES MACOSX_PACKAGE_LOCATION "Assets")
    else()
        set(SOURCES ${SOURCES}
            Source/macOS/App.mm)
    endif()
elseif(WIN32)
    set(SOURCES ${SOURCES}
        Source/Win32/App.cpp)
elseif(UNIX AND NOT ANDROID)
    set(SOURCES ${SOURCES}
        Source/Linux/App.cpp)
endif()

add_executable(UnitTests ${SOURCES} ${TEST_JAVASCRIPT} ${SCRIPTS} ${ASSETS})
if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    target_sources(UnitTests PRIVATE "Source/Tests.V8ForegroundTaskRunner.cpp")
    target_link_libraries(UnitTests PRIVATE AppRuntimeInternal)
endif()
target_compile_definitions(UnitTests PRIVATE JSRUNTIMEHOST_PLATFORM="${JSRUNTIMEHOST_PLATFORM}")
target_compile_definitions(UnitTests PRIVATE NAPI_JAVASCRIPT_ENGINE="${NAPI_JAVASCRIPT_ENGINE}")

# The V8JSI Node-API shim does not implement napi_create_dataview, so the
# CreateDataViewRejectsOverflowingRange test is compiled out on that backend.
if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    target_compile_definitions(UnitTests PRIVATE JSRUNTIMEHOST_NAPI_ENGINE_JSI)
endif()

target_link_libraries(UnitTests
    PRIVATE AppRuntime
    PRIVATE Console
    PRIVATE AbortController
    PRIVATE SchedulingInternal
    PRIVATE ScriptLoader
    PRIVATE URL
    PRIVATE UrlLib
    PRIVATE XMLHttpRequest
    PRIVATE Fetch
    PRIVATE WebSocket
    PRIVATE gtest_main
    PRIVATE FoundationInternal
    PRIVATE Blob
    PRIVATE File
    PRIVATE Performance
    PRIVATE TextDecoder
    PRIVATE TextEncoder
    ${ADDITIONAL_LIBRARIES})

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET UnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:UnitTests>>,copy,true> $<TARGET_RUNTIME_DLLS:UnitTests> $<TARGET_FILE_DIR:UnitTests> COMMAND_EXPAND_LISTS)

if(APPLE)
    enable_objc_arc(UnitTests)
endif()

if(IOS)
    set_target_properties(UnitTests PROPERTIES
        MACOSX_BUNDLE true
        MACOSX_BUNDLE_INFO_PLIST "${PLIST_FILE}"
        RESOURCE "${STORYBOARD}"
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET}
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.jsruntimehost.unittests")
else()
    # Incremental builds may still have resources from the former Scripts directory.
    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E rm -rf "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts")

    if(CMAKE_CONFIGURATION_TYPES)
        add_custom_command(TARGET UnitTests POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:UnitTests>/Assets"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${TEST_BUNDLE}" "$<TARGET_FILE_DIR:UnitTests>/Assets/tests.js")
    endif()

    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Assets/symlink_target.js" "${CMAKE_CFG_INTDIR}/Assets/symlink_1.js")
    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Assets/symlink_1.js" "${CMAKE_CFG_INTDIR}/Assets/symlink_2.js")

    foreach(ASSET ${ASSETS})
        get_filename_component(ASSET_NAME "${ASSET}" NAME)
        add_custom_command(TARGET UnitTests POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${ASSET}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Assets/${ASSET_NAME}")
    endforeach()
endif()

set_property(TARGET UnitTests PROPERTY FOLDER Tests)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${TEST_JAVASCRIPT})
source_group("Assets" FILES ${SCRIPTS} ${ASSETS})
