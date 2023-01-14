# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include(3rdparty/loadpng.cmake)
include(3rdparty/libsquish.cmake)
include(3rdparty/astc_encoder.cmake)
include(3rdparty/edtaa3.cmake)
include(3rdparty/etc1.cmake)
include(3rdparty/etc2.cmake)
include(3rdparty/nvtt.cmake)
include(3rdparty/pvrtc.cmake)
include(3rdparty/tinyexr.cmake)
include(3rdparty/iqa.cmake)
include(3rdparty/miniz.cmake)
include(bimg.cmake)
include(bimg_decode.cmake)
include(bimg_encode.cmake)

if(BGFX_BUILD_TOOLS_TEXTURE)
	include(texturec.cmake)
endif()
