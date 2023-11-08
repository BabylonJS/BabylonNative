set(SCRIPTS
    "Scripts/symlink_target.js"
    "Scripts/tests.js")

set(EXTERNAL_SCRIPTS
    "../node_modules/chai/chai.js"
    "../node_modules/mocha/mocha.js")

set(SOURCES
    "Shared/Shared.cpp"
    "Shared/Shared.h")

if(APPLE)
    if(IOS)
        set(SOURCES ${SOURCES}
            "iOS/App.mm")

        set(PLIST_FILE
            "${CMAKE_CURRENT_LIST_DIR}/iOS/Info.plist")

        set(STORYBOARD
            "${CMAKE_CURRENT_LIST_DIR}/iOS/Base.lproj/LaunchScreen.storyboard"
            "${CMAKE_CURRENT_LIST_DIR}/iOS/Base.lproj/Main.storyboard")

        set_source_files_properties(
            ${SCRIPTS}
            ${EXTERNAL_SCRIPTS}
            PROPERTIES MACOSX_PACKAGE_LOCATION "Scripts")
    else()
        set(SOURCES ${SOURCES}
            macOS/App.mm)
    endif()
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
    set(SOURCES ${SOURCES}
        ${APPX_FILES}
        ${APPX_ASSETS}
        UWP/App.cpp)
elseif(WIN32)
    set(SOURCES ${SOURCES}
        Win32/App.cpp)
elseif(UNIX AND NOT ANDROID)
    set(SOURCES ${SOURCES}
        Linux/App.cpp)
endif()

add_executable(UnitTests ${SOURCES} ${SCRIPTS} ${EXTERNAL_SCRIPTS})
target_compile_definitions(UnitTests PRIVATE JSRUNTIMEHOST_PLATFORM="${JSRUNTIMEHOST_PLATFORM}")

if(WINDOWS_STORE)
    target_compile_options(UnitTests PRIVATE /ZW)
    target_compile_options(UnitTests PRIVATE /await)
endif()

target_link_libraries(UnitTests
    PRIVATE AppRuntime
    PRIVATE Console
    PRIVATE AbortController
    PRIVATE Scheduling
    PRIVATE ScriptLoader
    PRIVATE URL
    PRIVATE UrlLib
    PRIVATE XMLHttpRequest
    PRIVATE WebSocket
    ${ADDITIONAL_LIBRARIES})

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET UnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:UnitTests>>,copy,true> $<TARGET_RUNTIME_DLLS:UnitTests> $<TARGET_FILE_DIR:UnitTests> COMMAND_EXPAND_LISTS)

if(IOS)
    set_target_properties(UnitTests PROPERTIES
        MACOSX_BUNDLE true
        MACOSX_BUNDLE_INFO_PLIST "${PLIST_FILE}"
        RESOURCE "${STORYBOARD}"
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET}
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.jsruntimehost.unittests")
else()
    foreach(SCRIPT ${SCRIPTS} ${EXTERNAL_SCRIPTS})
        get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
        add_custom_command(
            OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
            COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
            COMMENT "Copying ${SCRIPT_NAME}"
            MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
    endforeach()

    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/symlink_target.js" "${CMAKE_CFG_INTDIR}/Scripts/symlink_1.js")
    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/symlink_1.js" "${CMAKE_CFG_INTDIR}/Scripts/symlink_2.js")
endif()

set_property(TARGET UnitTests PROPERTY FOLDER Tests)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules PREFIX node_modules FILES ${EXTERNAL_SCRIPTS})
