# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

cmake_minimum_required(VERSION 3.20) # C++20 support required
project(bgfx)
cmake_policy(SET CMP0054 NEW)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Makefile generators on apple need this flag to compile mixed objective/c++
if(APPLE AND NOT XCODE)
	set(CMAKE_CXX_FLAGS "-ObjC++")
endif()

include(CMakeDependentOption)

option(BX_AMALGAMATED "Amalgamated bx build for faster compilation" OFF)
option(BX_CONFIG_DEBUG "Log debug messages (default: on in debug)" OFF)
option(BGFX_AMALGAMATED "Amalgamated bgfx build for faster compilation" OFF)
option(BGFX_BUILD_TOOLS "Build bgfx tools." ON)
cmake_dependent_option(BGFX_BUILD_TOOLS_BIN2C "Build bx binary to c converter." ON BGFX_BUILD_TOOLS OFF)
cmake_dependent_option(BGFX_BUILD_TOOLS_SHADER "Build bgfx shader tools." ON BGFX_BUILD_TOOLS OFF)
cmake_dependent_option(BGFX_BUILD_TOOLS_GEOMETRY "Build bgfx geometry tools." ON BGFX_BUILD_TOOLS OFF)
cmake_dependent_option(BGFX_BUILD_TOOLS_TEXTURE "Build bgfx texture tools." ON BGFX_BUILD_TOOLS OFF)
set(BGFX_TOOLS_PREFIX "" CACHE STRING "Prefix name to add to name of tools (to avoid clashes)")
option(BGFX_BUILD_EXAMPLES "Build bgfx examples." ON)
cmake_dependent_option(
	BGFX_BUILD_EXAMPLE_COMMON "Build bgfx example-common project." OFF "NOT BGFX_BUILD_EXAMPLES;NOT BGFX_BUILD_TOOLS"
	ON
)
option(BGFX_BUILD_TESTS "Build bgfx tests." OFF)
option(BGFX_INSTALL "Create installation target." ON)
cmake_dependent_option(
	BGFX_INSTALL_EXAMPLES "Install examples and their runtimes." OFF "BGFX_INSTALL;BGFX_BUILD_EXAMPLES" OFF
)
cmake_dependent_option(BGFX_WITH_WAYLAND "Use Wayland backend." ON "CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)
option(BGFX_CUSTOM_TARGETS "Include convenience custom targets." ON)
cmake_dependent_option(BGFX_CONFIG_MULTITHREADED "Build bgfx with multithreaded configuration" ON "NOT CMAKE_SYSTEM_NAME STREQUAL Emscripten" OFF)
option(BGFX_CONFIG_RENDERER_WEBGPU "Enable the webgpu renderer" OFF)
option(BGFX_CONFIG_DEBUG_ANNOTATION "Enable gfx debug annotations (default: on in debug)" OFF)

set(BGFX_OPENGL_VERSION "" CACHE STRING "Specify minimum opengl version")
set(BGFX_OPENGLES_VERSION "" CACHE STRING "Specify minimum OpenGL ES version")
set(BGFX_LIBRARY_TYPE "STATIC" CACHE STRING "Linking type for library")
set_property(CACHE BGFX_LIBRARY_TYPE PROPERTY STRINGS "STATIC" "SHARED")

# Declare a bgfx compile-time option and record it for forwarding to the bgfx target.
#
# An explicitly supplied value always wins. A normal variable set by a parent project is
# honoured rather than erased, matching how CMP0077 makes option() behave, and a cache
# entry from -D is left alone.
macro(bgfx_config_option NAME DOC)
	if(NOT DEFINED BGFX_CONFIG_${NAME})
		set(BGFX_CONFIG_${NAME} "" CACHE STRING "${DOC}")
	endif()
	list(APPEND BGFX_CONFIG_OPTIONS "BGFX_CONFIG_${NAME}")
endmacro()

# Forwarded separately because it is combined with BGFX_CONFIG_MULTITHREADED.
if(NOT DEFINED BGFX_CONFIG_DEFAULT_MAX_ENCODERS)
	set(BGFX_CONFIG_DEFAULT_MAX_ENCODERS "" CACHE STRING "Specify default maximum encoder count (multithreaded only)")
endif()

bgfx_config_option(MAX_DRAW_CALLS "Specify maximum draw calls")
bgfx_config_option(DRAW_CALL_BLOCK "Specify draw call allocation granularity")
bgfx_config_option(MAX_VIEWS "Specify maximum view count (should be a power of two)")
bgfx_config_option(MAX_FRAME_BUFFERS "Specify maximum frame buffers")
bgfx_config_option(MAX_VERTEX_LAYOUTS "Specify maximum vertex layouts")
bgfx_config_option(MAX_VERTEX_BUFFERS "Specify maximum vertex buffer count")
bgfx_config_option(MAX_VERTEX_STREAMS "Specify maximum vertex stream count")
bgfx_config_option(MAX_DYNAMIC_VERTEX_BUFFERS "Specify maximum dynamic vertex buffer count")
bgfx_config_option(MAX_INDEX_BUFFERS "Specify maximum index buffer count")
bgfx_config_option(MAX_DYNAMIC_INDEX_BUFFERS "Specify maximum dynamic index buffer count")
bgfx_config_option(MAX_TEXTURES "Specify maximum texture count")
bgfx_config_option(MAX_TEXTURE_SAMPLERS "Specify maximum texture samplers")
bgfx_config_option(MAX_SHADERS "Specify shader count")
bgfx_config_option(MAX_UNIFORMS "Specify maximum uniform count")
bgfx_config_option(MAX_OCCLUSION_QUERIES "Specify maximum occlusion query count")
bgfx_config_option(MAX_BLIT_ITEMS "Specify maximum blit item count")
bgfx_config_option(MAX_RECT_CACHE "Specify maximum scissor rect cache count")
bgfx_config_option(SORT_KEY_NUM_BITS_PROGRAM "Specify maximum program count as 2^(program bits)")
bgfx_config_option(MIN_RESOURCE_COMMAND_BUFFER_SIZE "Specify minimum resource command buffer size")
bgfx_config_option(MIN_UNIFORM_BUFFER_SIZE "Specify minimum uniform buffer size")
bgfx_config_option(UNIFORM_BUFFER_RESIZE_THRESHOLD_SIZE "Specify uniform buffer resize threshold size")
bgfx_config_option(UNIFORM_BUFFER_RESIZE_INCREMENT_SIZE "Specify uniform buffer resize increment size")
bgfx_config_option(DYNAMIC_VERTEX_BUFFER_SIZE "Specify dynamic vertex buffer block size in bytes")
bgfx_config_option(DYNAMIC_INDEX_BUFFER_SIZE "Specify dynamic index buffer block size in bytes")
bgfx_config_option(MAX_TRANSIENT_VERTEX_BUFFER_SIZE "Specify maximum transient vertex buffer size in bytes")
bgfx_config_option(MAX_TRANSIENT_INDEX_BUFFER_SIZE "Specify maximum transient index buffer size in bytes")
bgfx_config_option(DEBUG_UNIFORM "Specify whether uniform debug checks are enabled (0 or 1)")
bgfx_config_option(VIDEO "Specify whether video decoding support is built (0 or 1)")
bgfx_config_option(C99_API "Specify whether the C99 API is built (0 or 1)")

set(BGFX_CMAKE_USER_SCRIPT "" CACHE STRING "Path to custom cmake script to include")

if(BGFX_CMAKE_USER_SCRIPT)
	include(${BGFX_CMAKE_USER_SCRIPT})
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(BGFX_BUILD_TESTS)
	enable_testing()
endif()

if(NOT BX_DIR)
	set(BX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bx" CACHE STRING "Location of bx.")
elseif(NOT IS_ABSOLUTE "${BX_DIR}")
	get_filename_component(BX_DIR "${BX_DIR}" REALPATH BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
if(NOT BIMG_DIR)
	set(BIMG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bimg" CACHE STRING "Location of bimg.")
elseif(NOT IS_ABSOLUTE "${BIMG_DIR}")
	get_filename_component(BIMG_DIR "${BIMG_DIR}" REALPATH BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
if(NOT BGFX_DIR)
	set(BGFX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bgfx" CACHE STRING "Location of bgfx.")
elseif(NOT IS_ABSOLUTE "${BGFX_DIR}")
	get_filename_component(BGFX_DIR "${BGFX_DIR}" REALPATH BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

file(TO_CMAKE_PATH "${BX_DIR}" BX_DIR)
file(TO_CMAKE_PATH "${BIMG_DIR}" BIMG_DIR)
file(TO_CMAKE_PATH "${BGFX_DIR}" BGFX_DIR)

# sets project version from api ver / git rev
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.cmake)
if(BGFX_BUILD_TOOLS AND BGFX_CUSTOM_TARGETS)
	add_custom_target(tools)
	set_target_properties(tools PROPERTIES FOLDER "bgfx/tools")
endif()

if(BGFX_INSTALL)
	include(GNUInstallDirs)

	# Layout. This works for all platforms:
	#   * <prefix>/lib*/cmake/<PROJECT-NAME>
	#   * <prefix>/lib*/
	#   * <prefix>/include/
	set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
	set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

	# Configuration
	set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
	set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
	set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
	set(namespace "${PROJECT_NAME}::")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake/bx)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake/bimg)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake/bgfx)

if(BGFX_INSTALL)
	# Include module with fuction 'write_basic_package_version_file'
	include(CMakePackageConfigHelpers)

	# Configure '<PROJECT-NAME>ConfigVersion.cmake'
	# Use:
	#   * PROJECT_VERSION
	if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
		set(BGFX_COMPATIBILITY SameMajorVersion)
	else()
		set(BGFX_COMPATIBILITY SameMinorVersion)
	endif()
	write_basic_package_version_file("${version_config}" VERSION ${PROJECT_VERSION} COMPATIBILITY ${BGFX_COMPATIBILITY})

	# Configure '<PROJECT-NAME>Config.cmake'
	# Use variables:
	#   * TARGETS_EXPORT_NAME
	#   * PROJECT_NAME
	#   * BGFX_CMAKE_USER_SCRIPT_PRESENT
	#   * BGFX_CMAKE_USER_SCRIPT_INSTALL_NAME
	if(BGFX_CMAKE_USER_SCRIPT STREQUAL "")
		set(BGFX_CMAKE_USER_SCRIPT_PRESENT OFF)
		set(BGFX_CMAKE_USER_SCRIPT_INSTALL_NAME "NOT-USED")
	else()
		set(BGFX_CMAKE_USER_SCRIPT_PRESENT ON)
		get_filename_component(BGFX_CMAKE_USER_SCRIPT_INSTALL_NAME ${BGFX_CMAKE_USER_SCRIPT} NAME)
	endif()
	configure_package_config_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in" "${project_config}"
		INSTALL_DESTINATION "${config_install_dir}"
	)

	install(FILES ${BGFX_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/licences/${PROJECT_NAME})

	# install tools
	if(BGFX_CMAKE_USER_SCRIPT)
		install(FILES ${BGFX_CMAKE_USER_SCRIPT} DESTINATION "${config_install_dir}")
	endif()
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/bgfxToolUtils.cmake DESTINATION "${config_install_dir}")

	# Config
	#   * <prefix>/lib/cmake/bgfx/bgfxConfig.cmake
	#   * <prefix>/lib/cmake/bgfx/bgfxConfigVersion.cmake
	install(FILES "${project_config}" "${version_config}" DESTINATION "${config_install_dir}")

	# Config
	#   * <prefix>/lib/cmake/bgfx/bgfxTargets.cmake
	install(EXPORT "${TARGETS_EXPORT_NAME}" NAMESPACE "${namespace}" DESTINATION "${config_install_dir}")
endif()
