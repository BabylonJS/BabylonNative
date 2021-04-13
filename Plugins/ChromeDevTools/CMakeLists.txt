set(SOURCES 
    "Include/Babylon/Plugins/ChromeDevTools.h"
    "Source/ChromeDevToolsImpl.h"
    "Source/ChromeDevTools.cpp")

set(CHROME_DEVTOOLS_SUPPORTED_ENGINES "V8")

if (NAPI_JAVASCRIPT_ENGINE IN_LIST CHROME_DEVTOOLS_SUPPORTED_ENGINES)
    set(SOURCES ${SOURCES} "Source/ChromeDevTools${NAPI_JAVASCRIPT_ENGINE}.cpp")
else()
    set(SOURCES ${SOURCES} "Source/ChromeDevToolsNull.cpp")
endif()

add_library(ChromeDevTools ${SOURCES})
warnings_as_errors(ChromeDevTools)

target_include_directories(ChromeDevTools INTERFACE "Include")
target_include_directories(ChromeDevTools PRIVATE "Include/Babylon/Plugins")

target_link_to_dependencies(ChromeDevTools PUBLIC JsRuntime)

if (NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    target_link_to_dependencies(ChromeDevTools PRIVATE v8inspector)
endif()

set_property(TARGET ChromeDevTools PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
