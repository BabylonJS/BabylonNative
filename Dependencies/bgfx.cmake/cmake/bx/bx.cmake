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
if(NOT IS_DIRECTORY ${BX_DIR})
	message(SEND_ERROR "Could not load bx, directory does not exist. ${BX_DIR}")
	return()
endif()

include(GNUInstallDirs)

# Grab the bx source files
file(
	GLOB_RECURSE
	BX_SOURCES
	${BX_DIR}/include/*.h #
	${BX_DIR}/include/**.inl #
	${BX_DIR}/src/*.cpp #
	${BX_DIR}/scripts/*.natvis #
)

if(BX_AMALGAMATED)
	list(APPEND BX_NOBUILD "${BX_DIR}/src/allocator.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/bounds.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/bx.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/commandline.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/crtnone.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/debug.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/dtoa.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/easing.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/file.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/filepath.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/hash.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/math.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/mutex.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/os.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/process.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/semaphore.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/settings.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/sort.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/string.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/thread.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/timer.cpp")
	list(APPEND BX_NOBUILD "${BX_DIR}/src/url.cpp")
else()
	file(GLOB_RECURSE BX_NOBUILD "${BX_DIR}/src/amalgamated.*")
endif()

# Exclude files from the build but keep them in project
foreach(BX_SRC ${BX_NOBUILD})
	set_source_files_properties(${BX_SRC} PROPERTIES HEADER_FILE_ONLY ON)
endforeach()

add_library(bx STATIC ${BX_SOURCES})

if(MSVC)
	target_compile_options(bx PRIVATE /EHs-c-)
	target_compile_definitions(bx PRIVATE _HAS_EXCEPTIONS=0)
endif()

# Put in a "bgfx" folder in Visual Studio
set_target_properties(bx PROPERTIES FOLDER "bgfx")

# Build system specific configurations
if(MINGW)
	set(BX_COMPAT_PLATFORM mingw)
elseif(WIN32)
	set(BX_COMPAT_PLATFORM msvc)
elseif(APPLE) # APPLE is technically UNIX... ORDERING MATTERS!
	set(BX_COMPAT_PLATFORM osx)
elseif(UNIX)
	set(BX_COMPAT_PLATFORM linux)
endif()

# Add include directory of bx
target_include_directories(
	bx
	PUBLIC $<BUILD_INTERFACE:${BX_DIR}/include> #
		   $<BUILD_INTERFACE:${BX_DIR}/3rdparty> #
		   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> #
		   $<BUILD_INTERFACE:${BX_DIR}/include/compat/${BX_COMPAT_PLATFORM}> #
		   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/bx/compat/${BX_COMPAT_PLATFORM}> #
)

# All configurations
target_compile_definitions(bx PUBLIC "BX_CONFIG_DEBUG=$<IF:$<CONFIG:Debug>,1,$<BOOL:${BX_CONFIG_DEBUG}>>")
target_compile_definitions(bx PUBLIC "__STDC_LIMIT_MACROS")
target_compile_definitions(bx PUBLIC "__STDC_FORMAT_MACROS")
target_compile_definitions(bx PUBLIC "__STDC_CONSTANT_MACROS")

target_compile_features(bx PUBLIC cxx_std_14)
# (note: see bx\scripts\toolchain.lua for equivalent compiler flag)
target_compile_options(bx PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus /Zc:preprocessor>)

# bx's x86_64 SIMD path (include/bx/inline/simd128_sse.inl) uses SSE4.1
# intrinsics (e.g. _mm_round_ps / _mm_blendv_ps). bx's scripts/toolchain.lua
# sets an SSE4.2 minspec for x86_64 targets, but this CMake build did not, so
# GCC/Clang frontends (which do not enable SSE4.x by default) fail to compile
# the SIMD path on x86_64 with "needs target feature sse4.1". Propagate the same
# minspec here, PUBLIC so that bgfx / bimg (which compile bx SIMD headers into
# their own translation units) inherit it. MSVC (cl.exe) permits the intrinsics
# without an ISA flag and is left untouched.
if(APPLE)
	# Apple builds may be universal (multiple slices compiled in a single
	# invocation). Work out which slices are actually being built: an explicit
	# CMAKE_OSX_ARCHITECTURES wins, otherwise it is a single-slice build for the
	# host/system processor.
	set(BX_APPLE_ARCHS ${CMAKE_OSX_ARCHITECTURES})
	if(NOT BX_APPLE_ARCHS)
		set(BX_APPLE_ARCHS ${CMAKE_SYSTEM_PROCESSOR})
	endif()
	list(LENGTH BX_APPLE_ARCHS BX_APPLE_ARCH_COUNT)

	if(NOT "x86_64" IN_LIST BX_APPLE_ARCHS)
		# No x86_64 slice, so no SSE minspec is needed. Note that -Xarch_x86_64
		# must not be added here either: with no x86_64 slice to apply it to,
		# clang reports it as an unused argument, which is fatal under -Werror.
	elseif(BX_APPLE_ARCH_COUNT GREATER 1)
		# Universal build; -Xarch_x86_64 scopes the flag to the x86_64 slice only,
		# so the arm64 slice (which uses the NEON SIMD path) is unaffected.
		target_compile_options(bx PUBLIC "$<$<CXX_COMPILER_ID:AppleClang,Clang>:SHELL:-Xarch_x86_64 -msse4.2>")
	else()
		# Single-slice x86_64 build; the flag applies to the only slice there is.
		target_compile_options(bx PUBLIC $<$<CXX_COMPILER_ID:AppleClang,Clang>:-msse4.2>)
	endif()

	unset(BX_APPLE_ARCH_COUNT)
	unset(BX_APPLE_ARCHS)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|amd64|AMD64)")
	target_compile_options(bx PUBLIC $<$<CXX_COMPILER_ID:GNU,Clang>:-msse4.2>)
endif()

# Link against psapi on Windows
if(WIN32)
	target_link_libraries(bx PUBLIC psapi)
endif()

# Additional dependencies on Unix
if(ANDROID)
	# For __android_log_write
	find_library(LOG_LIBRARY log)
	mark_as_advanced(LOG_LIBRARY)
	target_link_libraries(bx PUBLIC ${LOG_LIBRARY})
elseif(APPLE)
	find_library(FOUNDATION_LIBRARY Foundation)
	mark_as_advanced(FOUNDATION_LIBRARY)
	target_link_libraries(bx PUBLIC ${FOUNDATION_LIBRARY})
elseif(UNIX)
	# Threads
	find_package(Threads)
	target_link_libraries(bx ${CMAKE_THREAD_LIBS_INIT} dl)

	# Real time (for clock_gettime)
	target_link_libraries(bx rt)
endif()

# Put in a "bgfx" folder in Visual Studio
set_target_properties(bx PROPERTIES FOLDER "bgfx")

if(BGFX_INSTALL)
	install(
		TARGETS bx
		EXPORT "${TARGETS_EXPORT_NAME}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	)
	# We will make sure tinystl and compat are not installed in /usr/include
	install(DIRECTORY "${BX_DIR}/include/bx" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
	install(DIRECTORY "${BX_DIR}/include/compat/${BX_COMPAT_PLATFORM}"
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/bx/compat"
	)
	install(DIRECTORY "${BX_DIR}/include/tinystl" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/bx")
endif()
