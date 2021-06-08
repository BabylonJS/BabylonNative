if((NOT UNIX AND NOT WIN32 AND NOT APPLE) OR WINDOWS_STORE OR ANDROID)
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

FILE(GLOB REFERENCE_IMAGES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "ReferenceImages/*")

set(BABYLON_SCRIPTS
    "../node_modules/babylonjs-loaders/babylonjs.loaders.js"
    "../node_modules/babylonjs/babylon.max.js"
    "../node_modules/babylonjs-materials/babylonjs.materials.js"
    "../node_modules/babylonjs-gui/babylon.gui.js")

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
    set(ADDITIONAL_LIBRARIES PRIVATE NativeXr)
    set(WIN32_EXECUTABLE TRUE)
    add_executable(ValidationTests WIN32 ${REFERENCE_IMAGES} ${BABYLON_SCRIPTS} ${SCRIPTS} ${SOURCES})
elseif(APPLE)
    find_library(JSCORE_LIBRARY JavaScriptCore)
    if(IOS)
        set(PLIST_FILE "${CMAKE_CURRENT_LIST_DIR}/iOS/Info.plist")
        set(STORYBOARD
            "${CMAKE_CURRENT_LIST_DIR}/iOS/Base.lproj/Main.storyboard"
            "${CMAKE_CURRENT_LIST_DIR}/iOS/Base.lproj/LaunchScreen.storyboard")
        set(ADDITIONAL_LIBRARIES PRIVATE z NativeXr)
        set(SOURCES
            ${SOURCES}
            "iOS/AppDelegate.swift"
            "iOS/ViewController.swift"
            "iOS/LibNativeBridge.h"
            "iOS/LibNativeBridge.mm"
            "Shared/TestUtils.h")
        set_source_files_properties(${SCRIPTS} ${BABYLON_SCRIPTS} PROPERTIES MACOSX_PACKAGE_LOCATION "Scripts")
        set_source_files_properties(${REFERENCE_IMAGES} PROPERTIES MACOSX_PACKAGE_LOCATION "ReferenceImages")
    else()
        set(PLIST_FILE "${CMAKE_CURRENT_LIST_DIR}/macOS/Info.plist")
        set(STORYBOARD "${CMAKE_CURRENT_LIST_DIR}/macOS/Base.lproj/Main.storyboard")
        set(SOURCES
            ${SOURCES}
            "macOS/main.m"
            "macOS/AppDelegate.mm"
            "macOS/AppDelegate.h"
            "macOS/ViewController.mm"
            "macOS/ViewController.h"
            "Shared/TestUtils.h")
        set_source_files_properties(${SCRIPTS} ${BABYLON_SCRIPTS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/Scripts")
        set_source_files_properties(${REFERENCE_IMAGES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/ReferenceImages")
     endif()
     set(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} PRIVATE ${JSCORE_LIBRARY})
     set(RESOURCE_FILES ${STORYBOARD})
     add_executable(ValidationTests ${SOURCES} ${RESOURCE_FILES} ${SCRIPTS} ${BABYLON_SCRIPTS} ${REFERENCE_IMAGES})
elseif(UNIX)
    set(SOURCES
        ${SOURCES}
        "X11/App.cpp")
    add_executable(ValidationTests ${BABYLON_SCRIPTS} ${SCRIPTS} ${SOURCES} ${REFERENCE_IMAGES})
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
    PRIVATE Canvas
    ${ADDITIONAL_LIBRARIES}
    PRIVATE XMLHttpRequest)

if(APPLE)
    target_link_libraries(ValidationTests PRIVATE "-framework MetalKit")
    if(IOS)
        set_target_properties(ValidationTests PROPERTIES
            MACOSX_BUNDLE true
            MACOSX_BUNDLE_INFO_PLIST "${PLIST_FILE}"
            XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
            RESOURCE "${RESOURCE_FILES}"

            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 9.0
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.BabylonNative.ValidationTests.iOS"

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
        set_target_properties(ValidationTests PROPERTIES
            MACOSX_BUNDLE true
            MACOSX_BUNDLE_INFO_PLIST "${PLIST_FILE}"
            XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
            RESOURCE "${RESOURCE_FILES}"
            FOLDER "ValidationTests")
    endif()
endif()

foreach(SCRIPT ${SCRIPTS} ${BABYLON_SCRIPTS} ${DEPENDENCIES})
    get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMENT "Copying ${SCRIPT_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
endforeach()

foreach(IMAGE ${REFERENCE_IMAGES})
    get_filename_component(IMAGE_NAME "${IMAGE}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/ReferenceImages/${IMAGE_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${IMAGE}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ReferenceImages/${IMAGE_NAME}"
        COMMENT "Copying ${IMAGE_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${IMAGE}")
endforeach()

set_property(TARGET ValidationTests PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ReferenceImages PREFIX ReferenceImages FILES ${REFERENCE_IMAGES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules PREFIX Scripts FILES ${BABYLON_SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
