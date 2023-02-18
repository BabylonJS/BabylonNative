# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

# Ensure the directory exists
if(NOT IS_DIRECTORY ${BGFX_DIR})
	message(SEND_ERROR "Could not load bgfx, directory does not exist. ${BGFX_DIR}")
	return()
endif()

set(SPIRV_CROSS ${BGFX_DIR}/3rdparty/spirv-cross)

file(
	GLOB
	SPIRV_CROSS_SOURCES
	#
	${SPIRV_CROSS}/spirv.hpp
	${SPIRV_CROSS}/spirv_cfg.cpp
	${SPIRV_CROSS}/spirv_cfg.hpp
	${SPIRV_CROSS}/spirv_common.hpp
	${SPIRV_CROSS}/spirv_cpp.cpp
	${SPIRV_CROSS}/spirv_cpp.hpp
	${SPIRV_CROSS}/spirv_cross.cpp
	${SPIRV_CROSS}/spirv_cross.hpp
	${SPIRV_CROSS}/spirv_cross_parsed_ir.cpp
	${SPIRV_CROSS}/spirv_cross_parsed_ir.hpp
	${SPIRV_CROSS}/spirv_cross_util.cpp
	${SPIRV_CROSS}/spirv_cross_util.hpp
	${SPIRV_CROSS}/spirv_glsl.cpp
	${SPIRV_CROSS}/spirv_glsl.hpp
	${SPIRV_CROSS}/spirv_hlsl.cpp
	${SPIRV_CROSS}/spirv_hlsl.hpp
	${SPIRV_CROSS}/spirv_msl.cpp
	${SPIRV_CROSS}/spirv_msl.hpp
	${SPIRV_CROSS}/spirv_parser.cpp
	${SPIRV_CROSS}/spirv_parser.hpp
	${SPIRV_CROSS}/spirv_reflect.cpp
	${SPIRV_CROSS}/spirv_reflect.hpp
)

add_library(spirv-cross STATIC ${SPIRV_CROSS_SOURCES})

target_compile_definitions(spirv-cross PRIVATE SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS)

# Put in a "bgfx" folder in Visual Studio
set_target_properties(spirv-cross PROPERTIES FOLDER "bgfx")

target_include_directories(
	spirv-cross #
	PUBLIC #
		   ${SPIRV_CROSS} #
	PRIVATE #
			${SPIRV_CROSS}/include #
)
