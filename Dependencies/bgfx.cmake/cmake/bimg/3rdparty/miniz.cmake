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
if(NOT IS_DIRECTORY ${BIMG_DIR})
	message(SEND_ERROR "Could not load bimg, directory does not exist. ${BIMG_DIR}")
	return()
endif()

if(NOT MINIZ_LIBRARIES)
	file(GLOB_RECURSE #
		 MINIZ_SOURCES #
		 ${BIMG_DIR}/3rdparty/tinyexr/deps/miniz/miniz.* #
	)
	set(MINIZ_INCLUDE_DIR ${BIMG_DIR}/3rdparty/tinyexr/deps/miniz)
endif()
