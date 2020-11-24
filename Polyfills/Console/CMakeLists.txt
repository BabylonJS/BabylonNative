set(SOURCES
    "Include/Babylon/Polyfills/Console.h"
    "Source/Console.cpp"
    "Source/Console.h")

add_library(Console ${SOURCES})
warnings_as_errors(Console)

target_include_directories(Console PUBLIC "Include")

if(WIN32)
    target_compile_definitions(Console PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

target_link_to_dependencies(Console PUBLIC napi)

set_property(TARGET Console PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
