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

file(
	GLOB_RECURSE
	BIMG_DECODE_SOURCES #
	${BIMG_DIR}/include/* #
	${BIMG_DIR}/src/image_decode*.* #
	#
	${LOADPNG_SOURCES} #
)

add_library(bimg_decode STATIC ${BIMG_DECODE_SOURCES})

# Put in a "bgfx" folder in Visual Studio
set_target_properties(bimg_decode PROPERTIES FOLDER "bgfx")
target_include_directories(
	bimg_decode
	PUBLIC $<BUILD_INTERFACE:${BIMG_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	PRIVATE ${LOADPNG_INCLUDE_DIR} #
			${MINIZ_INCLUDE_DIR} #
			${TINYEXR_INCLUDE_DIR} #
)

target_link_libraries(
	bimg_decode
	PUBLIC bx #
		   ${LOADPNG_LIBRARIES} #
		   ${MINIZ_LIBRARIES} #
		   ${TINYEXR_LIBRARIES} #
)

# AVIF decode support via the dav1d codec. Mirrors bimg's GENie build
# (bimg/scripts/bimg_decode.lua), which enables AVIF unconditionally.
target_compile_definitions(bimg_decode PRIVATE AVIF_CODEC_DAV1D)

target_sources(
	bimg_decode
	PRIVATE ${BIMG_DIR}/3rdparty/dav1d/dav1d-amalgamated.c #
			${BIMG_DIR}/3rdparty/dav1d/dav1d-bitdepth-8.c #
			${BIMG_DIR}/3rdparty/dav1d/dav1d-bitdepth-16.c #
			${BIMG_DIR}/3rdparty/libavif/libavif-amalgamated.c #
)

target_include_directories(
	bimg_decode
	PRIVATE ${BIMG_DIR}/3rdparty #
			${BIMG_DIR}/3rdparty/libavif #
			${BIMG_DIR}/3rdparty/libavif/include #
			${BIMG_DIR}/3rdparty/libavif/third_party/libyuv/include #
			${BIMG_DIR}/3rdparty/dav1d #
			${BIMG_DIR}/3rdparty/dav1d/include #
)

if(MSVC)
	target_include_directories(bimg_decode PRIVATE ${BIMG_DIR}/3rdparty/dav1d/include/compat/msvc)
endif()

# dav1d requires C11.
set_target_properties(bimg_decode PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES)

if(BGFX_INSTALL AND NOT BGFX_LIBRARY_TYPE MATCHES "SHARED")
	install(
		TARGETS bimg_decode
		EXPORT "${TARGETS_EXPORT_NAME}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		INCLUDES
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	)
endif()
