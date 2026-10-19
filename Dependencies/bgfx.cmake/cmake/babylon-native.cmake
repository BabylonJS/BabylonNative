include(FetchContent)

# Match FetchContent's cache typing and command-line relative-path normalization.
set(FETCHCONTENT_SOURCE_DIR_BGFX.CMAKE "${FETCHCONTENT_SOURCE_DIR_BGFX.CMAKE}"
	CACHE PATH "Override source directory for bgfx.cmake")
get_filename_component(_bgfx_tracked_wrapper "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(bgfx.cmake_SOURCE_DIR "${_bgfx_tracked_wrapper}")
if(NOT "${FETCHCONTENT_SOURCE_DIR_BGFX.CMAKE}" STREQUAL "")
	get_filename_component(bgfx.cmake_SOURCE_DIR "${FETCHCONTENT_SOURCE_DIR_BGFX.CMAKE}"
		ABSOLUTE)
endif()
if(NOT EXISTS "${bgfx.cmake_SOURCE_DIR}/CMakeLists.txt")
	message(FATAL_ERROR "FETCHCONTENT_SOURCE_DIR_BGFX.CMAKE is not a local wrapper: ${bgfx.cmake_SOURCE_DIR}")
endif()
set(bgfx.cmake_BINARY_DIR "${FETCHCONTENT_BASE_DIR}/bgfx.cmake-build")
add_subdirectory("${bgfx.cmake_SOURCE_DIR}" "${bgfx.cmake_BINARY_DIR}" EXCLUDE_FROM_ALL)

if(BABYLON_NATIVE_BUILD_SOURCETREE)
	include("${_bgfx_tracked_wrapper}/cmake/stage-sources.cmake")
	bgfx_stage_sources("${bgfx.cmake_SOURCE_DIR}" "${FETCHCONTENT_BASE_DIR}/bgfx.cmake-src")
endif()

foreach(_bgfx_variable BGFX_DIR BX_DIR BIMG_DIR bgfx.cmake_SOURCE_DIR bgfx.cmake_BINARY_DIR BGFX_PROJECT_VERSION)
	set(${_bgfx_variable} "${${_bgfx_variable}}" PARENT_SCOPE)
endforeach()
