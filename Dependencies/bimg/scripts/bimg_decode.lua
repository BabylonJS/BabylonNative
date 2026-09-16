--
-- Copyright 2010-2026 Branimir Karadzic. All rights reserved.
-- License: https://github.com/bkaradzic/bx#license-bsd-2-clause
--

if _OPTIONS["with-libheif"] then
	print('\n')
	print('\tWARNING!')
	print('')
	print('\t*** LICENSE INCOMPATIBILITY WARNING!')
	print('\t*** LibHeif is licensed under LGPL! See:')
	print('\t*** https://github.com/strukturag/libheif/blob/master/COPYING')
	print('\n')
end

project "bimg_decode"
	kind "StaticLib"

	includedirs {
		path.join(BIMG_DIR, "include"),
		path.join(BIMG_DIR, "3rdparty"),
		path.join(BIMG_DIR, "3rdparty/tinyexr/deps"),
	}

	files {
		path.join(BIMG_DIR, "include/**"),
		path.join(BIMG_DIR, "src/image_decode*.*"),
	}

	if _OPTIONS["with-libheif"] then
		defines {
			"BIMG_CONFIG_PARSE_HEIF=1",
		}
	end

	defines {
		"AVIF_CODEC_DAV1D",
	}

	includedirs {
		path.join(BIMG_DIR, "3rdparty/libavif"),
		path.join(BIMG_DIR, "3rdparty/libavif/include"),
		path.join(BIMG_DIR, "3rdparty/libavif/third_party/libyuv/include"),
		path.join(BIMG_DIR, "3rdparty/dav1d"),
		path.join(BIMG_DIR, "3rdparty/dav1d/include"),
	}

	files {
		path.join(BIMG_DIR, "3rdparty/dav1d/dav1d-amalgamated.c"),
		path.join(BIMG_DIR, "3rdparty/dav1d/dav1d-bitdepth-8.c"),
		path.join(BIMG_DIR, "3rdparty/dav1d/dav1d-bitdepth-16.c"),
		path.join(BIMG_DIR, "3rdparty/libavif/libavif-amalgamated.c"),
	}

	using_bx()

	configuration { "vs*" }
		includedirs {
			path.join(BIMG_DIR, "3rdparty/dav1d/include/compat/msvc"),
		}

	configuration { "gmake or xcode*" }
		buildoptions_c {
			"-std=c11",
		}

	configuration { "linux-*" }
		buildoptions {
			"-fPIC",
		}

	configuration {}
