set(NODE_API_TEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

option(JSR_NODE_API_BUILD_NATIVE_TESTS "Build Node-API native addon test modules" OFF)

if(JSR_NODE_API_BUILD_NATIVE_TESTS)
    set(JSR_NODE_API_NATIVE_TEST_DIRS
        2_function_arguments
        3_callbacks
        4_object_factory
        5_function_factory
    )
endif()

function(node_api_copy_test_sources TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${NODE_API_TEST_ROOT}/test
            $<TARGET_FILE_DIR:${TARGET_NAME}>/test
        COMMENT "Copying Node-API test assets for ${TARGET_NAME}"
    )
endfunction()

if(APPLE)
    set(NODE_LITE_PLATFORM_SRC node_lite_mac.cpp)
    set(NODE_LITE_CHILD_PROCESS_SRC child_process_mac.cpp)
elseif(WIN32)
    set(NODE_LITE_PLATFORM_SRC node_lite_windows.cpp)
    set(NODE_LITE_CHILD_PROCESS_SRC child_process.cpp)
else()
    set(NODE_LITE_PLATFORM_SRC node_lite_mac.cpp)
    set(NODE_LITE_CHILD_PROCESS_SRC child_process_mac.cpp)
    message(WARNING "Node-API node_lite platform not yet customized for ${CMAKE_SYSTEM_NAME}; using POSIX defaults.")
endif()

add_executable(node_lite
    ${NODE_LITE_CHILD_PROCESS_SRC}
    child_process.h
    compat.h
    js_runtime_api.cpp
    js_runtime_api.h
    node_lite.cpp
    node_lite.h
    node_lite_jsruntimehost.cpp
    ${NODE_LITE_PLATFORM_SRC}
    string_utils.cpp
    string_utils.h
)

target_include_directories(node_lite
    PRIVATE
        ${NODE_API_TEST_ROOT}
        ${NODE_API_TEST_ROOT}/include
        ${CMAKE_SOURCE_DIR}/Core/Node-API/Include/Shared
        ${CMAKE_SOURCE_DIR}/Core/Node-API/Include/Engine/${NAPI_JAVASCRIPT_ENGINE}
        ${CMAKE_SOURCE_DIR}/Core/Node-API/Source
)

target_compile_definitions(node_lite
    PRIVATE
        NODE_API_EXPERIMENTAL_NO_WARNING
)

target_link_libraries(node_lite
    PRIVATE
        napi
)

node_api_copy_test_sources(node_lite)

add_executable(NodeApiTests
    ${NODE_LITE_CHILD_PROCESS_SRC}
    child_process.h
    string_utils.cpp
    string_utils.h
    test_basics.cpp
    test_main.cpp
    test_main.h
)

target_include_directories(NodeApiTests
    PRIVATE
        ${NODE_API_TEST_ROOT}
        ${NODE_API_TEST_ROOT}/include
)

target_link_libraries(NodeApiTests
    PRIVATE
        gtest_main
)

node_api_copy_test_sources(NodeApiTests)

add_dependencies(NodeApiTests node_lite)

add_custom_target(NodeApiModules)

if(JSR_NODE_API_BUILD_NATIVE_TESTS)
    list(JOIN JSR_NODE_API_NATIVE_TEST_DIRS "," NODE_API_NATIVE_TESTS_STRING)
    target_compile_definitions(node_lite
        PRIVATE
            NODE_API_TESTS_HAVE_NATIVE_MODULES=1
            NODE_API_AVAILABLE_NATIVE_TESTS=\"${NODE_API_NATIVE_TESTS_STRING}\"
    )
    target_compile_definitions(NodeApiTests
        PRIVATE
            NODE_API_TESTS_HAVE_NATIVE_MODULES=1
            NODE_API_AVAILABLE_NATIVE_TESTS=\"${NODE_API_NATIVE_TESTS_STRING}\"
    )
endif()

function(add_node_api_module MODULE_TARGET)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "" "" "SOURCES;DEFINES")

    get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    if(NOT "${MODULE_TARGET}" STREQUAL "${FOLDER_NAME}")
        set(MODULE_TARGET "${FOLDER_NAME}_${MODULE_TARGET}")
    endif()

    add_library(${MODULE_TARGET} MODULE)
    target_sources(${MODULE_TARGET} PRIVATE ${ARG_SOURCES})
    target_include_directories(${MODULE_TARGET}
        PRIVATE
            ${NODE_API_TEST_ROOT}/include
            ${CMAKE_SOURCE_DIR}/Core/Node-API/Include/Shared
            ${CMAKE_SOURCE_DIR}/Core/Node-API/Include/Shared/napi
            ${CMAKE_SOURCE_DIR}/Core/Node-API/Include/Engine/${NAPI_JAVASCRIPT_ENGINE}
            ${CMAKE_SOURCE_DIR}/Core/Node-API/Include/Engine/${NAPI_JAVASCRIPT_ENGINE}/napi
    )

    target_compile_definitions(${MODULE_TARGET}
        PRIVATE
            NODE_API_EXPERIMENTAL_NO_WARNING
            NODE_GYP_MODULE_NAME=\"${FOLDER_NAME}\"
            ${ARG_DEFINES}
    )

    if(APPLE)
        target_link_options(${MODULE_TARGET}
            PRIVATE
                "-undefined" "dynamic_lookup"
        )
    endif()

    set(MODULE_OUTPUT_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/build/$<IF:$<CONFIG:Debug>,Debug,Release>)
    set_target_properties(${MODULE_TARGET}
        PROPERTIES
            PREFIX ""
            SUFFIX ".node"
            ARCHIVE_OUTPUT_DIRECTORY ${MODULE_OUTPUT_DIR}
            LIBRARY_OUTPUT_DIRECTORY ${MODULE_OUTPUT_DIR}
            RUNTIME_OUTPUT_DIRECTORY ${MODULE_OUTPUT_DIR}
    )

    add_dependencies(NodeApiModules ${MODULE_TARGET})

    add_custom_command(TARGET ${MODULE_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:node_lite>/test/js-native-api/${FOLDER_NAME}/build
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_BINARY_DIR}/build
            $<TARGET_FILE_DIR:node_lite>/test/js-native-api/${FOLDER_NAME}/build
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:NodeApiTests>/test/js-native-api/${FOLDER_NAME}/build
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_BINARY_DIR}/build
            $<TARGET_FILE_DIR:NodeApiTests>/test/js-native-api/${FOLDER_NAME}/build
        COMMENT "Copying Node-API module ${MODULE_TARGET} outputs"
    )
endfunction()

add_dependencies(NodeApiTests NodeApiModules)

add_subdirectory(test)
