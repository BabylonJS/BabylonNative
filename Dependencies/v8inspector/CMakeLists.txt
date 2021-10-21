# ------------------------------- asio -------------------------------
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE "Dependencies/asio/asio/include")

# ------------------------------ llhttp ------------------------------
set(LLHTTP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/llhttp")

if (NOT EXISTS "${LLHTTP_DIR}/build")
    message(STATUS "Generating llhttp library")

    if (WIN32)
        set(NPM_CMD "npm.cmd")
    else()
        set(NPM_CMD "npm")
    endif()

    # Download npm dependencies, if necessary
    if (NOT EXISTS "${LLHTTP_DIR}/node_modules")
        message(STATUS "Downloading llhttp npm dependencies")
        execute_process(COMMAND ${NPM_CMD} ci WORKING_DIRECTORY ${LLHTTP_DIR} OUTPUT_QUIET ERROR_VARIABLE NPM_CI_ERROR)
    endif()

    # Generate the library
    message(STATUS "Running llhttp generation script")
    execute_process(COMMAND ${NPM_CMD} run build WORKING_DIRECTORY ${LLHTTP_DIR} OUTPUT_QUIET ERROR_VARIABLE NPM_RUN_BUILD_ERROR)

    if (NOT ${NPM_RUN_BUILD_ERROR} STREQUAL "")
        message(FATAL_ERROR "Problem with llhttp generation:\n${NPM_CI_ERROR}\n${NPM_RUN_BUILD_ERROR}")
    endif()
    message(STATUS "Finished llhttp library generation")
endif()

set(LLHTTP_SOURCES
    "Dependencies/llhttp/build/c/llhttp.c"
    "Dependencies/llhttp/src/native/api.c"
    "Dependencies/llhttp/src/native/http.c")
add_library(llhttp ${LLHTTP_SOURCES})
target_include_directories(llhttp PUBLIC "Dependencies/llhttp/build")
target_include_directories(llhttp PUBLIC "Dependencies/llhttp/src/native")
source_group(TREE ${LLHTTP_DIR} FILES ${LLHTTP_SOURCES})
set_property(TARGET llhttp PROPERTY FOLDER Dependencies)

# --------------------------- v8 inspector ---------------------------
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

target_include_directories(v8inspector PUBLIC "Include")
target_include_directories(v8inspector PRIVATE "Source")

target_compile_definitions(v8inspector PRIVATE ASIO_STANDALONE)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_link_to_dependencies(v8inspector PRIVATE asio)
target_link_to_dependencies(v8inspector PRIVATE llhttp)
target_link_to_dependencies(v8inspector PRIVATE javascript_engine)
disable_warnings(v8inspector)