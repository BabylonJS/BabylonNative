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
	message(SEND_ERROR "Could not load bimg_encode, directory does not exist. ${BIMG_DIR}")
	return()
endif()

add_library(bimg_encode STATIC)

# Put in a "bgfx" folder in Visual Studio
set_target_properties(bimg_encode PROPERTIES FOLDER "bgfx")

target_include_directories(
	bimg_encode
	PUBLIC $<BUILD_INTERFACE:${BIMG_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	PRIVATE ${BIMG_DIR}/3rdparty #
			${BIMG_DIR}/3rdparty/astc-encoder/include #
			${BIMG_DIR}/3rdparty/iqa/include #
			${BIMG_DIR}/3rdparty/nvtt #
			${MINIZ_INCLUDE_DIR} #
)

file(
	GLOB_RECURSE
	BIMG_ENCODE_SOURCES
	${BIMG_DIR}/include/* #
	${BIMG_DIR}/src/image_encode.* #
	${BIMG_DIR}/src/image_cubemap_filter.* #
	${BIMG_DIR}/3rdparty/libsquish/**.cpp #
	${BIMG_DIR}/3rdparty/libsquish/**.h #
	${BIMG_DIR}/3rdparty/edtaa3/**.cpp #
	${BIMG_DIR}/3rdparty/edtaa3/**.h #
	${BIMG_DIR}/3rdparty/etc1/**.cpp #
	${BIMG_DIR}/3rdparty/etc1/**.h #
	${BIMG_DIR}/3rdparty/etc2/**.cpp #
	${BIMG_DIR}/3rdparty/etc2/**.hpp #
	${BIMG_DIR}/3rdparty/nvtt/**.cpp #
	${BIMG_DIR}/3rdparty/nvtt/**.h #
	${BIMG_DIR}/3rdparty/pvrtc/**.cpp #
	${BIMG_DIR}/3rdparty/pvrtc/**.h #
	${BIMG_DIR}/3rdparty/tinyexr/**.h #
	${BIMG_DIR}/3rdparty/iqa/include/**.h #
	${BIMG_DIR}/3rdparty/iqa/source/**.c #
)

target_sources(bimg_encode PRIVATE ${BIMG_ENCODE_SOURCES})

target_link_libraries(bimg_encode PUBLIC bx)

include(CheckCXXCompilerFlag)
foreach(flag "-Wno-implicit-fallthrough" "-Wno-shadow" "-Wno-shift-negative-value" "-Wno-undef")
	check_cxx_compiler_flag(${flag} flag_supported)
	if(flag_supported)
		target_compile_options(bimg_encode PRIVATE ${flag})
	endif()
endforeach()

foreach(flag "-Wno-class-memaccess" "-Wno-deprecated-copy")
	check_cxx_compiler_flag(${flag} flag_supported)
	if(flag_supported)
		foreach(file ${BIMG_ENCODE_SOURCES})
			get_source_file_property(lang ${file} LANGUAGE)
			if(lang STREQUAL "CXX")
				set_source_files_properties(${file} PROPERTIES COMPILE_OPTIONS ${flag})
			endif()
		endforeach()
	endif()
endforeach()
