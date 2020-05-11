# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET astc-codec )
	return()
endif()

file(
	GLOB
	ASTC_CODEC_SOURCES
	${BIMG_DIR}/3rdparty/astc-codec/src/decoder/*.cc
	${BIMG_DIR}/3rdparty/astc-codec/src/decoder/*.h
)

add_library( astc-codec STATIC ${ASTC_CODEC_SOURCES} )
target_include_directories( astc-codec
	PUBLIC
		$<BUILD_INTERFACE:${BIMG_DIR}/3rdparty>
		$<BUILD_INTERFACE:${BIMG_DIR}/3rdparty/astc-codec>
		$<BUILD_INTERFACE:${BIMG_DIR}/3rdparty/astc-codec/include> )
set_target_properties( astc-codec PROPERTIES FOLDER "bgfx/3rdparty" )