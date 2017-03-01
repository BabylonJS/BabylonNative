# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

# Ensure the directory exists
if( NOT IS_DIRECTORY ${BX_DIR} )
	message( SEND_ERROR "Could not load bx, directory does not exist. ${BX_DIR}" )
	return()
endif()

# Grab the bx source files
file( GLOB BX_SOURCES ${BX_DIR}/src/*.cpp )

# Create the bx target
add_library( bx STATIC ${BX_SOURCES} )

# Link against psapi in Visual Studio
if( MSVC )
	target_link_libraries( bx PUBLIC psapi )
endif()

# Add include directory of bx
target_include_directories( bx PUBLIC ${BX_DIR}/include )

# Build system specific configurations
if( MSVC )
	target_include_directories( bx PUBLIC ${BX_DIR}/include/compat/msvc )
elseif( MINGW )
	target_include_directories( bx PUBLIC ${BX_DIR}/include/compat/mingw )
elseif( APPLE )
	target_include_directories( bx PUBLIC ${BX_DIR}/include/compat/osx )
endif()

# All configurations
target_compile_definitions( bx PUBLIC "__STDC_LIMIT_MACROS" )
target_compile_definitions( bx PUBLIC "__STDC_FORMAT_MACROS" )
target_compile_definitions( bx PUBLIC "__STDC_CONSTANT_MACROS" )

# Threads
if( UNIX AND NOT APPLE )
	find_package( Threads )
	target_link_libraries( bx ${CMAKE_THREAD_LIBS_INIT} dl )
endif()

# Put in a "bgfx" folder in Visual Studio
set_target_properties( bx PROPERTIES FOLDER "bgfx" )
