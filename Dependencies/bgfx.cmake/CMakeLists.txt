# Copyright (c) 2016 Joshua Brookover

cmake_minimum_required( VERSION 3.0 )
project( bgfx )

set_property( GLOBAL PROPERTY USE_FOLDERS ON )

include( cmake/bx.cmake )
include( cmake/bgfx.cmake )
include( cmake/tools.cmake )
include( cmake/examples.cmake )
