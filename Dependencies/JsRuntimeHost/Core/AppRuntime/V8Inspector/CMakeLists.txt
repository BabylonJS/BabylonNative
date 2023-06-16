set(SOURCES
    "Include/V8Inc.h"
    "Include/V8InspectorAgent.h"
    "Source/V8InspectorAgent.cpp"
    "Source/V8InspectorSocket.h"
    "Source/V8InspectorSocket.cpp"
    "Source/V8InspectorSocketServer.h"
    "Source/V8InspectorSocketServer.cpp"
    "Source/V8InspectorTCP.h"
    "Source/V8InspectorTCP.cpp"
    "Source/V8InspectorUtils.h"
    "Source/V8InspectorUtils.cpp")

add_library(v8inspector ${SOURCES})
warnings_as_errors(v8inspector)

target_include_directories(v8inspector
    PUBLIC "Include"
    PRIVATE "${llhttp_SOURCE_DIR}/include")

target_compile_definitions(v8inspector
    PRIVATE ASIO_STANDALONE)

target_link_libraries(v8inspector
    PRIVATE asio
    PRIVATE llhttp_static
    PRIVATE napi)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
