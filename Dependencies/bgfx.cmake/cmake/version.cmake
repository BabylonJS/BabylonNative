# bgfx versioning scheme:
# bgfx 1.104.7082
#      ^ ^^^ ^^^^
#      | |   +--- Commit number  (https://github.com/bkaradzic/bgfx / git rev-list --count HEAD)
#      | +------- API version    (from https://github.com/bkaradzic/bgfx/blob/master/scripts/bgfx.idl#L4)
#      +--------- Major revision (always 1)
#
# BGFX_API_VERSION generated from https://github.com/bkaradzic/bgfx/blob/master/scripts/bgfx.idl#L4
# bgfx/src/version.h:
# BGFX_REV_NUMBER
# BGFX_REV_SHA1

find_package(Git QUIET)

file(READ "${BGFX_DIR}/scripts/bgfx.idl" BGFX_IDL)
string(REGEX MATCH "version\\(([0-9]+)\\)" _api "${BGFX_IDL}")
set(BGFX_API_VERSION "${CMAKE_MATCH_1}")
if(BGFX_API_VERSION STREQUAL "")
	message(FATAL_ERROR "Cannot read the bgfx API version from ${BGFX_DIR}/scripts/bgfx.idl")
endif()

unset(BGFX_REV_NUMBER)
unset(BGFX_REV)
if(Git_FOUND)
	execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${BGFX_DIR}" rev-parse --show-toplevel
		RESULT_VARIABLE _git_result OUTPUT_VARIABLE _git_root
		OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
	if(_git_result EQUAL 0)
		file(REAL_PATH "${_git_root}" _git_root)
		file(REAL_PATH "${BGFX_DIR}" _source_root)
		if(WIN32)
			string(TOLOWER "${_git_root}" _git_root)
			string(TOLOWER "${_source_root}" _source_root)
		endif()
		# An archive nested in BabylonNative must not inherit its ancestor's Git version.
		if(_git_root STREQUAL _source_root)
			execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${BGFX_DIR}" rev-list --count HEAD
				RESULT_VARIABLE _count_result OUTPUT_VARIABLE BGFX_REV_NUMBER
				OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
			execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${BGFX_DIR}" rev-parse HEAD
				RESULT_VARIABLE _rev_result OUTPUT_VARIABLE BGFX_REV
				OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
			if(NOT _count_result EQUAL 0 OR NOT _rev_result EQUAL 0)
				message(WARNING "Could not query the selected bgfx Git revision; using archive metadata.")
				unset(BGFX_REV_NUMBER)
				unset(BGFX_REV)
			endif()
		endif()
	endif()
endif()

if(NOT DEFINED BGFX_REV_NUMBER AND EXISTS "${BGFX_DIR}/.bgfx-cmake-version")
	file(READ "${BGFX_DIR}/.bgfx-cmake-version" _metadata)
	string(REGEX MATCH "api=([0-9]+)" _api "${_metadata}")
	if(NOT CMAKE_MATCH_1 STREQUAL BGFX_API_VERSION)
		message(FATAL_ERROR "Packaged bgfx version metadata does not match the selected API version.")
	endif()
	string(REGEX MATCH "revision=([0-9]+)" _revision "${_metadata}")
	set(BGFX_REV_NUMBER "${CMAKE_MATCH_1}")
	string(REGEX MATCH "sha=([0-9a-f]+)" _sha "${_metadata}")
	set(BGFX_REV "${CMAKE_MATCH_1}")
endif()

if(NOT DEFINED BGFX_REV_NUMBER AND EXISTS "${BGFX_DIR}/src/version.h")
	file(READ "${BGFX_DIR}/src/version.h" _version_header)
	string(REGEX MATCH "#define[ \t]+BGFX_REV_NUMBER[ \t]+([0-9]+)" _revision "${_version_header}")
	if(NOT CMAKE_MATCH_1 STREQUAL "")
		set(BGFX_REV_NUMBER "${CMAKE_MATCH_1}")
		string(REGEX MATCH "#define[ \t]+BGFX_REV_SHA1[ \t]+\"([0-9a-f]+)\"" _sha "${_version_header}")
		set(BGFX_REV "${CMAKE_MATCH_1}")
	endif()
endif()

if(NOT DEFINED BGFX_REV_NUMBER)
	message(WARNING "Selected bgfx sources have no Git or archive revision metadata; using revision 0.")
	set(BGFX_REV_NUMBER 0)
endif()
if(NOT BGFX_REV_NUMBER MATCHES "^[0-9]+$")
	message(FATAL_ERROR "Invalid selected bgfx revision number: '${BGFX_REV_NUMBER}'")
endif()

# set project specific versions
set(PROJECT_VERSION 1.${BGFX_API_VERSION}.${BGFX_REV_NUMBER})
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR ${BGFX_API_VERSION})
set(PROJECT_VERSION_PATCH ${BGFX_REV_NUMBER})
