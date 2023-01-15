# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

add_executable(shaderc)

#target_include_directories(
#	shaderc
#	PRIVATE
#    ${BIMG_DIR}/include #
#    ${GLSL_OPTIMIZER}/include #
#    ${GLSL_OPTIMIZER}/src/mesa #
#    ${GLSL_OPTIMIZER}/src/mapi #
#    ${GLSL_OPTIMIZER}/src/glsl #
#)

target_link_libraries(
	shaderc
	PRIVATE bx
			bgfx-vertexlayout
			fcpp
			glslang
			glsl-optimizer
			spirv-opt
			spirv-cross
)
target_link_libraries(
	shaderc
	PRIVATE bx
			bimg
			bgfx-vertexlayout
			fcpp
			glslang
			glsl-optimizer
			spirv-opt
			spirv-cross
			webgpu
)
if(BGFX_AMALGAMATED)
	target_link_libraries(shaderc PRIVATE bgfx-shader)
endif()

# Grab the shaderc source files
file(
	GLOB
	SHADERC_SOURCES #
	${BGFX_DIR}/tools/shaderc/*.cpp #
	${BGFX_DIR}/tools/shaderc/*.h #
	${BGFX_DIR}/src/shader* #
)

target_sources(shaderc PRIVATE ${SHADERC_SOURCES})

set_target_properties(geometryc PROPERTIES FOLDER "bgfx/tools")

if(BGFX_BUILD_TOOLS AND BGFX_CUSTOM_TARGETS)
	add_dependencies(tools shaderc)
endif()

if(ANDROID)
	target_link_libraries(shaderc PRIVATE log)
elseif(IOS)
	set_target_properties(shaderc PROPERTIES MACOSX_BUNDLE ON MACOSX_BUNDLE_GUI_IDENTIFIER shaderc)
endif()
