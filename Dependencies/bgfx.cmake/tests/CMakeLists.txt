cmake_minimum_required(VERSION 3.21)
project(bgfx_cmake_contracts NONE)
enable_testing()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(BGFX_TEST_SOURCE_ROOT "" CACHE PATH "Already fetched directory containing bgfx, bx and bimg")
if(NOT BGFX_TEST_SOURCE_ROOT)
	message(FATAL_ERROR "Set BGFX_TEST_SOURCE_ROOT to the build's _deps/bgfx.cmake-src directory.")
endif()
foreach(contract managed roots integration package options install)
	add_test(NAME bgfx.${contract} COMMAND "${Python3_EXECUTABLE}"
		"${CMAKE_CURRENT_SOURCE_DIR}/contracts.py" "${contract}"
		--sources "${BGFX_TEST_SOURCE_ROOT}"
		--work "${CMAKE_CURRENT_BINARY_DIR}/${contract}"
		--cmake "${CMAKE_COMMAND}" --generator "${CMAKE_GENERATOR}"
		--platform "${CMAKE_GENERATOR_PLATFORM}")
	set_tests_properties(bgfx.${contract} PROPERTIES TIMEOUT 1800)
endforeach()
set(BGFX_TEST_NATIVE_SOURCE_ROOT "" CACHE PATH "Optional BabylonNative source-tree build's _deps")
if(BGFX_TEST_NATIVE_SOURCE_ROOT)
	add_test(NAME bgfx.native-package COMMAND "${Python3_EXECUTABLE}"
		"${CMAKE_CURRENT_SOURCE_DIR}/contracts.py" native-package
		--sources "${BGFX_TEST_SOURCE_ROOT}" --native-sources "${BGFX_TEST_NATIVE_SOURCE_ROOT}"
		--work "${CMAKE_CURRENT_BINARY_DIR}/native-package"
		--cmake "${CMAKE_COMMAND}" --generator "${CMAKE_GENERATOR}"
		--platform "${CMAKE_GENERATOR_PLATFORM}")
	set_tests_properties(bgfx.native-package PROPERTIES TIMEOUT 1800)
endif()
