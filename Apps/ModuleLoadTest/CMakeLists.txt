set(SOURCES
    "Source/App.h"
    "Source/App.cpp"
    "Source/ModuleSnapshot.h")

if(WIN32 AND NOT WINDOWS_STORE)
    list(APPEND SOURCES
        "Source/App.Win32.cpp"
        "Source/ModuleSnapshot.Win32.cpp")
elseif(APPLE AND NOT IOS)
    list(APPEND SOURCES
        "Source/App.Apple.mm"
        "Source/ModuleSnapshot.macOS.mm")
    find_library(JAVASCRIPTCORE_LIBRARY JavaScriptCore)
    set(ADDITIONAL_LIBRARIES PRIVATE ${JAVASCRIPTCORE_LIBRARY})
elseif(UNIX AND NOT ANDROID)
    list(APPEND SOURCES
        "Source/App.X11.cpp"
        "Source/ModuleSnapshot.Linux.cpp")
else()
    # Unsupported platform (UWP, Android, iOS) — mobile/store bundling
    # changes module-load semantics. Silently skip.
    return()
endif()

add_executable(ModuleLoadTest ${SOURCES})

target_link_libraries(ModuleLoadTest
    PRIVATE AppRuntime
    PRIVATE Blob
    PRIVATE Canvas
    PRIVATE Console
    PRIVATE GraphicsDevice
    PRIVATE NativeEngine
    PRIVATE NativeEncoding
    PRIVATE Window
    PRIVATE XMLHttpRequest
    ${ADDITIONAL_LIBRARIES})

add_test(NAME ModuleLoadTest COMMAND ModuleLoadTest CONFIGURATIONS Release RelWithDebInfo)

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
add_custom_command(TARGET ModuleLoadTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:ModuleLoadTest>>,copy,true> $<TARGET_RUNTIME_DLLS:ModuleLoadTest> $<TARGET_FILE_DIR:ModuleLoadTest> COMMAND_EXPAND_LISTS)

set_property(TARGET ModuleLoadTest PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
