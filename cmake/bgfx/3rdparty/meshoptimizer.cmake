# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

# Ensure the directory exists
if(NOT IS_DIRECTORY ${BGFX_DIR})
	message(SEND_ERROR "Could not load bgfx, directory does not exist. ${BGFX_DIR}")
	return()
endif()

if(NOT MESHOPTIMIZER_LIBRARIES)
	file(
		GLOB #
		MESHOPTIMIZER_SOURCES #
		${BGFX_DIR}/3rdparty/meshoptimizer/src/*.cpp #
		${BGFX_DIR}/3rdparty/meshoptimizer/src/*.h #
	)
	set(MESHOPTIMIZER_INCLUDE_DIR ${BGFX_DIR}/3rdparty)
endif()
