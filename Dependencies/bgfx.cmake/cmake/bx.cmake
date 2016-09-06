# Copyright (c) 2016 Joshua Brookover

if( NOT BX_DIR )
	set( BX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bx" CACHE STRING "Location of bgfx." )
endif()

if( NOT IS_DIRECTORY ${BX_DIR} )
	message( SEND_ERROR "Could not load bx, directory does not exist. ${BX_DIR}" )
	return()
endif()

add_library( bx INTERFACE )

target_include_directories( bx INTERFACE ${BX_DIR}/include )

if( MSVC )
	target_include_directories( bx INTERFACE ${BX_DIR}/include/compat/msvc )
elseif( MINGW )
	target_include_directories( bx INTERFACE ${BX_DIR}/include/compat/mingw )
elseif( APPLE )
	target_include_directories( bx INTERFACE ${BX_DIR}/include/compat/osx )
endif()
