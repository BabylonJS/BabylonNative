if((NOT UNIX AND NOT WIN32 AND NOT APPLE) OR WINDOWS_STORE OR (APPLE AND IOS) OR ANDROID)
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(BABYLONSCRIPTS 
    "../BabylonScripts/babylon.glTF2FileLoader.js"
    "../BabylonScripts/babylon.max.js"
    "../BabylonScripts/babylonjs.materials.js"
    "../BabylonScripts/babylon.gui.js")

set(SCRIPTS
    "Scripts/validation_native.js"
    "Scripts/config.json")

if(WIN32)
    set(SOURCES
        "Shared/TestUtils.h"
        "Win32/App.cpp"
        "Win32/App.h"
        "Win32/App.ico"
        "Win32/App.rc"
        "Win32/Resource.h"
        "Win32/small.ico"
        "Win32/targetver.h")

    set(WIN32_EXECUTABLE TRUE)
    add_executable(ValidationTests WIN32 ${BABYLONSCRIPTS} ${SCRIPTS} ${SOURCES})
elseif(APPLE)
        find_library(JSCORE_LIBRARY JavaScriptCore)
        set(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} PRIVATE ${JSCORE_LIBRARY})
        set(PLIST_FILE "${CMAKE_CURRENT_LIST_DIR}/macOS/Info.plist")
        set(STORYBOARD "${CMAKE_CURRENT_LIST_DIR}/macOS/Base.lproj/Main.storyboard")
        set(JSSCRIPT "${CMAKE_CURRENT_LIST_DIR}/Scripts/config.json"
            "${CMAKE_CURRENT_LIST_DIR}/Scripts/validation_native.js")
        set(RESOURCE_FILES ${STORYBOARD} ${JSSCRIPT} ${BABYLONSCRIPTS})
        set(SOURCES
            ${SOURCES}
            "macOS/main.m"
            "macOS/AppDelegate.m"
            "macOS/AppDelegate.h"
            "macOS/ViewController.mm"
            "macOS/ViewController.h"
            "Shared/TestUtils.h")
        add_executable(ValidationTests ${BABYLONSCRIPTS} ${SCRIPTS} ${SOURCES} ${RESOURCE_FILES})
elseif(UNIX)
    set(SOURCES
        ${SOURCES}
        "X11/App.cpp")
    add_executable(ValidationTests ${BABYLONSCRIPTS} ${SCRIPTS} ${SOURCES})
endif()

warnings_as_errors(ValidationTests)
target_compile_definitions(ValidationTests PRIVATE UNICODE)
target_compile_definitions(ValidationTests PRIVATE _UNICODE)

target_include_directories(ValidationTests PRIVATE "Source" ".")

if (UNIX AND NOT APPLE AND NOT ANDROID)
    # Ubuntu mixes old experimental header and new runtime libraries
    # Resulting in crash at runtime for std::filesystem
    # https://stackoverflow.com/questions/56738708/c-stdbad-alloc-on-stdfilesystempath-append
    target_link_libraries(ValidationTests 
        PRIVATE stdc++fs)
endif()

target_link_to_dependencies(ValidationTests
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE AppRuntime
    PRIVATE NativeEngine
    PRIVATE Console
    PRIVATE Window
    PRIVATE ScriptLoader
    ${ADDITIONAL_LIBRARIES}
    PRIVATE XMLHttpRequest)

if(APPLE AND NOT IOS)
    target_link_libraries(ValidationTests PUBLIC "-framework MetalKit")
    set_target_properties(ValidationTests PROPERTIES
        MACOSX_BUNDLE true
        MACOSX_BUNDLE_INFO_PLIST "${PLIST_FILE}"
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        RESOURCE "${RESOURCE_FILES}"
        FOLDER "ValidationTests")
endif()

foreach(script ${SCRIPTS} ${BABYLONSCRIPTS})
    get_filename_component(SCRIPT_NAME "${script}" NAME)
    # Copy scripts to the parent of the executable location since CMake can't use generator
    # expressions with OUTPUT. See https://gitlab.kitware.com/cmake/cmake/-/issues/12877.
    add_custom_command(
        OUTPUT "Scripts/${SCRIPT_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${script}" "${CMAKE_CURRENT_BINARY_DIR}/Scripts/${SCRIPT_NAME}"
        COMMENT "Copying ${SCRIPT_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${script}")
endforeach()

set_property(TARGET ValidationTests PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../BabylonScripts PREFIX Scripts FILES ${BABYLONSCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
