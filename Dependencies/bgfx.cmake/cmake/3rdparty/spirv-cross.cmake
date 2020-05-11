# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET spirv-cross )
	return()
endif()

file(
	GLOB
	SPIRV_CROSS_SOURCES
	${BGFX_DIR}/3rdparty/spirv-cross/*.cpp
	${BGFX_DIR}/3rdparty/spirv-cross/*.h
)

add_library( spirv-cross STATIC ${SPIRV_CROSS_SOURCES} )
target_compile_definitions( spirv-cross PRIVATE SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS )
target_include_directories( spirv-cross PUBLIC
	${BGFX_DIR}/3rdparty/spirv-cross
	${BGFX_DIR}/3rdparty/spirv-cross/include
)

if( MSVC )
	target_compile_options( spirv-cross PRIVATE
		"/wd4018"
		"/wd4245"
		"/wd4706"
		"/wd4715"
	)
endif()

set_target_properties( spirv-cross PROPERTIES FOLDER "bgfx/3rdparty" )
