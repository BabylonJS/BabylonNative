# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( TARGET meshoptimizer )
	return()
endif()

file( GLOB MESHOPTIMIZER_SOURCES ${BGFX_DIR}/3rdparty/meshoptimizer/src/*.cpp ${BGFX_DIR}/3rdparty/meshoptimizer/src/*.h )

add_library( meshoptimizer STATIC ${MESHOPTIMIZER_SOURCES} )
target_include_directories( meshoptimizer PUBLIC ${BGFX_DIR}/3rdparty )
set_target_properties( meshoptimizer PROPERTIES FOLDER "bgfx/3rdparty" )
