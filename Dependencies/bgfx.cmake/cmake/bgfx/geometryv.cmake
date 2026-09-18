# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

# Grab the geometryv source files
file(GLOB_RECURSE GEOMETRYV_SOURCES #
	 ${BGFX_DIR}/tools/geometryv/*
)

if(ANDROID)
	add_library(geometryv SHARED ${GEOMETRYV_SOURCES})
else()
	add_executable(geometryv ${GEOMETRYV_SOURCES})
endif()

target_link_libraries(geometryv PRIVATE example-common)
set_target_properties(
	geometryv PROPERTIES FOLDER "bgfx/tools" #
						 OUTPUT_NAME ${BGFX_TOOLS_PREFIX}geometryv #
)

if(BGFX_BUILD_TOOLS_GEOMETRY AND BGFX_CUSTOM_TARGETS)
	add_dependencies(tools geometryv)
endif()

if(ANDROID)
	set_property(TARGET geometryv PROPERTY PREFIX "")
elseif(EMSCRIPTEN)
	target_link_options(geometryv PRIVATE -sMAX_WEBGL_VERSION=2)
elseif(IOS)
	set_target_properties(geometryv PROPERTIES MACOSX_BUNDLE ON MACOSX_BUNDLE_GUI_IDENTIFIER geometryv)
endif()

if(BGFX_INSTALL)
	install(TARGETS geometryv EXPORT "${TARGETS_EXPORT_NAME}" DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
