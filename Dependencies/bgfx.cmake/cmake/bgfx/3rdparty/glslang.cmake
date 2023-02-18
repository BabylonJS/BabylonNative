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

set(GLSLANG ${BGFX_DIR}/3rdparty/glslang)
set(SPIRV_TOOLS ${BGFX_DIR}/3rdparty/spirv-tools)

file(
	GLOB_RECURSE
	GLSLANG_SOURCES
	${GLSLANG}/glslang/*.cpp
	${GLSLANG}/glslang/*.h
	#
	${GLSLANG}/hlsl/*.cpp
	${GLSLANG}/hlsl/*.h
	#
	${GLSLANG}/SPIRV/*.cpp
	${GLSLANG}/SPIRV/*.h
	#
	${GLSLANG}/OGLCompilersDLL/*.cpp
	${GLSLANG}/OGLCompilersDLL/*.h
)

if(WIN32)
	list(FILTER GLSLANG_SOURCES EXCLUDE REGEX "glslang/OSDependent/Unix/.*.cpp")
	list(FILTER GLSLANG_SOURCES EXCLUDE REGEX "glslang/OSDependent/Unix/.*.h")
else()
	list(FILTER GLSLANG_SOURCES EXCLUDE REGEX "glslang/OSDependent/Windows/.*.cpp")
	list(FILTER GLSLANG_SOURCES EXCLUDE REGEX "glslang/OSDependent/Windows/.*.h")
endif()

add_library(glslang STATIC ${GLSLANG_SOURCES})

target_compile_definitions(
	glslang
	PRIVATE #
			ENABLE_OPT=1 # spriv-tools
			ENABLE_HLSL=1 #
)

# Put in a "bgfx" folder in Visual Studio
set_target_properties(glslang PROPERTIES FOLDER "bgfx")

target_include_directories(
	glslang
	PUBLIC ${GLSLANG} #
		   ${GLSLANG}/glslang/Public #
	PRIVATE ${GLSLANG}/.. #
			${SPIRV_TOOLS}/include #
			${SPIRV_TOOLS}/source #
)
