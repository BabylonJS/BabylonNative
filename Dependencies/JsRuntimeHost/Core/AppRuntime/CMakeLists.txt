if(APPLE)
    set(IMPL_EXT "mm")
else()
    set(IMPL_EXT "cpp")
endif()

set(SOURCES
    "Include/Babylon/Dispatchable.h"
    "Include/Babylon/AppRuntime.h"
    "Source/AppRuntime.cpp"
    "Source/AppRuntime_${NAPI_JAVASCRIPT_ENGINE}.cpp"
    "Source/AppRuntime_${JSRUNTIMEHOST_PLATFORM}.${IMPL_EXT}")

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    list(APPEND SOURCES
        "Source/V8ForegroundTaskRunner.h"
        "Source/V8ForegroundTaskRunner.cpp"
        "Source/V8Platform.h"
        "Source/V8Platform.cpp")
endif()

add_library(AppRuntime ${SOURCES})
warnings_as_errors(AppRuntime)

target_include_directories(AppRuntime
    PRIVATE "Include/Babylon"
    INTERFACE "Include")

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    target_include_directories(AppRuntime PRIVATE "../Node-API/Source")
    if(ANDROID AND NAPI_ANDROID_V8_INTL_ROOT)
        set(snapshot "${NAPI_ANDROID_V8_INTL_ROOT}/snapshot_blob/${ANDROID_ABI}/snapshot_blob.bin")
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${snapshot}")
        file(READ "${snapshot}" snapshot_bytes HEX)
        if(NOT snapshot_bytes)
            message(FATAL_ERROR "Empty V8 startup snapshot: ${snapshot}")
        endif()
        string(REGEX REPLACE "(..)" "0x\\1," snapshot_bytes "${snapshot_bytes}")
        configure_file(Source/AndroidV8Snapshot.h.in AndroidV8Snapshot.h @ONLY)
        target_include_directories(AppRuntime PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
        target_compile_definitions(AppRuntime PRIVATE JSRUNTIMEHOST_EMBED_V8_SNAPSHOT)
    endif()
endif()

target_link_libraries(AppRuntime
    PRIVATE FoundationInternal
    PRIVATE arcana
    PUBLIC JsRuntime)

# AppRuntime_macOS.mm / AppRuntime_iOS.mm call NSLog (Foundation) and other
# Apple ObjC++ APIs.  Xcode's "Link Frameworks Automatically" setting auto-
# links Foundation/CoreFoundation/UIKit for ObjC translation units, so the
# default `-G Xcode` macOS/iOS builds work without an explicit link.  Other
# generators (notably Ninja, which we use for the Hermes macOS CI job to
# work around the Xcode "new build system" rejecting Hermes's multi-target
# generated source) leave those frameworks unresolved.  Declaring them
# PUBLIC here propagates them onto the final executable's link line for
# every generator, and also covers other Apple-side static deps in the
# tree (e.g. UrlLib's `UrlRequest_Apple.mm` reference to NSBundle) without
# needing to patch the dep itself.
if(APPLE)
    target_link_libraries(AppRuntime
        PUBLIC "-framework Foundation"
        PUBLIC "-framework CoreFoundation")
endif()

# WebKitGTK's JavaScriptCore completes some engine work (notably async WebAssembly promises) on a
# GLib main context, which AppRuntime_JavaScriptCore.cpp iterates on the JavaScript thread.
if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JavaScriptCore" AND UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLIB REQUIRED glib-2.0)
    target_include_directories(AppRuntime PRIVATE ${GLIB_INCLUDE_DIRS})
    target_link_libraries(AppRuntime PUBLIC ${GLIB_LINK_LIBRARIES})
    target_compile_definitions(AppRuntime PRIVATE JSRUNTIMEHOST_JAVASCRIPTCORE_GLIB)
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8" AND JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR)
    add_subdirectory(V8Inspector)

    target_link_libraries(AppRuntime
        PRIVATE v8inspector)

    target_compile_definitions(AppRuntime
        PRIVATE ENABLE_V8_INSPECTOR)

    target_link_libraries(AppRuntime
        PRIVATE v8inspector)

    set_property(TARGET v8inspector PROPERTY FOLDER Dependencies)
elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "QuickJS")
    target_link_libraries(AppRuntime PRIVATE qjs)
endif()

set_property(TARGET AppRuntime PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(AppRuntimeInternal INTERFACE)
target_include_directories(AppRuntimeInternal INTERFACE "Source")
target_link_libraries(AppRuntimeInternal INTERFACE AppRuntime INTERFACE FoundationInternal)
