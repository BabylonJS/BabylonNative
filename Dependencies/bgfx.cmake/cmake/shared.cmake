# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

add_library( bgfx-vertexlayout INTERFACE )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/generated/vertexlayout.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/generated/vertexlayout.cpp )
target_sources( bgfx-vertexlayout INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated/vertexlayout.cpp )
target_include_directories( bgfx-vertexlayout INTERFACE ${BGFX_DIR}/include )

add_library( bgfx-shader INTERFACE )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/generated/shader.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/generated/shader.cpp )
target_sources( bgfx-shader INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated/shader.cpp )
target_include_directories( bgfx-shader INTERFACE ${BGFX_DIR}/include )

add_library( bgfx-bounds INTERFACE )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/generated/bounds.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/generated/bounds.cpp )
target_sources( bgfx-bounds INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated/bounds.cpp )
target_include_directories( bgfx-bounds INTERFACE ${BGFX_DIR}/include )
target_include_directories( bgfx-bounds INTERFACE ${BGFX_DIR}/examples/common )

# Frameworks required on OS X
if( ${CMAKE_SYSTEM_NAME} MATCHES Darwin )
	find_library( COCOA_LIBRARY Cocoa )
	mark_as_advanced( COCOA_LIBRARY )
	target_link_libraries( bgfx-vertexlayout INTERFACE ${COCOA_LIBRARY} )
endif()
