# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

# Grab the texturec source files
file(GLOB_RECURSE TEXTUREC_SOURCES #
	 ${BIMG_DIR}/tools/texturec/*.cpp #
	 ${BIMG_DIR}/tools/texturec/*.h #
)

add_executable(texturec ${TEXTUREC_SOURCES})

target_link_libraries(texturec PRIVATE bimg_decode bimg_encode bimg)
set_target_properties(
	texturec PROPERTIES FOLDER "bgfx/tools" #
						OUTPUT_NAME ${BGFX_TOOLS_PREFIX}texturec #
)

if(BGFX_BUILD_TOOLS_TEXTURE)
	add_executable(bgfx::texturec ALIAS texturec)
	if(BGFX_CUSTOM_TARGETS)
		add_dependencies(tools texturec)
	endif()
endif()

if(ANDROID)
	target_link_libraries(texturec PRIVATE log)
elseif(IOS)
	set_target_properties(texturec PROPERTIES MACOSX_BUNDLE ON MACOSX_BUNDLE_GUI_IDENTIFIER texturec)
endif()

if(BGFX_INSTALL)
	install(TARGETS texturec EXPORT "${TARGETS_EXPORT_NAME}" DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
