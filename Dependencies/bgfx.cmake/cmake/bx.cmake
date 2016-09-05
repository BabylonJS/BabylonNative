# Copyright (c) 2016 Joshua Brookover

set( BX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bx" )

add_library( bx INTERFACE )

target_include_directories( bx INTERFACE ${BX_DIR}/include )

if( MSVC )
	target_include_directories( bx INTERFACE ${BX_DIR}/include/compat/msvc )
elseif( MINGW )
	target_include_directories( bx INTERFACE ${BX_DIR}/include/compat/mingw )
elseif( APPLE )
	target_include_directories( bx INTERFACE ${BX_DIR}/include/compat/osx )
endif()
