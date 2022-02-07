# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

add_executable( geometryv ${BGFX_DIR}/tools/geometryv/geometryv.cpp )
set_target_properties( geometryv PROPERTIES FOLDER "bgfx/tools" )
target_link_libraries( geometryv example-common )
if(EMSCRIPTEN)
	target_link_options( geometryv PRIVATE -sMAX_WEBGL_VERSION=2 )
endif()
if( BGFX_CUSTOM_TARGETS )
	add_dependencies( tools geometryv )
endif()

if (IOS)
	set_target_properties(geometryv PROPERTIES MACOSX_BUNDLE ON
										      MACOSX_BUNDLE_GUI_IDENTIFIER geometryv)
endif()
