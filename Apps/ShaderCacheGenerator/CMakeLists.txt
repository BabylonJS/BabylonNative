# ShaderCacheGenerator
#
# A headless Win32 + OpenGL tool that builds a GPU shader cache for a Babylon.js
# scene. It hosts the scene script through the Babylon::Embedding Runtime/View
# API (backed by an offscreen window + the ANGLE OpenGL backend), renders frames
# until the scene has finished compiling its shaders, and writes the resulting
# ShaderCache to a file.
#
# Usage:
#     ShaderCacheGenerator <scene.js> <output.bin> [--width N] [--height N]
#                          [--max-frames N] [--stable-checks N] [--check-interval N]
#
# The produced cache is GRAPHICS_API-specific: configuring with
# -DGRAPHICS_API=OpenGLWindowsDevOnly (which maps to the OpenGL/ANGLE backend)
# yields GLES shaders compatible with the Android OpenGL backend.

set(PLAYGROUND_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Playground")

set(SOURCES
    "Source/Main.cpp")

# Reused from Apps/Playground/Shared: command-line parsing, process diagnostics
# (console/crash/exit handling) and the log callback.
set(SHARED_SOURCES
    "${PLAYGROUND_DIR}/Shared/CommandLine.cpp"
    "${PLAYGROUND_DIR}/Shared/CommandLine.h"
    "${PLAYGROUND_DIR}/Shared/Diagnostics.cpp"
    "${PLAYGROUND_DIR}/Shared/Diagnostics.h"
    "${PLAYGROUND_DIR}/Shared/PlaygroundScripts.cpp"
    "${PLAYGROUND_DIR}/Shared/PlaygroundScripts.h")

add_executable(ShaderCacheGenerator ${SOURCES} ${SHARED_SOURCES})

warnings_as_errors(ShaderCacheGenerator)

target_compile_definitions(ShaderCacheGenerator
    PRIVATE UNICODE
    PRIVATE _UNICODE)

# So Main.cpp can include the reused headers as <Shared/CommandLine.h> etc.
target_include_directories(ShaderCacheGenerator PRIVATE "${PLAYGROUND_DIR}")

# SUBSYSTEM:CONSOLE so stdout/stderr behave like a normal console app; the tool
# still creates a hidden Win32 window for the GL swapchain.
set_target_properties(ShaderCacheGenerator PROPERTIES
    LINK_FLAGS "/SUBSYSTEM:CONSOLE")

# Embedding transitively provides the polyfills/plugins (NativeEngine,
# ShaderCache, ...) and their include paths. ShaderCache is listed explicitly
# because Main.cpp includes <Babylon/Plugins/ShaderCache.h> and calls Save().
# bx / Foundation are required by the reused Shared sources (Diagnostics.cpp,
# PlaygroundScripts.cpp). Shlwapi provides UrlCreateFromPathA for the file://
# script URL.
target_link_libraries(ShaderCacheGenerator
    PRIVATE Embedding
    PRIVATE ShaderCache
    PRIVATE bx
    PRIVATE Foundation
    PRIVATE Shlwapi)

# Copy transitive runtime DLLs next to the executable.
add_custom_command(TARGET ShaderCacheGenerator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:ShaderCacheGenerator>>,copy,true> $<TARGET_RUNTIME_DLLS:ShaderCacheGenerator> $<TARGET_FILE_DIR:ShaderCacheGenerator> COMMAND_EXPAND_LISTS)

# Copy the ANGLE GL DLLs so the OpenGL backend can load at runtime.
if(ANGLE_LIBEGL)
    add_custom_command(TARGET ShaderCacheGenerator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ANGLE_LIBEGL}" "$<TARGET_FILE_DIR:ShaderCacheGenerator>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ANGLE_LIBGLESV2}" "$<TARGET_FILE_DIR:ShaderCacheGenerator>"
        COMMENT "Copying ANGLE libraries to ShaderCacheGenerator output directory")
endif()

set_property(TARGET ShaderCacheGenerator PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group("Shared" FILES ${SHARED_SOURCES})
