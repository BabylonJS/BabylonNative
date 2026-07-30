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

# bx's SIMD path (bx/inline/simd128_sse.inl) uses SSE4.1 intrinsics on x86, matching the
# SSE4.2 minspec bx's own scripts/toolchain.lua sets. GCC-style drivers need the flag
# explicitly or the build fails with "needs target feature sse4.1"; MSVC's cl.exe does not.
# PUBLIC because bgfx/bimg compile bx's SIMD headers into their own translation units.
set(BX_X86_ARCH_REGEX "^(x86_64|amd64|AMD64|x64|i[3-6]86|x86)$")

if(APPLE)
	# Apple builds can be universal and CMAKE_SYSTEM_PROCESSOR names only one slice (it is
	# aarch64 for ios-cmake's SIMULATOR64COMBINED, which also builds x86_64), so the slice
	# list has to come from CMAKE_OSX_ARCHITECTURES.
	set(BX_TARGET_ARCHS ${CMAKE_OSX_ARCHITECTURES})
	if(NOT BX_TARGET_ARCHS)
		set(BX_TARGET_ARCHS ${CMAKE_SYSTEM_PROCESSOR})
	endif()

	set(BX_X86_ARCHS ${BX_TARGET_ARCHS})
	list(FILTER BX_X86_ARCHS INCLUDE REGEX "${BX_X86_ARCH_REGEX}")
	list(LENGTH BX_TARGET_ARCHS BX_TARGET_ARCH_COUNT)

	if(NOT BX_X86_ARCHS)
		# Nothing to do. An -Xarch_ flag matching no slice is an unused argument, which is
		# fatal for consumers building with -Werror.
	elseif(BX_TARGET_ARCH_COUNT EQUAL 1)
		target_compile_options(bx PUBLIC "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse4.2>")
	else()
		# -Xarch_ scopes the minspec to the x86 slices so arm64 keeps bx's NEON path.
		# -Wno-unused-command-line-argument is required because one invocation need not cover
		# every slice: Xcode compiles per slice, and ios-cmake's COMBINED platforms narrow the
		# slice list further per SDK.
		set(BX_SSE_MINSPEC "")
		foreach(BX_X86_ARCH IN LISTS BX_X86_ARCHS)
			list(APPEND BX_SSE_MINSPEC "-Xarch_${BX_X86_ARCH}" "-msse4.2")
		endforeach()
		list(APPEND BX_SSE_MINSPEC "-Wno-unused-command-line-argument")
		list(JOIN BX_SSE_MINSPEC " " BX_SSE_MINSPEC)
		target_compile_options(bx PUBLIC "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:SHELL:${BX_SSE_MINSPEC}>")
		unset(BX_SSE_MINSPEC)
	endif()

	unset(BX_TARGET_ARCH_COUNT)
	unset(BX_X86_ARCHS)
	unset(BX_TARGET_ARCHS)
elseif(NOT EMSCRIPTEN)
	# CMAKE_CXX_COMPILER_ARCHITECTURE_ID is the target arch where the compiler reports it
	# (MSVC/clang-cl); CMAKE_SYSTEM_PROCESSOR reports the host on Windows but is the target
	# elsewhere (the Android NDK sets i686 for the x86 ABI). Emscripten reports x86 yet builds
	# bx's wasm path, and emcc rejects -msse4.2 without -msimd128.
	if(CMAKE_CXX_COMPILER_ARCHITECTURE_ID)
		set(BX_TARGET_ARCH "${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
	else()
		set(BX_TARGET_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
	endif()

	if(BX_TARGET_ARCH MATCHES "${BX_X86_ARCH_REGEX}")
		target_compile_options(bx PUBLIC "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse4.2>")
	endif()

	unset(BX_TARGET_ARCH)
endif()

unset(BX_X86_ARCH_REGEX)

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
