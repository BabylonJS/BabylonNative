# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

# Grab the texturev source files
file(GLOB_RECURSE TEXTUREV_SOURCES #
	 ${BGFX_DIR}/tools/texturev/*
)

if(ANDROID)
	add_library(texturev SHARED ${TEXTUREV_SOURCES})
else()
	add_executable(texturev ${TEXTUREV_SOURCES})
endif()

target_link_libraries(texturev PRIVATE example-common)
set_target_properties(
	texturev PROPERTIES FOLDER "bgfx/tools" #
						OUTPUT_NAME ${BGFX_TOOLS_PREFIX}texturev #
)

if(BGFX_BUILD_TOOLS_TEXTURE AND BGFX_CUSTOM_TARGETS)
	add_dependencies(tools texturev)
endif()

if(ANDROID)
	set_property(TARGET texturev PROPERTY PREFIX "")
elseif(EMSCRIPTEN)
	target_link_options(texturev PRIVATE -sMAX_WEBGL_VERSION=2)
elseif(IOS)
	set_target_properties(texturev PROPERTIES MACOSX_BUNDLE ON MACOSX_BUNDLE_GUI_IDENTIFIER texturev)
endif()

if(BGFX_INSTALL)
	install(TARGETS texturev EXPORT "${TARGETS_EXPORT_NAME}" DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
