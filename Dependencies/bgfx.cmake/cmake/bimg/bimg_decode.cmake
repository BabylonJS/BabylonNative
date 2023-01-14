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
	message(SEND_ERROR "Could not load bimg_decode, directory does not exist. ${BIMG_DIR}")
	return()
endif()

add_library(bimg_decode STATIC)

# Put in a "bgfx" folder in Visual Studio
set_target_properties(bimg_decode PROPERTIES FOLDER "bgfx")
target_include_directories(
	bimg_decode
	PUBLIC $<BUILD_INTERFACE:${BIMG_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	PRIVATE ${LOADPNG_INCLUDE_DIR} #
			${MINIZ_INCLUDE_DIR} #
			${TINYEXR_INCLUDE_DIR} #
)

file(
	GLOB_RECURSE
	BIMG_DECODE_SOURCES #
	${BIMG_DIR}/include/* #
	${BIMG_DIR}/src/image_decode.* #
	#
	${LOADPNG_SOURCES} #
	${MINIZ_SOURCES} #
)

target_sources(bimg_decode PRIVATE ${BIMG_DECODE_SOURCES})

target_link_libraries(
	bimg_decode
	PUBLIC bx #
		   ${LOADPNG_LIBRARIES} #
		   ${MINIZ_LIBRARIES} #
		   ${TINYEXR_LIBRARIES} #
)
