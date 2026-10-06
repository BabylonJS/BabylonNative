# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

# Grab the geometryc source files
file(
	GLOB_RECURSE
	GEOMETRYC_SOURCES #
	${BGFX_DIR}/tools/geometryc/*.cpp #
	${BGFX_DIR}/tools/geometryc/*.h #
	#
	${MESHOPTIMIZER_SOURCES}
)
add_executable(geometryc ${GEOMETRYC_SOURCES})

target_include_directories(geometryc PRIVATE ${MESHOPTIMIZER_INCLUDE_DIR})
target_link_libraries(geometryc PRIVATE bx bgfx-vertexlayout ${MESHOPTIMIZER_LIBRARIES})
target_compile_definitions(geometryc PRIVATE "-D_CRT_SECURE_NO_WARNINGS")
set_target_properties(
	geometryc PROPERTIES FOLDER "bgfx/tools" #
						 OUTPUT_NAME ${BGFX_TOOLS_PREFIX}geometryc #
)

if(BGFX_BUILD_TOOLS_GEOMETRY)
	add_executable(bgfx::geometryc ALIAS geometryc)
	if(BGFX_CUSTOM_TARGETS)
		add_dependencies(tools geometryc)
	endif()
endif()

if(IOS)
	set_target_properties(geometryc PROPERTIES MACOSX_BUNDLE ON MACOSX_BUNDLE_GUI_IDENTIFIER geometryc)
endif()

if(BGFX_INSTALL)
	install(TARGETS geometryc EXPORT "${TARGETS_EXPORT_NAME}" DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
