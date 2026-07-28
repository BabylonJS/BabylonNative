# The unit tests build on every platform Babylon Native supports.
#
# The JsRuntimeHost suites (Source/Tests.JsRuntime.cpp) only need a JavaScript
# engine and the polyfills, so they are always included -- this is the base of
# the target. The Babylon Native suites drive a real graphics device through a
# platform window, so they are added on top only where that host exists; see
# BABYLON_NATIVE_UNIT_TESTS_GRAPHICS in the root CMakeLists.
#
# Android does not build this target at all: it drives the same sources through
# the Gradle instrumented-test harness in the Android subdirectory.

# --------------------------------------------------
# JsRuntimeHost suites (every platform)
# --------------------------------------------------

set(SOURCES
    "Source/Tests.JsRuntime.cpp")

set(TEST_ASSETS
    "JavaScript/dist/tests.jsRuntime.js")

# Data files the JsRuntimeHost suites load over app:/// (XMLHttpRequest, Fetch,
# Blob, File).
set(JSRUNTIME_ASSETS
    "Assets/Halo_Believe.ply"
    "Assets/sample.json")

set(JSRUNTIME_SCRIPTS
    "Scripts/symlink_target.js")

set(LIBRARIES
    PRIVATE AbortController
    PRIVATE AppRuntime
    PRIVATE Blob
    PRIVATE Console
    PRIVATE Fetch
    PRIVATE File
    PRIVATE Foundation
    PRIVATE Performance
    PRIVATE Scheduling
    PRIVATE ScriptLoader
    PRIVATE TextDecoder
    PRIVATE TextEncoder
    PRIVATE URL
    PRIVATE UrlLib
    PRIVATE WebSocket
    PRIVATE XMLHttpRequest
    PRIVATE gtest_main)

file(GLOB TEST_SCRIPTS "JavaScript/src/*.ts")
set_source_files_properties(${TEST_SCRIPTS} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------
# Babylon Native suites (desktop only)
# --------------------------------------------------

if(BABYLON_NATIVE_UNIT_TESTS_GRAPHICS)
    if(NOT((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS AND NOT VISIONOS) OR (UNIX AND NOT ANDROID AND NOT APPLE)))
        message(FATAL_ERROR "BABYLON_NATIVE_UNIT_TESTS_GRAPHICS requires a desktop platform, not ${CMAKE_SYSTEM_NAME}.")
    endif()

    option(BABYLON_NATIVE_TESTS_USE_NOOP_METAL_DEVICE "Use a noop Metal device for Apple platforms." OFF)

    set(BABYLONJS_ASSETS
        "../node_modules/babylonjs/babylon.max.js")

    set(BABYLONJS_MATERIALS_ASSETS
        "../node_modules/babylonjs-materials/babylonjs.materials.js")

    set(TEST_ASSETS ${TEST_ASSETS}
        "JavaScript/dist/tests.externalTexture.deviceLoss.js"
        "JavaScript/dist/tests.externalTexture.msaa.js"
        "JavaScript/dist/tests.externalTexture.render.js"
        "JavaScript/dist/tests.javaScript.all.js"
        "JavaScript/dist/tests.shaderCache.basicScene.js"
        "JavaScript/dist/tests.shaderCompilation.comprehensiveGLSL.js")

    set(SOURCES ${SOURCES}
        "Source/App.h"
        "Source/App.cpp"
        "Source/Tests.ExternalTexture.cpp"
        "Source/Tests.ExternalTexture.DeviceLoss.cpp"
        "Source/Tests.ExternalTexture.Msaa.cpp"
        "Source/Tests.ExternalTexture.Render.cpp"
        "Source/Tests.JavaScript.cpp"
        "Source/Tests.NativeEngine.Teardown.cpp"
        "Source/Tests.ShaderCache.cpp"
        "Source/Tests.ShaderCompilation.cpp"
        "Source/Tests.UniformPadding.cpp"
        "Source/Helpers.h"
        "Source/Helpers.${GRAPHICS_API}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}")

    if(GRAPHICS_API STREQUAL "D3D11" OR GRAPHICS_API STREQUAL "D3D12")
        set(SOURCES ${SOURCES}
            "Source/Tests.Device.cpp")
    endif()

    if(GRAPHICS_API STREQUAL "D3D11")
        set(SOURCES ${SOURCES}
            "Source/Tests.Device.${GRAPHICS_API}.cpp")
    endif()

    set(LIBRARIES ${LIBRARIES}
        PRIVATE Canvas
        PRIVATE GraphicsDevice
        PRIVATE ExternalTexture
        PRIVATE NativeEngine
        PRIVATE NativeEncoding
        PRIVATE ShaderCache
        PRIVATE Window)

    if(APPLE)
        set(SOURCES ${SOURCES} "Source/App.Apple.mm")
        if(BABYLON_NATIVE_TESTS_USE_NOOP_METAL_DEVICE)
            set(ADDITIONAL_COMPILE_DEFINITIONS
                PRIVATE USE_NOOP_METAL_DEVICE
                PRIVATE SKIP_EXTERNAL_TEXTURE_TESTS)
        endif()
        set(ADDITIONAL_LIBRARIES PRIVATE "-framework JavaScriptCore")
    elseif(UNIX AND NOT ANDROID)
        set(SOURCES ${SOURCES} "Source/App.X11.cpp")
        set(ADDITIONAL_COMPILE_DEFINITIONS PRIVATE SKIP_EXTERNAL_TEXTURE_TESTS)
    elseif(WIN32)
        set(SOURCES ${SOURCES}
            "Source/App.Win32.cpp"
            "Source/RenderDoc.h"
            "Source/RenderDoc.cpp")
        set(ADDITIONAL_COMPILE_DEFINITIONS ${ADDITIONAL_COMPILE_DEFINITIONS} PRIVATE HAS_RENDERDOC)
    endif()
else()
    # No graphics device to configure, so the host just runs gtest. One file
    # covers every platform; see the comment at the top of it.
    set(SOURCES ${SOURCES} "Source/App.JsRuntime.cpp")

    if(IOS)
        set(PLIST_FILE "${CMAKE_CURRENT_LIST_DIR}/iOS/Info.plist")
    endif()

    if(APPLE)
        set(ADDITIONAL_LIBRARIES PRIVATE "-framework JavaScriptCore")
    endif()
endif()

# --------------------------------------------------
# Target
# --------------------------------------------------

if(IOS)
    # Loaded out of the app bundle rather than copied next to the executable.
    set_source_files_properties(${TEST_ASSETS} ${JSRUNTIME_ASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION "Assets")
    set_source_files_properties(${JSRUNTIME_SCRIPTS} PROPERTIES MACOSX_PACKAGE_LOCATION "Scripts")
else()
    set_source_files_properties(${JSRUNTIME_ASSETS} ${JSRUNTIME_SCRIPTS} PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

add_executable(UnitTests ${BABYLONJS_ASSETS} ${BABYLONJS_MATERIALS_ASSETS} ${TEST_ASSETS} ${JSRUNTIME_ASSETS} ${JSRUNTIME_SCRIPTS} ${TEST_SCRIPTS} ${SOURCES})
target_link_libraries(UnitTests ${LIBRARIES} ${ADDITIONAL_LIBRARIES})

# The JsRuntimeHost suite reports its host platform to the JS bundle so that it
# can skip the cases that do not apply there.
# (ARCANA_TEST_HOOKS is defined globally from the root CMakeLists, since the
# hook has to be active in Core/AppRuntime's translation unit too.)
target_compile_definitions(UnitTests
    PRIVATE JSRUNTIMEHOST_PLATFORM="${JSRUNTIMEHOST_PLATFORM}")

# The V8JSI Node-API shim does not implement napi_create_dataview, so the
# CreateDataViewRejectsOverflowingRange test is compiled out on that backend.
if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    target_compile_definitions(UnitTests PRIVATE JSRUNTIMEHOST_NAPI_ENGINE_JSI)
endif()

target_compile_definitions(UnitTests PRIVATE ${ADDITIONAL_COMPILE_DEFINITIONS})

if(GRAPHICS_API STREQUAL "D3D12")
    target_compile_definitions(UnitTests PRIVATE SKIP_RENDER_TESTS)
endif()

add_test(NAME UnitTests COMMAND UnitTests)

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET UnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:UnitTests>>,copy,true> $<TARGET_RUNTIME_DLLS:UnitTests> $<TARGET_FILE_DIR:UnitTests> COMMAND_EXPAND_LISTS)

if(ANGLE_LIBEGL)
    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ANGLE_LIBEGL}" "$<TARGET_FILE_DIR:UnitTests>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ANGLE_LIBGLESV2}" "$<TARGET_FILE_DIR:UnitTests>"
        COMMENT "Copying ANGLE libraries to UnitTests output directory"
    )
endif()

if(IOS)
    set_target_properties(UnitTests PROPERTIES
        MACOSX_BUNDLE true
        MACOSX_BUNDLE_INFO_PLIST "${PLIST_FILE}"
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET}
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.babylonnative.unittests")
else()
    foreach(ASSET ${BABYLONJS_ASSETS} ${BABYLONJS_MATERIALS_ASSETS} ${TEST_ASSETS} ${JSRUNTIME_ASSETS})
        get_filename_component(ASSET_NAME "${ASSET}" NAME)
        add_custom_command(
            OUTPUT "${CMAKE_CFG_INTDIR}/Assets/${ASSET_NAME}"
            COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${ASSET}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Assets/${ASSET_NAME}"
            COMMENT "Copying ${ASSET_NAME}"
            MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${ASSET}")
    endforeach()

    # The JsRuntimeHost suite validates that the app:/// scheme resolves
    # symlinks, including a symlink to a symlink. These cannot be committed to
    # the repo, so they are created next to the copied target at build time.
    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:UnitTests>/Scripts"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Scripts/symlink_target.js" "$<TARGET_FILE_DIR:UnitTests>/Scripts/symlink_target.js"
        COMMAND "${CMAKE_COMMAND}" -E rm -f "$<TARGET_FILE_DIR:UnitTests>/Scripts/symlink_1.js" "$<TARGET_FILE_DIR:UnitTests>/Scripts/symlink_2.js"
        COMMAND "${CMAKE_COMMAND}" -E create_symlink "symlink_target.js" "$<TARGET_FILE_DIR:UnitTests>/Scripts/symlink_1.js"
        COMMAND "${CMAKE_COMMAND}" -E create_symlink "symlink_1.js" "$<TARGET_FILE_DIR:UnitTests>/Scripts/symlink_2.js"
        COMMENT "Creating symlinks for the JsRuntimeHost file resolution tests")
endif()

set_property(TARGET UnitTests PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/JavaScript/dist PREFIX Assets FILES ${TEST_ASSETS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/JavaScript/src PREFIX TypeScript FILES ${TEST_SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Assets FILES ${JSRUNTIME_ASSETS} ${JSRUNTIME_SCRIPTS})

if(BABYLON_NATIVE_UNIT_TESTS_GRAPHICS)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules/babylonjs PREFIX Assets FILES ${BABYLONJS_ASSETS})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules/babylonjs-materials PREFIX Assets FILES ${BABYLONJS_MATERIALS_ASSETS})
endif()
