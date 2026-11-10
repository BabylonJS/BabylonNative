# bgfx.cmake - bgfx building in cmake
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

set(L_SMASH_DIR ${BGFX_DIR}/3rdparty/l-smash)

file(
	GLOB_RECURSE
	L_SMASH_SOURCES #
	${L_SMASH_DIR}/*.c #
	${L_SMASH_DIR}/*.h #
)

add_library(l-smash STATIC ${L_SMASH_SOURCES})

# Put in a "bgfx" folder in Visual Studio
set_target_properties(l-smash PROPERTIES FOLDER "bgfx")

target_include_directories(l-smash PUBLIC ${BGFX_DIR}/3rdparty PRIVATE ${L_SMASH_DIR})

if(MSVC)
	target_compile_options(
		l-smash
		PRIVATE
			"/wd4005" # warning C4005: '_CRT_SECURE_NO_WARNINGS': macro redefinition
			"/wd4018" # warning C4018: '<=': signed/unsigned mismatch
			"/wd4100" # warning C4100: 'class': unreferenced parameter
			"/wd4116" # warning C4116: unnamed type definition in parentheses
			"/wd4125" # warning C4125: decimal digit terminates octal escape sequence
			"/wd4133" # warning C4133: '=': incompatible types
			"/wd4210" # warning C4210: nonstandard extension used: function given file scope
			"/wd4244" # warning C4244: 'initializing': conversion, possible loss of data
			"/wd4245" # warning C4245: '=': conversion, signed/unsigned mismatch
			"/wd4267" # warning C4267: 'initializing': conversion, possible loss of data
			"/wd4389" # warning C4389: '!=': signed/unsigned mismatch
			"/wd4701" # warning C4701: potentially uninitialized local variable used
	)
else()
	target_compile_options(
		l-smash
		PRIVATE
			"-Wno-empty-body" #
			"-Wno-implicit-fallthrough" #
			"-Wno-missing-field-initializers" #
			"-Wno-pragmas" #
			"-Wno-sign-compare" #
			"-Wno-type-limits" #
			"-Wno-undef" #
			"-Wno-unused-function" #
			"-Wno-unused-parameter" #
	)
endif()

if(MINGW)
	target_compile_options(
		l-smash
		PRIVATE
			"-Wno-maybe-uninitialized" #
			"-Wno-stringop-overflow" #
	)
endif()
