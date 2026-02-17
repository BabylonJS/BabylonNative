cmake_minimum_required(VERSION 3.18)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------
FetchContent_Declare(CMakeExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG ea28b7689530bfdc4905806f27ecf7e8ed4b5419)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz")
# --------------------------------------------------

FetchContent_MakeAvailable(CMakeExtensions)

project(TestInstall)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32 OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID AND NOT APPLE))
    FetchContent_MakeAvailable_With_Message(googletest)
endif()

# __cplusplus value is not in sync with language version used. MVSC needs this flag to update it accordingly
# https://gitlab.kitware.com/cmake/cmake/-/issues/18837
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

set(NAPI_JAVASCRIPT_ENGINE "Chakra")

if(WIN32 AND NOT WINDOWS_STORE)
    set(SOURCE_DIR "../../Apps/UnitTests/Source")
    set(SOURCES
        "${SOURCE_DIR}/App.cpp"
        "${SOURCE_DIR}/App.h"
        "${SOURCE_DIR}/App.Win32.cpp"
        "${SOURCE_DIR}/Tests.Device.D3D11.cpp"
        "${SOURCE_DIR}/Tests.ExternalTexture.cpp"
        "${SOURCE_DIR}/Tests.ExternalTexture.D3D11.cpp"
        "${SOURCE_DIR}/Tests.JavaScript.cpp"
        "${SOURCE_DIR}/Tests.ShaderCache.cpp"
        "${SOURCE_DIR}/Utils.D3D11.cpp"
        "${SOURCE_DIR}/Utils.h")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_executable(TestInstall ${SOURCES})

message(STATUS "Install include directory: ${INSTALL_DIR}/include")
target_include_directories(TestInstall PRIVATE "${INSTALL_DIR}/include")

message(STATUS "Install lib directory: ${INSTALL_DIR}/lib")
set(INSTALL_LIBS_DIR "${INSTALL_DIR}/lib")
target_link_directories(TestInstall PRIVATE ${INSTALL_LIBS_DIR})
file(GLOB INSTALL_LIBS "${INSTALL_LIBS_DIR}/*.lib")
set(INSTALL_LIB_NAMES)
foreach(LIB_PATH ${INSTALL_LIBS})
    get_filename_component(LIB_NAME ${LIB_PATH} NAME_WLE)
    list(APPEND INSTALL_LIB_NAMES ${LIB_NAME})
endforeach()
message(STATUS "Found libs:")
foreach(LIB_NAME ${INSTALL_LIB_NAMES})
    message(STATUS "  ${LIB_NAME}")
endforeach()

target_link_libraries(TestInstall
    ${INSTALL_LIB_NAMES}
    gtest_main
    chakrart
    d3d11
    d3dcompiler
    onecore)

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET TestInstall POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:TestInstall>>,copy,true> $<TARGET_RUNTIME_DLLS:TestInstall> $<TARGET_FILE_DIR:TestInstall> COMMAND_EXPAND_LISTS)
