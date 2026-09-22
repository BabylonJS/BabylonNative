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

set(FCPP_DIR ${BGFX_DIR}/3rdparty/fcpp)

file(
	GLOB
	FCPP_SOURCES
	${FCPP_DIR}/*.h
	${FCPP_DIR}/cpp1.c
	${FCPP_DIR}/cpp2.c
	${FCPP_DIR}/cpp3.c
	${FCPP_DIR}/cpp4.c
	${FCPP_DIR}/cpp5.c
	${FCPP_DIR}/cpp6.c
	${FCPP_DIR}/cpp6.c
)

add_library(fcpp STATIC ${FCPP_SOURCES})

target_compile_definitions(
	fcpp
	PRIVATE "NINCLUDE=64" #
			"NWORK=65536" #
			"NBUFF=65536" #
			"OLD_PREPROCESSOR=0" #
	# "MSG_PREFIX=\"Preprocessor: \"" #
)

# Put in a "bgfx" folder in Visual Studio
set_target_properties(fcpp PROPERTIES FOLDER "bgfx")

target_include_directories(fcpp PUBLIC ${FCPP_DIR})

if(MSVC)
	target_compile_options(
		fcpp
		PRIVATE
			"/wd4055" # warning C4055: 'type cast': from data pointer 'void *' to function pointer 'void (__cdecl *)(char *,void *)'
			"/wd4244" # warning C4244: '=': conversion from 'const flex_int32_t' to 'YY_CHAR', possible loss of data
			"/wd4701" # warning C4701: potentially uninitialized local variable 'lower' used
			"/wd4706" # warning C4706: assignment within conditional expression
	)
else()
	target_compile_options(
		fcpp
		PRIVATE -Wno-implicit-fallthrough #
				-Wno-incompatible-pointer-types #
				-Wno-parentheses-equality #
	)
endif()
