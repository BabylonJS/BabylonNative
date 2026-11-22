if(JSR_NODE_API_BUILD_NATIVE_TESTS)
  set(NODE_API_TEST_INDEX 0)
  foreach(NODE_API_TEST_DIR ${JSR_NODE_API_NATIVE_TEST_DIRS})
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${NODE_API_TEST_DIR}/CMakeLists.txt)
      # ccache with CCACHE_BASEDIR hands cl.exe source paths relative to the build directory, and
      # cl.exe cannot open the joined path once it exceeds MAX_PATH. Short numbered build
      # directories keep the deepest addons under the limit.
      add_subdirectory(${NODE_API_TEST_DIR} ${NODE_API_TEST_BINARY_ROOT}/addons/${NODE_API_TEST_INDEX})
      math(EXPR NODE_API_TEST_INDEX "${NODE_API_TEST_INDEX} + 1")
    endif()
  endforeach()
endif()
