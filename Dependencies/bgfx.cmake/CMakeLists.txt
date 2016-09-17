# bgfx.cmake - bgfx building in cmake
# Written in 2016 by Joshua Brookover <josh@jalb.me>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

cmake_minimum_required( VERSION 3.0 )
project( bgfx )

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
if( APPLE )
	set( CMAKE_CXX_FLAGS "-ObjC++" )
endif()

option( BGFX_BUILD_TOOLS    "Build bgfx tools."    ON )
option( BGFX_BUILD_EXAMPLES "Build bgfx examples." ON )

option( BGFX_INSTALL "Create installation target." ON )

option( BGFX_USE_OVR "Build with OVR support." OFF )

if( BGFX_USE_OVR )
	include( cmake/ovr.cmake )
endif()

include( cmake/shared.cmake )
include( cmake/bx.cmake )
include( cmake/bgfx.cmake )

if( BGFX_BUILD_TOOLS )
	include( cmake/tools.cmake )
endif()

if( BGFX_BUILD_EXAMPLES )
	include( cmake/examples.cmake )
endif()

if( BGFX_INSTALL )
	# install bx
	install( DIRECTORY ${BX_DIR}/include DESTINATION . )

	# install bgfx
	install( TARGETS bgfx DESTINATION lib )
	install( DIRECTORY ${BGFX_DIR}/include DESTINATION . )

	# install tools
	if( BGFX_BUILD_TOOLS )
		install( TARGETS shaderc DESTINATION bin )
		install( TARGETS geometryc DESTINATION bin )
	endif()
endif()
