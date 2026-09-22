/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bimg/blob/master/LICENSE
 */

#ifndef BIMG_CONFIG_H_HEADER_GUARD
#define BIMG_CONFIG_H_HEADER_GUARD

#include <bx/bx.h>

// Decoders ---

#ifndef BIMG_CONFIG_DECODE_ENABLE
#	define BIMG_CONFIG_DECODE_ENABLE 1
#endif // BIMG_CONFIG_DECODE_ENABLE

#ifndef BIMG_CONFIG_DECODE_BC1
#	define BIMG_CONFIG_DECODE_BC1 BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_BC1

#ifndef BIMG_CONFIG_DECODE_BC2
#	define BIMG_CONFIG_DECODE_BC2 BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_BC2

#ifndef BIMG_CONFIG_DECODE_BC3
#	define BIMG_CONFIG_DECODE_BC3 BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_BC3

#ifndef BIMG_CONFIG_DECODE_BC4
#	define BIMG_CONFIG_DECODE_BC4 BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_BC4

#ifndef BIMG_CONFIG_DECODE_BC5
#	define BIMG_CONFIG_DECODE_BC5 BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_BC5

#ifndef BIMG_CONFIG_DECODE_BC6
#	define BIMG_CONFIG_DECODE_BC6 BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_BC6

#ifndef BIMG_CONFIG_DECODE_BC7
#	define BIMG_CONFIG_DECODE_BC7 BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_BC7

#ifndef BIMG_CONFIG_DECODE_ATC
#	define BIMG_CONFIG_DECODE_ATC BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_ATC

#ifndef BIMG_CONFIG_DECODE_ASTC
#	define BIMG_CONFIG_DECODE_ASTC BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_ASTC

#ifndef BIMG_CONFIG_DECODE_ETC1
#	define BIMG_CONFIG_DECODE_ETC1 BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_ETC1

#ifndef BIMG_CONFIG_DECODE_ETC2
#	define BIMG_CONFIG_DECODE_ETC2 BIMG_CONFIG_DECODE_ENABLE
#endif // BIMG_CONFIG_DECODE_ETC2

// Parsers ---

/// Enable or disable all image format parsers.
///
#ifndef BIMG_CONFIG_PARSE_ENABLE
#	define BIMG_CONFIG_PARSE_ENABLE 1
#endif // BIMG_CONFIG_PARSE_ENABLE

/// AVIF (AV1 Image File Format) - image format based on AV1.
///
#ifndef BIMG_CONFIG_PARSE_AVIF
#	define BIMG_CONFIG_PARSE_AVIF BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_AVIF

/// BMP (Windows Bitmap) - uncompressed raster image format.
///
#ifndef BIMG_CONFIG_PARSE_BMP
#	define BIMG_CONFIG_PARSE_BMP BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_BMP

/// EXR (OpenEXR) - high dynamic range image format by ILM.
///
#ifndef BIMG_CONFIG_PARSE_EXR
#	define BIMG_CONFIG_PARSE_EXR BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_EXR

/// GIF (Graphics Interchange Format) - indexed-color image format with animation support.
///
#ifndef BIMG_CONFIG_PARSE_GIF
#	define BIMG_CONFIG_PARSE_GIF BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_GIF

/// HDR (Radiance RGBE) - high dynamic range image format.
///
#ifndef BIMG_CONFIG_PARSE_HDR
#	define BIMG_CONFIG_PARSE_HDR BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_HDR

/// HEIF (High Efficiency Image File Format) - image format based on HEVC.
///
#ifndef BIMG_CONFIG_PARSE_HEIF
#	define BIMG_CONFIG_PARSE_HEIF 0
#endif // BIMG_CONFIG_PARSE_HEIF

/// JPEG (Joint Photographic Experts Group) - lossy compressed image format.
///
#ifndef BIMG_CONFIG_PARSE_JPEG
#	define BIMG_CONFIG_PARSE_JPEG BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_JPEG

/// PIC (Softimage) - image format used by Softimage 3D tools.
///
#ifndef BIMG_CONFIG_PARSE_PIC
#	define BIMG_CONFIG_PARSE_PIC BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_PIC

/// PNG (Portable Network Graphics) - lossless compressed image format.
///
#ifndef BIMG_CONFIG_PARSE_PNG
#	define BIMG_CONFIG_PARSE_PNG BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_PNG

/// PNM (Portable Any Map) - family of simple image formats (PBM, PGM, PPM).
///
#ifndef BIMG_CONFIG_PARSE_PNM
#	define BIMG_CONFIG_PARSE_PNM BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_PNM

/// PSD (Adobe Photoshop Document) - layered image format.
///
#ifndef BIMG_CONFIG_PARSE_PSD
#	define BIMG_CONFIG_PARSE_PSD BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_PSD

/// TGA (Truevision TGA) - raster image format with optional RLE compression.
///
#ifndef BIMG_CONFIG_PARSE_TGA
#	define BIMG_CONFIG_PARSE_TGA BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_TGA

/// WebP - lossy and lossless image format by Google.
///
#ifndef BIMG_CONFIG_PARSE_WEBP
#	define BIMG_CONFIG_PARSE_WEBP BIMG_CONFIG_PARSE_ENABLE
#endif // BIMG_CONFIG_PARSE_WEBP

/// WIC (Windows Imaging Component) - Windows-only backup decoder for common raster formats (PNG, JPEG, BMP, GIF).
///
#ifndef BIMG_CONFIG_PARSE_WIC
#	define BIMG_CONFIG_PARSE_WIC (BIMG_CONFIG_PARSE_ENABLE && BX_PLATFORM_WINDOWS && (0 \
		|| 0 == BIMG_CONFIG_PARSE_PNG                                                   \
		|| 0 == BIMG_CONFIG_PARSE_JPEG                                                  \
		|| 0 == BIMG_CONFIG_PARSE_BMP                                                   \
		|| 0 == BIMG_CONFIG_PARSE_GIF                                                   \
		) )
#endif // BIMG_CONFIG_PARSE_WIC

#endif // BIMG_CONFIG_H_HEADER_GUARD
