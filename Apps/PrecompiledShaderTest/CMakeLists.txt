# PrecompiledShaderTest
#
# A cross-platform headless app that validates the ShaderTool -> ShaderCache ->
# NativeEngine pipeline by rendering a scene using pre-compiled shaders and
# saving the result as a PNG screenshot.
#
# The JavaScript build produces:
#     JavaScript/dist/index.js                      -- bundled scene script
#     JavaScript/dist/shaders/<name>/vertex.fx      -- preprocessed vertex shaders
#     JavaScript/dist/shaders/<name>/fragment.fx    -- preprocessed fragment shaders
#
# Build-time: ShaderTool compiles shader pairs from JavaScript/dist/shaders/ into shaders.bin
# Run-time:   The app loads shaders.bin into ShaderCache and renders without ShaderCompiler

# ---- JavaScript build ----

set(JS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/JavaScript")
npm(install WORKING_DIRECTORY "${JS_DIR}")
npm(run build WORKING_DIRECTORY "${JS_DIR}")

# ---- Shaders ----

# Shader directory produced by the JavaScript build.
set(JS_SHADER_DIR "${JS_DIR}/dist/shaders")

# Shader pairs to compile with ShaderTool (vertex, fragment).
# Each pair corresponds to an effect registered in JavaScript/src/effects/effectList.ts.
set(SHADER_FILES
    "${JS_SHADER_DIR}/simple/vertex.fx"
    "${JS_SHADER_DIR}/simple/fragment.fx")

set(SHADERS_BIN "${CMAKE_CURRENT_BINARY_DIR}/shaders.bin")

add_custom_command(
    OUTPUT "${SHADERS_BIN}"
    COMMAND ShaderTool -o "${SHADERS_BIN}" ${SHADER_FILES}
    DEPENDS ShaderTool ${SHADER_FILES}
    COMMENT "Compiling shaders with ShaderTool"
    VERBATIM)

# ---- Scripts ----

# The bundled scene script produced by the JavaScript build.
set(SCRIPTS
    "JavaScript/dist/index.js")

# ---- Sources ----

set(SOURCES
    "Source/App.h"
    "Source/App.cpp")

if(WIN32)
    FetchContent_Declare(DirectXTK
        GIT_REPOSITORY https://github.com/microsoft/DirectXTK.git
        GIT_TAG e7d9b64ca6bb8977d4e3006727deaf57a7c1de84)

    set(BUILD_TESTING OFF)
    set(BUILD_TOOLS OFF)
    set(BUILD_XAUDIO_WIN8 OFF)
    set(DIRECTX_ARCH ${CMAKE_VS_PLATFORM_NAME})

    if(NOT TARGET DirectXTK)
        FetchContent_MakeAvailable_With_Message(DirectXTK)
        set_property(TARGET DirectXTK PROPERTY FOLDER Apps/Dependencies)
    endif()

    set(SOURCES ${SOURCES} "Source/App.Win32.cpp")
    set(PLATFORM_LIBRARIES PRIVATE DirectXTK)
elseif(APPLE)
    find_library(JAVASCRIPTCORE_LIBRARY JavaScriptCore)
    set(SOURCES ${SOURCES} "Source/App.Apple.mm")
    set(PLATFORM_LIBRARIES PRIVATE ${JAVASCRIPTCORE_LIBRARY})
endif()

# ---- Reference image ----

set(REFERENCE_IMAGE "Source/reference.png")

# ---- Executable ----

add_executable(PrecompiledShaderTest
    ${SCRIPTS}
    ${SHADERS_BIN}
    ${SOURCES}
    ${REFERENCE_IMAGE})

warnings_as_errors(PrecompiledShaderTest)

target_compile_definitions(PrecompiledShaderTest
    PRIVATE UNICODE
    PRIVATE _UNICODE)

target_link_libraries(PrecompiledShaderTest
    PRIVATE AppRuntime
    PRIVATE Console
    PRIVATE ExternalTexture
    PRIVATE NativeEngine
    PRIVATE ScriptLoader
    PRIVATE ShaderCache
    PRIVATE Window
    ${PLATFORM_LIBRARIES})

if(APPLE)
    enable_objc_arc(PrecompiledShaderTest)
endif()

# ---- Copy assets to output directory ----

set(ASSETS)
foreach(SCRIPT ${SCRIPTS})
    list(APPEND ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
endforeach()
list(APPEND ASSETS "${SHADERS_BIN}")
list(APPEND ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/${REFERENCE_IMAGE}")

foreach(ASSET ${ASSETS})
    get_filename_component(ASSET_NAME "${ASSET}" NAME)
    add_custom_command(TARGET PrecompiledShaderTest POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy "${ASSET}" "$<TARGET_FILE_DIR:PrecompiledShaderTest>/${ASSET_NAME}")
endforeach()

# ---- Project organization ----

set_property(TARGET PrecompiledShaderTest PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group("Assets" FILES ${SCRIPTS})
source_group("Assets" FILES ${SHADERS_BIN})
source_group("Assets" FILES ${REFERENCE_IMAGE})
