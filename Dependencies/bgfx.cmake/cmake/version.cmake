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

set(GIT_REV_COUNT "")
if(GIT_FOUND AND EXISTS "${BGFX_DIR}/.git")
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" -C "${BGFX_DIR}" rev-parse --is-shallow-repository
		OUTPUT_VARIABLE BGFX_SHALLOW
		OUTPUT_STRIP_TRAILING_WHITESPACE
		RESULT_VARIABLE BGFX_GIT_RESULT
	)
	if(BGFX_GIT_RESULT EQUAL 0 AND BGFX_SHALLOW STREQUAL "false")
		execute_process(
			COMMAND "${GIT_EXECUTABLE}" -C "${BGFX_DIR}" rev-parse --short HEAD
			OUTPUT_VARIABLE GIT_REV
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		execute_process(
			COMMAND "${GIT_EXECUTABLE}" -C "${BGFX_DIR}" rev-list --count HEAD
			OUTPUT_VARIABLE GIT_REV_COUNT
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	endif()
	if(NOT GIT_REV_COUNT MATCHES "^[0-9]+$" OR NOT GIT_REV)
		set(GIT_REV_COUNT "")
		message(WARNING "Cannot count bgfx revisions in ${BGFX_DIR} (shallow clone?); using ${BGFX_DIR}/src/version.h")
	endif()
endif()

if(NOT GIT_REV_COUNT)
	file(READ "${BGFX_DIR}/src/version.h" BGFX_VERSION_HEADER)
	string(REGEX MATCH "#define BGFX_REV_NUMBER[ \t]+([0-9]+)" _ "${BGFX_VERSION_HEADER}")
	set(GIT_REV_COUNT "${CMAKE_MATCH_1}")
	if(NOT GIT_REV_COUNT)
		message(FATAL_ERROR "Cannot find BGFX_REV_NUMBER in ${BGFX_DIR}/src/version.h")
	endif()
	string(REGEX MATCH "#define BGFX_REV_SHA1[ \t]+\"([0-9a-f]+)\"" _ "${BGFX_VERSION_HEADER}")
	string(SUBSTRING "${CMAKE_MATCH_1}" 0 7 GIT_REV)
endif()

# read version(100) from bgfx.idl
file(READ "${BGFX_DIR}/scripts/bgfx.idl" BGFX_IDL)
string(REGEX MATCH "version\\(([^\)]+)\\)" BGFX_API_VERSION ${BGFX_IDL})
set(BGFX_API_VERSION ${CMAKE_MATCH_1})
if(NOT BGFX_API_VERSION MATCHES "^[0-9]+$")
	message(FATAL_ERROR "Cannot read bgfx API version from ${BGFX_DIR}/scripts/bgfx.idl")
endif()
set(BGFX_REV_NUMBER ${GIT_REV_COUNT})
set(BGFX_REV ${GIT_REV})

# set project specific versions
set(PROJECT_VERSION 1.${BGFX_API_VERSION}.${BGFX_REV_NUMBER})
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR ${BGFX_API_VERSION})
set(PROJECT_VERSION_PATCH ${BGFX_REV_NUMBER})
