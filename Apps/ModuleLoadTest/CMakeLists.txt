if(NOT(WIN32 AND NOT WINDOWS_STORE))
    # Windows-only for the initial implementation.
    # macOS and Linux support will be added in follow-up commits on this PR.
    return()
endif()

set(SOURCES
    "Source/App.h"
    "Source/App.cpp"
    "Source/App.Win32.cpp"
    "Source/ModuleSnapshot.h"
    "Source/ModuleSnapshot.Win32.cpp")

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
    PRIVATE XMLHttpRequest)

add_test(NAME ModuleLoadTest COMMAND ModuleLoadTest CONFIGURATIONS Release RelWithDebInfo)

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
add_custom_command(TARGET ModuleLoadTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:ModuleLoadTest>>,copy,true> $<TARGET_RUNTIME_DLLS:ModuleLoadTest> $<TARGET_FILE_DIR:ModuleLoadTest> COMMAND_EXPAND_LISTS)

set_property(TARGET ModuleLoadTest PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
