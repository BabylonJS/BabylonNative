# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

if( BGFX_CUSTOM_TARGETS )
	add_custom_target( tools )
	set_target_properties( tools PROPERTIES FOLDER "bgfx/tools" )
endif()

include( ${CMAKE_CURRENT_LIST_DIR}/tools/geometryc.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/tools/geometryv.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/tools/shaderc.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/tools/texturec.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/tools/texturev.cmake )
