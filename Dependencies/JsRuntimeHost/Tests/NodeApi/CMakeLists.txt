set(NODE_API_TEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

option(JSR_NODE_API_BUILD_NATIVE_TESTS "Build Node-API native addon test modules" ON)

set(JSR_NODE_API_NATIVE_TEST_DIRS
    2_function_arguments
    3_callbacks
    4_object_factory
    5_function_factory
    test_reference_double_free
)
if(NOT JSR_NODE_API_BUILD_NATIVE_TESTS)
    set(JSR_NODE_API_NATIVE_TEST_DIRS "")
endif()

function(node_api_copy_test_sources TARGET_NAME)
    add_custom_target(${TARGET_NAME}Assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${NODE_API_TEST_ROOT}/test
            $<TARGET_FILE_DIR:${TARGET_NAME}>/test
        COMMENT "Copying Node-API test assets for ${TARGET_NAME}"
    )
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}Assets)
endfunction()

if(NOT ANDROID)
    if(WIN32)
        set(NODE_LITE_PLATFORM_SRC node_lite_windows.cpp)
        set(NODE_LITE_CHILD_PROCESS_SRC child_process.cpp)
    else()
        set(NODE_LITE_PLATFORM_SRC node_lite_posix.cpp)
        set(NODE_LITE_CHILD_PROCESS_SRC child_process_posix.cpp)
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
        string_utils.h)

    target_include_directories(node_lite
        PRIVATE
            ${NODE_API_TEST_ROOT}
            ${NODE_API_TEST_ROOT}/include
            ${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Include/Shared
            ${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Include/Engine/${NAPI_JAVASCRIPT_ENGINE}
            ${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Source)

    string(TOUPPER "${NAPI_JAVASCRIPT_ENGINE}" NODE_API_ENGINE_UPPER)
    target_compile_definitions(node_lite
        PRIVATE
            JSR_NAPI_ENGINE_${NODE_API_ENGINE_UPPER}
            NODE_API_EXPERIMENTAL_NO_WARNING
            NODE_API_BUILD_TYPE="$<IF:$<CONFIG:Debug>,Debug,Release>")
    target_link_libraries(node_lite PRIVATE napi)

    get_target_property(NAPI_LIBRARY_TYPE napi TYPE)
    if(WIN32)
        if(NAPI_LIBRARY_TYPE STREQUAL "STATIC_LIBRARY")
            set_target_properties(node_lite PROPERTIES ENABLE_EXPORTS ON)
            target_link_options(node_lite PRIVATE "/WHOLEARCHIVE:$<TARGET_FILE:napi>")
            if(NAPI_JAVASCRIPT_ENGINE STREQUAL "Hermes")
                target_link_options(node_lite PRIVATE "/WHOLEARCHIVE:$<TARGET_FILE:hermesNapi>")
            endif()
        endif()
        add_custom_command(TARGET node_lite POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E
                $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:node_lite>>,copy,true>
                $<TARGET_RUNTIME_DLLS:node_lite>
                $<TARGET_FILE_DIR:node_lite>
            COMMAND_EXPAND_LISTS)
    elseif(UNIX AND NOT APPLE)
        set_target_properties(node_lite PROPERTIES ENABLE_EXPORTS ON)
        if(NAPI_LIBRARY_TYPE STREQUAL "STATIC_LIBRARY"
                AND NAPI_JAVASCRIPT_ENGINE STREQUAL "Hermes")
            target_link_options(node_lite
                PRIVATE
                    "LINKER:--whole-archive"
                    "$<TARGET_FILE:hermesNapi>"
                    "LINKER:--no-whole-archive")
        endif()
    endif()

    node_api_copy_test_sources(node_lite)
    add_executable(NodeApiTests
        ${NODE_LITE_CHILD_PROCESS_SRC}
        child_process.h
        string_utils.cpp
        string_utils.h
        test_basics.cpp
        test_main.cpp
        main.cpp
        test_main.h)
    target_include_directories(NodeApiTests
        PRIVATE
            ${NODE_API_TEST_ROOT}
            ${NODE_API_TEST_ROOT}/include)
    target_link_libraries(NodeApiTests PRIVATE gtest_main)
    add_dependencies(NodeApiTests node_lite)

    add_custom_target(NodeApiModules)
    list(JOIN JSR_NODE_API_NATIVE_TEST_DIRS "," NODE_API_NATIVE_TESTS_STRING)
    foreach(TARGET_NAME node_lite NodeApiTests)
        target_compile_definitions(${TARGET_NAME}
            PRIVATE NODE_API_AVAILABLE_NATIVE_TESTS="${NODE_API_NATIVE_TESTS_STRING}")
        if(JSR_NODE_API_BUILD_NATIVE_TESTS)
            target_compile_definitions(${TARGET_NAME} PRIVATE NODE_API_TESTS_HAVE_NATIVE_MODULES=1)
        endif()
    endforeach()
else()
    # Object files retain the statically registered Google Test basics.
    add_library(NodeApiTestHarness OBJECT
        android_tests.cpp
        child_process_android.cpp
        js_runtime_api.cpp
        node_lite.cpp
        node_lite_android.cpp
        node_lite_jsruntimehost.cpp
        string_utils.cpp
        test_basics.cpp
        test_main.cpp)
    target_include_directories(NodeApiTestHarness
        PUBLIC "${NODE_API_TEST_ROOT}"
        PRIVATE "${NODE_API_TEST_ROOT}/include"
            "${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Source")
    string(TOUPPER "${NAPI_JAVASCRIPT_ENGINE}" NODE_API_ENGINE_UPPER)
    list(JOIN JSR_NODE_API_NATIVE_TEST_DIRS "," NODE_API_NATIVE_TESTS_STRING)
    target_compile_definitions(NodeApiTestHarness
        PUBLIC NODE_API_AVAILABLE_NATIVE_TESTS="${NODE_API_NATIVE_TESTS_STRING}"
        PRIVATE JSR_NAPI_ENGINE_${NODE_API_ENGINE_UPPER}
            NODE_API_BUILD_TYPE="$<IF:$<CONFIG:Debug>,Debug,Release>"
            NODE_API_EXPERIMENTAL_NO_WARNING)
    if(JSR_NODE_API_BUILD_NATIVE_TESTS)
        target_compile_definitions(NodeApiTestHarness PRIVATE NODE_API_TESTS_HAVE_NATIVE_MODULES=1)
    endif()
    target_link_libraries(NodeApiTestHarness PRIVATE napi AppRuntime gtest android log dl)
    add_custom_target(NodeApiModules)
endif()

function(add_node_api_module MODULE_TARGET)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "" "" "SOURCES;DEFINES")

    get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    if(NOT "${MODULE_TARGET}" STREQUAL "${FOLDER_NAME}")
        set(MODULE_TARGET "${FOLDER_NAME}_${MODULE_TARGET}")
    endif()

    # On Android the addon is a SHARED lib<name>.so so AGP packages it into the APK's lib/<abi>/ (the
    # app's nativeLibraryDir, the only place a native library may be dlopen'd from on API 29+).
    # Elsewhere it is a MODULE (dlopen-only) .node loaded by the node_lite child-process runner.
    if(ANDROID)
        add_library(${MODULE_TARGET} SHARED)
    else()
        add_library(${MODULE_TARGET} MODULE)
    endif()
    target_sources(${MODULE_TARGET} PRIVATE ${ARG_SOURCES})
    target_include_directories(${MODULE_TARGET}
        PRIVATE
            ${NODE_API_TEST_ROOT}/include
            ${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Include/Shared
            ${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Include/Shared/napi
            ${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Include/Engine/${NAPI_JAVASCRIPT_ENGINE}
            ${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Include/Engine/${NAPI_JAVASCRIPT_ENGINE}/napi
    )

    target_compile_definitions(${MODULE_TARGET}
        PRIVATE
            NODE_API_EXPERIMENTAL_NO_WARNING
            NODE_GYP_MODULE_NAME=\"${FOLDER_NAME}\"
            ${ARG_DEFINES}
    )

    if(APPLE)
        # The addon is dlopen'd into node_lite, which already provides the napi_* symbols; let them
        # remain unresolved at link time.
        target_link_options(${MODULE_TARGET}
            PRIVATE
                "-undefined" "dynamic_lookup"
        )
    elseif(ANDROID)
        # Link the shared napi (libnapi.so) so the addon's napi_* imports resolve at dlopen via a real
        # DT_NEEDED -- the same libnapi.so the host depends on, so there is a single napi instance.
        target_link_libraries(${MODULE_TARGET} PRIVATE napi)
    elseif(WIN32)
        target_compile_definitions(${MODULE_TARGET}
            PRIVATE
                "NAPI_EXTERN=__declspec(dllimport)"
        )
        if(NAPI_LIBRARY_TYPE STREQUAL "SHARED_LIBRARY")
            target_link_libraries(${MODULE_TARGET} PRIVATE napi)
        else()
            target_link_libraries(${MODULE_TARGET} PRIVATE node_lite)
        endif()
    endif()

    if(ANDROID)
        set_target_properties(${MODULE_TARGET}
            PROPERTIES
                PREFIX "lib"
                SUFFIX ".so"
        )
    else()
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
    endif()

    add_dependencies(NodeApiModules ${MODULE_TARGET})

    if(NOT ANDROID)
        # Stage the built .node next to the node_lite / NodeApiTests runners so they can dlopen it
        # relative to the copied test files.
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
    endif()

    if(APPLE)
        # The copy_directory above runs as an Xcode "Run Script" phase, which executes BEFORE
        # Xcode's implicit CodeSign phase signs the original module. The copied .node files (the
        # ones node_lite/NodeApiTests actually dlopen) are therefore unsigned on a clean build, and
        # macOS refuses to load them ("Trying to load an unsigned library"). Ad-hoc sign the copies
        # directly so clean builds work without a second pass.
        set(_node_api_cfg_dir $<IF:$<CONFIG:Debug>,Debug,Release>)
        add_custom_command(TARGET ${MODULE_TARGET} POST_BUILD
            COMMAND codesign --force --sign -
                "$<TARGET_FILE_DIR:node_lite>/test/js-native-api/${FOLDER_NAME}/build/${_node_api_cfg_dir}/$<TARGET_FILE_NAME:${MODULE_TARGET}>"
            COMMAND codesign --force --sign -
                "$<TARGET_FILE_DIR:NodeApiTests>/test/js-native-api/${FOLDER_NAME}/build/${_node_api_cfg_dir}/$<TARGET_FILE_NAME:${MODULE_TARGET}>"
            COMMENT "Ad-hoc signing copied ${MODULE_TARGET}.node for dlopen on macOS"
            VERBATIM
        )
    endif()
endfunction()

if(ANDROID)
    add_dependencies(NodeApiTestHarness NodeApiModules)
    if(JSR_NODE_API_BUILD_NATIVE_TESTS)
        add_subdirectory(test/js-native-api)
    endif()
else()
    add_dependencies(NodeApiTests NodeApiModules)
    add_subdirectory(test)
endif()
