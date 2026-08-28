# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if(TARGET webgpu)
	return()
endif()

file(GLOB WEBGPU_SOURCES ${BGFX_DIR}/3rdparty/webgpu/include/webgpu/*.h
	 # ${BGFX_DIR}/3rdparty/webgpu/webgpu_cpp.cpp  # requires dawn to be installed
)

# Library without sources is interface
#add_library( webgpu STATIC ${WEBGPU_SOURCES} )
add_library(webgpu INTERFACE)
target_include_directories(
	webgpu # PUBLIC
	INTERFACE $<BUILD_INTERFACE:${BGFX_DIR}/3rdparty/webgpu/include>
)

# These properties are not allowed on interface
# set_target_properties(webgpu PROPERTIES FOLDER "bgfx/3rdparty" PREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}bgfx-")

if(BGFX_INSTALL AND BGFX_CONFIG_RENDERER_WEBGPU)
	install(
		TARGETS webgpu
		EXPORT "${TARGETS_EXPORT_NAME}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		INCLUDES
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	)
endif()
