# bgfx.cmake - bgfx building in cmake
# Written in 2016 by Joshua Brookover <josh@jalb.me>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

# The bx location is customizable via cache variable BX_DIR
if( NOT BX_DIR )
	set( BX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bx" CACHE STRING "Location of bgfx." )
endif()

# Ensure the directory exists
if( NOT IS_DIRECTORY ${BX_DIR} )
	message( SEND_ERROR "Could not load bx, directory does not exist. ${BX_DIR}" )
	return()
endif()

# Create the bx target
add_library( bx INTERFACE )

# Add include directory of bx
target_include_directories( bx INTERFACE ${BX_DIR}/include )

# Build system specific configurations
if( MSVC )
	target_include_directories( bx INTERFACE ${BX_DIR}/include/compat/msvc )
	target_compile_definitions( bx INTERFACE "__STDC_FORMAT_MACROS" )
elseif( MINGW )
	target_include_directories( bx INTERFACE ${BX_DIR}/include/compat/mingw )
elseif( APPLE )
	target_include_directories( bx INTERFACE ${BX_DIR}/include/compat/osx )
endif()
