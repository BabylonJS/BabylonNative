--
-- Copyright 2010-2026 Branimir Karadzic. All rights reserved.
-- License: https://github.com/bkaradzic/bx#license-bsd-2-clause
--

project "bimg"
	kind "StaticLib"

	defines {
		"ASTCENC_F16C=0",
		"ASTCENC_NEON=0",
	}

	includedirs {
		path.join(BIMG_DIR, "include"),
		path.join(BIMG_DIR, "3rdparty/astc-encoder/include"),
	}

	files {
		path.join(BIMG_DIR, "include/**"),
		path.join(BIMG_DIR, "src/image.cpp"),
		path.join(BIMG_DIR, "src/bimg_p.h"),
		path.join(BIMG_DIR, "src/config.h"),

		path.join(BIMG_DIR, "3rdparty/astc-encoder/source/**.cpp"),
		path.join(BIMG_DIR, "3rdparty/astc-encoder/source/**.h"),
	}

	using_bx()

	configuration {}

	removeflags {
		"FloatFast", -- astc-encoder doesn't work with it.
	}

	configuration { "linux-*" }
		buildoptions {
			"-fPIC",
		}

	configuration {}
