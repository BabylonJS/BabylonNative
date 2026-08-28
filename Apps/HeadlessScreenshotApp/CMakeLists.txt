FetchContent_Declare(DirectXTK
    GIT_REPOSITORY https://github.com/microsoft/DirectXTK.git
    GIT_TAG e7d9b64ca6bb8977d4e3006727deaf57a7c1de84
    # Populate the source tree without configuring it: the shaders have to be
    # compiled before DirectXTK's own CMakeLists.txt is processed (see below).
    SOURCE_SUBDIR DoNotConfigure)

include(${CMAKE_CURRENT_LIST_DIR}/../../Scripts/extensions.cmake)

set(BUILD_TESTING OFF)
set(BUILD_TOOLS OFF)
set(BUILD_XAUDIO_WIN8 OFF)
set(DIRECTX_ARCH ${CMAKE_VS_PLATFORM_NAME})

FetchContent_MakeAvailable_With_Message(DirectXTK)

# DirectXTK compiles its HLSL shaders from a custom build step that runs
# `cmake -E env ... CompileShaders.cmd` with a *relative* command name and
# relies on the Windows SDK bin directory being on PATH. Neither holds here:
# CMake no longer resolves a relative command against WORKING_DIRECTORY, and
# the custom build step doesn't inherit a developer command prompt, so fxc.exe
# isn't found. Compile the shaders ourselves at configure time (absolute paths,
# explicitly located fxc.exe) and hand the result to DirectXTK through its
# USE_PREBUILT_SHADERS / COMPILED_SHADERS contract.
set(DIRECTXTK_COMPILED_SHADERS "${directxtk_BINARY_DIR}/Shaders/Compiled")
if(NOT EXISTS "${DIRECTXTK_COMPILED_SHADERS}/SpriteEffect_SpriteVertexShader.inc")
    find_program(FXC_EXECUTABLE fxc
        HINTS
            "$ENV{WindowsSdkVerBinPath}/${CMAKE_VS_PLATFORM_NAME}"
            "$ENV{WindowsSdkDir}bin/${CMAKE_VS_PLATFORM_NAME}"
        PATHS
            "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/bin/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/${CMAKE_VS_PLATFORM_NAME}"
            "$ENV{ProgramFiles}/Windows Kits/10/bin/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/${CMAKE_VS_PLATFORM_NAME}")
    if(NOT FXC_EXECUTABLE)
        message(FATAL_ERROR "fxc.exe (Windows SDK HLSL compiler) is required to build DirectXTK shaders but was not found.")
    endif()

    cmake_path(GET FXC_EXECUTABLE PARENT_PATH FXC_DIR)
    file(MAKE_DIRECTORY "${DIRECTXTK_COMPILED_SHADERS}")

    message(STATUS "Compiling DirectXTK shaders")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env
            "CompileShadersOutput=${DIRECTXTK_COMPILED_SHADERS}"
            "PATH=${FXC_DIR};$ENV{PATH}"
            "${directxtk_SOURCE_DIR}/Src/Shaders/CompileShaders.cmd"
        WORKING_DIRECTORY "${directxtk_SOURCE_DIR}/Src/Shaders"
        OUTPUT_FILE "${DIRECTXTK_COMPILED_SHADERS}/compileshaders.log"
        ERROR_FILE "${DIRECTXTK_COMPILED_SHADERS}/compileshaders.log"
        RESULT_VARIABLE DIRECTXTK_SHADERS_RESULT)
    if(NOT DIRECTXTK_SHADERS_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to compile DirectXTK shaders (${DIRECTXTK_SHADERS_RESULT}). See ${DIRECTXTK_COMPILED_SHADERS}/compileshaders.log")
    endif()
    message(STATUS "Compiling DirectXTK shaders - done")
endif()

set(USE_PREBUILT_SHADERS ON)
set(COMPILED_SHADERS "${DIRECTXTK_COMPILED_SHADERS}")
add_subdirectory("${directxtk_SOURCE_DIR}" "${directxtk_BINARY_DIR}")

set_property(TARGET DirectXTK PROPERTY FOLDER Apps/Dependencies)

set(BABYLON_SCRIPTS
    "../node_modules/babylonjs/babylon.max.js"
    "../node_modules/babylonjs-loaders/babylonjs.loaders.js")

set(SCRIPTS
    "Scripts/index.js")

set(SOURCES
    "Win32/RenderDoc.h"
    "Win32/RenderDoc.cpp"
    "Win32/App.cpp")

add_executable(HeadlessScreenshotApp ${BABYLON_SCRIPTS} ${SCRIPTS} ${SOURCES})

target_compile_definitions(HeadlessScreenshotApp
    PRIVATE UNICODE
    PRIVATE _UNICODE)

target_link_libraries(HeadlessScreenshotApp
    PRIVATE AppRuntime
    PRIVATE Console
    PRIVATE DirectXTK
    PRIVATE ExternalTexture
    PRIVATE NativeEngine
    PRIVATE ScriptLoader
    PRIVATE Window
    PRIVATE XMLHttpRequest)

foreach(SCRIPT ${BABYLON_SCRIPTS} ${SCRIPTS})
    get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMENT "Copying ${SCRIPT_NAME}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
endforeach()

set_property(TARGET HeadlessScreenshotApp PROPERTY FOLDER Apps)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../node_modules PREFIX Scripts FILES ${BABYLON_SCRIPTS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
