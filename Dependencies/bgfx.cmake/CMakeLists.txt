# Copyright (c) 2016 Joshua Brookover

cmake_minimum_required( VERSION 3.0 )
project( bgfx )

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
if( APPLE )
	set( CMAKE_CXX_FLAGS "-ObjC++" )
endif()

option( BGFX_BUILD_TOOLS    "Build bgfx tools."    ON )
option( BGFX_BUILD_EXAMPLES "Build bgfx examples." ON )

include( cmake/bx.cmake )
include( cmake/bgfx.cmake )

if( BGFX_BUILD_TOOLS )
	include( cmake/tools.cmake )
endif()

if( BGFX_BUILD_EXAMPLES )
	include( cmake/examples.cmake )
endif()
