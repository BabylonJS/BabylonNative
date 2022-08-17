# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include( CMakeParseArguments )

include( ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/fcpp.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/glsl-optimizer.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/glslang.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/spirv-cross.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/spirv-tools.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/webgpu.cmake )

add_executable( shaderc ${BGFX_DIR}/tools/shaderc/shaderc.cpp ${BGFX_DIR}/tools/shaderc/shaderc.h ${BGFX_DIR}/tools/shaderc/shaderc_glsl.cpp ${BGFX_DIR}/tools/shaderc/shaderc_hlsl.cpp ${BGFX_DIR}/tools/shaderc/shaderc_pssl.cpp ${BGFX_DIR}/tools/shaderc/shaderc_spirv.cpp ${BGFX_DIR}/tools/shaderc/shaderc_metal.cpp )
target_compile_definitions( shaderc PRIVATE "-D_CRT_SECURE_NO_WARNINGS" )
set_target_properties( shaderc PROPERTIES FOLDER "bgfx/tools" )
target_link_libraries(shaderc PRIVATE bx bimg bgfx-vertexlayout bgfx-shader fcpp glsl-optimizer glslang spirv-cross spirv-tools webgpu)

if( BGFX_CUSTOM_TARGETS )
	add_dependencies( tools shaderc )
endif()

if (ANDROID)
    target_link_libraries(shaderc PRIVATE log)
elseif (IOS)
	set_target_properties(shaderc PROPERTIES MACOSX_BUNDLE ON
											 MACOSX_BUNDLE_GUI_IDENTIFIER shaderc)
endif()
