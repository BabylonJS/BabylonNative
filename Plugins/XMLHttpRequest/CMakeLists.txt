set(SOURCES 
    "Include/Babylon/XMLHttpRequest.h"
    "Source/XMLHttpRequest.cpp"
    "Source/XMLHttpRequest${BABYLON_NATIVE_PLATFORM}.cpp")

add_library(XMLHttpRequest ${SOURCES})

target_include_directories(XMLHttpRequest PRIVATE "Include/Babylon")
target_include_directories(XMLHttpRequest INTERFACE "Include")

target_link_libraries(XMLHttpRequest 
    PUBLIC JsRuntime
    PUBLIC arcana
    PRIVATE BabylonNativeUtils)

set_property(TARGET XMLHttpRequest PROPERTY FOLDER Plugins/Babylon)
