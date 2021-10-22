set(SOURCES 
    "Include/Babylon/Plugins/ChromeDevTools.h")

if (WIN32)
    set(CHROME_DEVTOOLS_SUPPORTED_ENGINES ${CHROME_DEVTOOLS_SUPPORTED_ENGINES} "JSI")
    if (NOT WINDOWS_STORE)
        set(CHROME_DEVTOOLS_SUPPORTED_ENGINES ${CHROME_DEVTOOLS_SUPPORTED_ENGINES} "V8")
    endif()
elseif(ANDROID)
    set(CHROME_DEVTOOLS_SUPPORTED_ENGINES ${CHROME_DEVTOOLS_SUPPORTED_ENGINES} "V8")
endif()

if (NAPI_JAVASCRIPT_ENGINE IN_LIST CHROME_DEVTOOLS_SUPPORTED_ENGINES)
    set(SOURCES ${SOURCES} "Source/ChromeDevTools${NAPI_JAVASCRIPT_ENGINE}.cpp")
else()
    set(SOURCES ${SOURCES} "Source/ChromeDevToolsNull.cpp")
endif()

add_library(ChromeDevTools ${SOURCES})
warnings_as_errors(ChromeDevTools)

if (NAPI_JAVASCRIPT_ENGINE IN_LIST CHROME_DEVTOOLS_SUPPORTED_ENGINES AND NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    target_link_to_dependencies(ChromeDevTools PRIVATE v8inspector)
endif()

target_include_directories(ChromeDevTools INTERFACE "Include")
target_include_directories(ChromeDevTools PRIVATE "Include/Babylon/Plugins")

target_link_to_dependencies(ChromeDevTools PUBLIC JsRuntime)

set_property(TARGET ChromeDevTools PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
