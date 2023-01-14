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

add_library(bimg STATIC)

# Put in a "bgfx" folder in Visual Studio
set_target_properties(bimg PROPERTIES FOLDER "bgfx")

target_include_directories(
	bimg PUBLIC $<BUILD_INTERFACE:${BIMG_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	PRIVATE ${ASTC_ENCODER_INCLUDE_DIR} #
			${MINIZ_INCLUDE_DIR} #
)

file(
	GLOB_RECURSE
	BIMG_SOURCES
	${BIMG_DIR}/include/* #
	${BIMG_DIR}/src/image.* #
	${BIMG_DIR}/src/image_gnf.cpp #
	#
	${ASTC_ENCODER_SOURCES}
	${MINIZ_SOURCES}
)

target_sources(bimg PRIVATE ${BIMG_SOURCES})

target_link_libraries(
	bimg
	PUBLIC bx #
		   ${ASTC_ENCODER_LIBRARIES} #
		   ${MINIZ_LIBRARIES} #
)
