# bgfx versioning scheme:
# bgfx 1.104.7082
#      ^ ^^^ ^^^^
#      | |   +--- Commit number  (https://github.com/bkaradzic/bgfx / git rev-list --count HEAD)
#      | +------- API version    (from https://github.com/bkaradzic/bgfx/blob/master/scripts/bgfx.idl#L4)
#      +--------- Major revision (always 1)

# BGFX_API_VERSION generated from https://github.com/bkaradzic/bgfx/blob/master/scripts/bgfx.idl#L4
# bgfx/src/version.h:
# BGFX_REV_NUMBER
# BGFX_REV_SHA1

find_package(Git QUIET)

execute_process(COMMAND "${GIT_EXECUTABLE}" -C bgfx log --pretty=format:'%h' -n 1
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_REV
                ERROR_QUIET)

execute_process(COMMAND "${GIT_EXECUTABLE}" -C bgfx rev-list --count HEAD
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_REV_COUNT
                ERROR_QUIET)

# read version(100) from bgfx.idl
file(READ "${BGFX_DIR}/scripts/bgfx.idl" BGFX_IDL)
string(REGEX MATCH "version\\(([^\)]+)\\)"
       BGFX_API_VERSION ${BGFX_IDL})
set(BGFX_API_VERSION ${CMAKE_MATCH_1})
set(BGFX_REV_NUMBER ${GIT_REV_COUNT})
set(BGFX_REV ${GIT_REV})

# set project specific versions
set(PROJECT_VERSION 1.${BGFX_API_VERSION}.${BGFX_REV_NUMBER})
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR ${BGFX_API_VERSION})
set(PROJECT_VERSION_PATCH ${BGFX_REV_NUMBER})
