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

add_library(AppRuntime ${SOURCES})
warnings_as_errors(AppRuntime)

target_include_directories(AppRuntime
    PRIVATE "Include/Babylon"
    INTERFACE "Include")

target_link_libraries(AppRuntime
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
