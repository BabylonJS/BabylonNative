set(SCRIPTS
    "Scripts/experience.js"
    "Scripts/playground_runner.js")

set(BABYLON_SCRIPTS
    "../Dependencies/node_modules/babylonjs-loaders/babylonjs.loaders.js"
    "../Dependencies/node_modules/babylonjs-loaders/babylonjs.loaders.js.map"
    "../Dependencies/node_modules/babylonjs/babylon.max.js"
    "../Dependencies/node_modules/babylonjs/babylon.max.js.map"
    "../Dependencies/node_modules/babylonjs-materials/babylonjs.materials.js"
    "../Dependencies/node_modules/babylonjs-materials/babylonjs.materials.js.map"
    "../Dependencies/node_modules/babylonjs-gui/babylon.gui.js"
    "../Dependencies/node_modules/babylonjs-gui/babylon.gui.js.map")

set(ADDITIONAL_SCRIPTS
    "../Dependencies/Scripts/ammo.js"
    "../Dependencies/Scripts/meshwriter.min.js"
    "../Dependencies/Scripts/recast.js")

set(SOURCES
    "Shared/InputManager.h")

if(APPLE)
    find_library(JSCORE_LIBRARY JavaScriptCore)
    if(IOS)
        set(PLIST_FILE
            "${CMAKE_CURRENT_LIST_DIR}/iOS/Info.plist")
        set(STORYBOARD
            "${CMAKE_CURRENT_LIST_DIR}/iOS/Base.lproj/Main.storyboard"
            "${CMAKE_CURRENT_LIST_DIR}/iOS/Base.lproj/LaunchScreen.storyboard")
        set(RESOURCE_FILES ${STORYBOARD} ${SCRIPTS})
        set(ADDITIONAL_LIBRARIES PRIVATE z NativeXr)
        set(SOURCES
            ${SOURCES}
            "iOS/AppDelegate.swift"
            "iOS/ViewController.swift"
            "iOS/LibNativeBridge.h"
            "iOS/LibNativeBridge.mm")
        set_source_files_properties(${SCRIPTS} ${BABYLON_SCRIPTS} ${ADDITIONAL_SCRIPTS} PROPERTIES MACOSX_PACKAGE_LOCATION "Scripts")
    else()
        set(PLIST_FILE "${CMAKE_CURRENT_LIST_DIR}/macOS/Info.plist")
        set(STORYBOARD "${CMAKE_CURRENT_LIST_DIR}/macOS/Base.lproj/Main.storyboard")
        set(RESOURCE_FILES ${STORYBOARD})
        set(SOURCES
            ${SOURCES}
            "macOS/main.m"
            "macOS/AppDelegate.m"
            "macOS/AppDelegate.h"
            "macOS/ViewController.mm"
            "macOS/ViewController.h")
        set_source_files_properties(${SCRIPTS} ${BABYLON_SCRIPTS} ${ADDITIONAL_SCRIPTS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/Scripts")
    endif()
    set(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES}
        PRIVATE ${JSCORE_LIBRARY}
        PRIVATE NativeCamera)
    set(RESOURCE_FILES ${STORYBOARD})
elseif(UNIX)
    set(SOURCES
        ${SOURCES}
        "X11/App.cpp")
elseif(WINDOWS_STORE)
    set(APPX_FILES "UWP/Package.appxmanifest" "UWP/TemporaryKey.pfx")
    set_property(SOURCE ${APPX_FILES} PROPERTY VS_DEPLOYMENT_CONTENT 1)
    set(APPX_ASSETS
        "UWP/Assets/LockScreenLogo.scale-200.png"
        "UWP/Assets/SplashScreen.scale-200.png"
        "UWP/Assets/Square44x44Logo.scale-200.png"
        "UWP/Assets/Square44x44Logo.targetsize-24_altform-unplated.png"
        "UWP/Assets/Square150x150Logo.scale-200.png"
        "UWP/Assets/StoreLogo.png"
        "UWP/Assets/Wide310x150Logo.scale-200.png")
    set_property(SOURCE ${APPX_ASSETS} PROPERTY VS_DEPLOYMENT_CONTENT 1)
    set_property(SOURCE ${APPX_ASSETS} PROPERTY VS_DEPLOYMENT_LOCATION "Assets")
    set(SOURCES
        ${SOURCES}
        ${APPX_FILES}
        ${APPX_ASSETS}
        "UWP/App.cpp"
        "UWP/App.h")
    set(ADDITIONAL_LIBRARIES PRIVATE NativeXr)
elseif(WIN32)
    set(SOURCES
        ${SOURCES}
        "Win32/App.cpp"
        "Win32/App.h"
        "Win32/App.ico"
        "Win32/App.rc"
        "Win32/Resource.h"
        "Win32/small.ico"
        "Win32/targetver.h")
    set(ADDITIONAL_LIBRARIES PRIVATE NativeXr NativeCamera)
endif()

if(WIN32)
    set(WIN32_EXECUTABLE TRUE)
    add_executable(Playground WIN32 ${SCRIPTS} ${BABYLON_SCRIPTS} ${ADDITIONAL_SCRIPTS} ${SOURCES} ${RESOURCE_FILES})
else()
    add_executable(Playground ${SCRIPTS} ${BABYLON_SCRIPTS} ${ADDITIONAL_SCRIPTS} ${SOURCES} ${RESOURCE_FILES})
endif()

warnings_as_errors(Playground)
target_compile_definitions(Playground PRIVATE UNICODE)
target_compile_definitions(Playground PRIVATE _UNICODE)

if(WINDOWS_STORE)
    target_compile_options(Playground PRIVATE /ZW)
    target_compile_options(Playground PRIVATE /await)
endif()

target_include_directories(Playground PRIVATE "Source" ".")

target_link_to_dependencies(Playground
    PRIVATE AppRuntime
    PRIVATE NativeCapture
    PRIVATE ChromeDevTools
    PRIVATE NativeEngine
    PRIVATE NativeOptimizations
    PRIVATE Console
    PRIVATE Window
    PRIVATE ScriptLoader
    PRIVATE XMLHttpRequest
    PRIVATE Canvas
    ${ADDITIONAL_LIBRARIES}
    ${BABYLON_NATIVE_PLAYGROUND_EXTENSION_LIBRARIES})

if(WIN32)
    target_link_to_dependencies(Playground
        PRIVATE "shlwapi.lib")
endif()

if (UNIX AND NOT APPLE AND NOT ANDROID)
    # Ubuntu mixes old experimental header and new runtime libraries
    # Resulting in crash at runtime for std::filesystem
    # https://stackoverflow.com/questions/56738708/c-stdbad-alloc-on-stdfilesystempath-append
    target_link_libraries(Playground 
        PRIVATE stdc++fs)
endif()

if(APPLE)
    if(IOS)
        set_target_properties(Playground PROPERTIES
            MACOSX_BUNDLE true
            MACOSX_BUNDLE_INFO_PLIST "${PLIST_FILE}"
            XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
            RESOURCE "${RESOURCE_FILES}"

            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 9.0
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.BabylonNative.Playground.iOS"

            XCODE_ATTRIBUTE_SWIFT_VERSION "4.0"
            XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER "${CMAKE_CURRENT_LIST_DIR}/iOS/LibNativeBridge.h"
            XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks"
            XCODE_ATTRIBUTE_ALWAYS_EMBED_SWIFT_STANDARD_LIBRARIES YES

            # CMake seems to add a custom flag "-Wno-unknown-pragmas" to the Swift compiler. That flag is used for Clang,
            # So we need to make sure we override it with nothing here in order to compile Swift.
            XCODE_ATTRIBUTE_OTHER_SWIFT_FLAGS "")

            # Swift support
            set(CMAKE_Swift_COMPILER_FORCED TRUE)
            set(CMAKE_Swift_LANGUAGE_VERSION 4.0)
            enable_language(Swift)
    else()
        target_link_libraries(Playground PUBLIC "-framework MetalKit")

        set_target_properties(Playground PROPERTIES
            MACOSX_BUNDLE true
            MACOSX_BUNDLE_INFO_PLIST "${PLIST_FILE}"
            XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
            RESOURCE "${RESOURCE_FILES}"
            FOLDER "Playground")
    endif()
endif()

if(WINDOWS_STORE)
    set_property(SOURCE ${SCRIPTS} ${BABYLON_SCRIPTS} ${ADDITIONAL_SCRIPTS} PROPERTY VS_DEPLOYMENT_CONTENT 1)
    set_property(SOURCE ${SCRIPTS} ${BABYLON_SCRIPTS} ${ADDITIONAL_SCRIPTS} PROPERTY VS_DEPLOYMENT_LOCATION "Scripts")
else()
    foreach(SCRIPT ${SCRIPTS} ${BABYLON_SCRIPTS} ${ADDITIONAL_SCRIPTS})
        get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
        add_custom_command(
            OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
            COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
            COMMENT "Copying ${SCRIPT_NAME}"
            MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
    endforeach()
endif()

set_property(TARGET Playground PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/node_modules PREFIX Scripts FILES ${BABYLON_SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/Scripts PREFIX Scripts FILES ${ADDITIONAL_SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Playground)
