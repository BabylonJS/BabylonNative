# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if(TARGET tint)
	return()
endif()

if(NOT IS_DIRECTORY ${BGFX_DIR})
	message(SEND_ERROR "Could not load bgfx, directory does not exist. ${BGFX_DIR}")
	return()
endif()

set(TINT_DIR ${BGFX_DIR}/3rdparty/dawn)
set(SPIRV_HEADERS ${BGFX_DIR}/3rdparty/spirv-headers)
set(SPIRV_TOOLS ${BGFX_DIR}/3rdparty/spirv-tools)

file(
	GLOB_RECURSE
	TINT_SOURCES
	${TINT_DIR}/src/tint/utils/*.cc
	${TINT_DIR}/src/tint/utils/*.h
	${TINT_DIR}/src/tint/lang/core/*.cc
	${TINT_DIR}/src/tint/lang/core/*.h
	${TINT_DIR}/src/tint/lang/null/*.cc
	${TINT_DIR}/src/tint/lang/null/*.h
	${TINT_DIR}/src/tint/lang/spirv/*.cc
	${TINT_DIR}/src/tint/lang/spirv/*.h
	${TINT_DIR}/src/tint/lang/wgsl/*.cc
	${TINT_DIR}/src/tint/lang/wgsl/*.h
	${TINT_DIR}/src/tint/api/*.cc
	${TINT_DIR}/src/tint/api/*.h
)

add_library(tint STATIC ${TINT_SOURCES})

set_target_properties(tint PROPERTIES FOLDER "bgfx")

target_include_directories(
	tint
	PUBLIC ${TINT_DIR}
		   ${TINT_DIR}/src/tint
	PRIVATE ${TINT_DIR}/third_party/protobuf/src
			${TINT_DIR}/third_party/abseil-cpp
			${SPIRV_TOOLS}
			${SPIRV_TOOLS}/include
			${SPIRV_TOOLS}/include/generated
			${SPIRV_HEADERS}/include
)

target_compile_definitions(
	tint
	PRIVATE TINT_BUILD_GLSL_WRITER=0
			TINT_BUILD_HLSL_WRITER=0
			TINT_BUILD_MSL_WRITER=0
			TINT_BUILD_NULL_WRITER=0
			TINT_BUILD_SPV_READER=1
			TINT_BUILD_SPV_WRITER=0
			TINT_BUILD_WGSL_READER=0
			TINT_BUILD_WGSL_WRITER=1
			TINT_ENABLE_IR_VALIDATION=0
)

if(WIN32)
	target_compile_definitions(
		tint
		PRIVATE TINT_BUILD_IS_LINUX=0
				TINT_BUILD_IS_MAC=0
				TINT_BUILD_IS_WIN=1
	)
elseif(APPLE)
	target_compile_definitions(
		tint
		PRIVATE TINT_BUILD_IS_LINUX=0
				TINT_BUILD_IS_MAC=1
				TINT_BUILD_IS_WIN=0
	)
else()
	target_compile_definitions(
		tint
		PRIVATE TINT_BUILD_IS_LINUX=1
				TINT_BUILD_IS_MAC=0
				TINT_BUILD_IS_WIN=0
	)
endif()

if(MSVC)
	target_compile_options(
		tint
		PRIVATE "/Zc:preprocessor"
	)
endif()
