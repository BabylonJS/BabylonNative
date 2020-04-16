set(SOURCES 
    "Include/Babylon/Polyfills/DeviceInputSystem.h"
    "Source/DeviceInputSystem.cpp"
    "Source/DeviceInputSystem.h")

add_library(DeviceInputSystem ${SOURCES})

target_include_directories(DeviceInputSystem PUBLIC "Include")

target_link_to_dependencies(DeviceInputSystem
    PUBLIC JsRuntime
    PUBLIC BabylonNativeUtils)

set_property(TARGET DeviceInputSystem PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
