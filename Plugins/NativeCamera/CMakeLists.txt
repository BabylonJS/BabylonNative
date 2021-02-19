set(SOURCES 
    "Include/Babylon/Plugins/NativeCamera.h"
    "Source/NativeCamera.cpp")

if(APPLE)
    set(SOURCES ${SOURCES} "Source/NativeCamera${BABYLON_NATIVE_PLATFORM}.mm")
else()
    set(SOURCES ${SOURCES} "Source/NativeCamera${BABYLON_NATIVE_PLATFORM}.cpp")
endif()

add_library(NativeCamera ${SOURCES})
warnings_as_errors(NativeCamera)

target_include_directories(NativeCamera INTERFACE "Include")
target_include_directories(NativeCamera PRIVATE "Include/Babylon/Plugins")

target_link_to_dependencies(NativeCamera
    PUBLIC JsRuntime
    PRIVATE GraphicsInternal)

set_property(TARGET NativeCamera PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
