# bgfx.cmake - bgfx building in cmake
# Written in 2016 by Joshua Brookover <josh@jalb.me>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

add_library( bgfx-vertexdecl INTERFACE )
target_sources( bgfx-vertexdecl INTERFACE ${BGFX_DIR}/src/vertexdecl.cpp )
target_include_directories( bgfx-vertexdecl INTERFACE ${BGFX_DIR}/include )

add_library( bgfx-bounds INTERFACE )
target_sources( bgfx-bounds INTERFACE ${BGFX_DIR}/examples/common/bounds.cpp )
target_include_directories( bgfx-bounds INTERFACE ${BGFX_DIR}/include )
target_include_directories( bgfx-bounds INTERFACE ${BGFX_DIR}/examples/common )
