/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bimg/blob/master/LICENSE
 */

#ifndef BIMG_IMAGE_H_HEADER_GUARD
#define BIMG_IMAGE_H_HEADER_GUARD

#include <stdint.h> // uint32_t
#include <stdlib.h> // NULL

#define BIMG_API_VERSION UINT32_C(7)

namespace bx
{
	struct AllocatorI;
	class  Error;
	struct ReaderSeekerI;
	struct WriterI;

} // namespace bx

namespace bimg
{
	///
	typedef void (*PackFn)(void*, const float*);

	///
	typedef void (*UnpackFn)(float*, const void*);

	/// Texture format enum.
	///
	/// Notation:
	///
	///       RGBA16S
	///       ^   ^ ^
	///       |   | +-- [ ]Unorm
	///       |   |     [F]loat
	///       |   |     [S]norm
	///       |   |     [I]nt
	///       |   |     [U]int
	///       |   +---- Number of bits per component
	///       +-------- Components
	///
	/// @attention Availability depends on Caps (see: formats).
	///
	/// @attention C99 equivalent is `bgfx_texture_format_t`.
	///
	struct TextureFormat
	{
		/// Texture formats:
		enum Enum
		{
			BC1,          //!< Block Compression 1. 5-bit R, 6-bit G, 5-bit B, 1-bit A. 4 BPP.
			BC2,          //!< Block Compression 2. 5-bit R, 6-bit G, 5-bit B, 4-bit explicit A. 8 BPP.
			BC3,          //!< Block Compression 3. 5-bit R, 6-bit G, 5-bit B, 8-bit interpolated A. 8 BPP.
			BC4,          //!< Block Compression 4. Single 8-bit red channel, unsigned normalized. 4 BPP.
			BC4S,         //!< Block Compression 4. Single 8-bit red channel, signed normalized. 4 BPP.
			BC5,          //!< Block Compression 5. Two 8-bit channels (RG), unsigned normalized. 8 BPP.
			BC5S,         //!< Block Compression 5. Two 8-bit channels (RG), signed normalized. 8 BPP.
			BC6H,         //!< Block Compression 6H. Three 16-bit signed floating-point channels (RGB), HDR. 8 BPP.
			BC6HU,        //!< Block Compression 6H. Three 16-bit unsigned floating-point channels (RGB), HDR. 8 BPP.
			BC7,          //!< RGB 4-7 bits per color channel, 0-8 bits alpha. Block Compression 7. High-quality RGBA, 4-7 bits per color, 0-8 bits alpha. 8 BPP.
			ETC1,         //!< Ericsson Texture Compression 1. 8-bit per channel RGB. 4 BPP.
			ETC2,         //!< Ericsson Texture Compression 2. 8-bit per channel RGB. 4 BPP.
			ETC2A,        //!< Ericsson Texture Compression 2 with full alpha. 8-bit per channel RGBA. 8 BPP.
			ETC2A1,       //!< Ericsson Texture Compression 2 with 1-bit punch-through alpha. 4 BPP.
			EACR11,       //!< ETC2 Alpha Compression, single 11-bit red channel, unsigned normalized. 4 BPP.
			EACR11S,      //!< ETC2 Alpha Compression, single 11-bit red channel, signed normalized. 4 BPP.
			EACRG11,      //!< ETC2 Alpha Compression, two 11-bit channels (RG), unsigned normalized. 8 BPP.
			EACRG11S,     //!< ETC2 Alpha Compression, two 11-bit channels (RG), signed normalized. 8 BPP.
			PTC12,        //!< PowerVR Texture Compression v1. 3-channel RGB. 2 BPP.
			PTC14,        //!< PowerVR Texture Compression v1. 3-channel RGB. 4 BPP.
			PTC12A,       //!< PowerVR Texture Compression v1. 4-channel RGBA. 2 BPP.
			PTC14A,       //!< PowerVR Texture Compression v1. 4-channel RGBA. 4 BPP.
			PTC22,        //!< PowerVR Texture Compression v2. 4-channel RGBA. 2 BPP.
			PTC24,        //!< PowerVR Texture Compression v2. 4-channel RGBA. 4 BPP.
			ATC,          //!< AMD Texture Compression. 3-channel RGB. 4 BPP.
			ATCE,         //!< AMD Texture Compression with explicit alpha. 4-channel RGBA. 8 BPP.
			ATCI,         //!< AMD Texture Compression with interpolated alpha. 4-channel RGBA. 8 BPP.
			ASTC4x4,      //!< Adaptive Scalable Texture Compression, 4x4 block, RGBA. 8.00 BPP.
			ASTC5x4,      //!< Adaptive Scalable Texture Compression, 5x4 block, RGBA. 6.40 BPP.
			ASTC5x5,      //!< Adaptive Scalable Texture Compression, 5x5 block, RGBA. 5.12 BPP.
			ASTC6x5,      //!< Adaptive Scalable Texture Compression, 6x5 block, RGBA. 4.27 BPP.
			ASTC6x6,      //!< Adaptive Scalable Texture Compression, 6x6 block, RGBA. 3.56 BPP.
			ASTC8x5,      //!< Adaptive Scalable Texture Compression, 8x5 block, RGBA. 3.20 BPP.
			ASTC8x6,      //!< Adaptive Scalable Texture Compression, 8x6 block, RGBA. 2.67 BPP.
			ASTC8x8,      //!< Adaptive Scalable Texture Compression, 8x8 block, RGBA. 2.00 BPP.
			ASTC10x5,     //!< Adaptive Scalable Texture Compression, 10x5 block, RGBA. 2.56 BPP.
			ASTC10x6,     //!< Adaptive Scalable Texture Compression, 10x6 block, RGBA. 2.13 BPP.
			ASTC10x8,     //!< Adaptive Scalable Texture Compression, 10x8 block, RGBA. 1.60 BPP.
			ASTC10x10,    //!< Adaptive Scalable Texture Compression, 10x10 block, RGBA. 1.28 BPP.
			ASTC12x10,    //!< Adaptive Scalable Texture Compression, 12x10 block, RGBA. 1.07 BPP.
			ASTC12x12,    //!< Adaptive Scalable Texture Compression, 12x12 block, RGBA. 0.89 BPP.

			Unknown,      //!< Compressed formats above.

			R1,           //!< 1-bit single-channel red. Monochrome, 1-bit per pixel. 1 BPP.
			A8,           //!< 8-bit single-channel alpha, unsigned normalized. 8 BPP.
			R8,           //!< 8-bit single-channel red, unsigned normalized. 8 BPP.
			R8I,          //!< 8-bit single-channel red, signed integer. 8 BPP.
			R8U,          //!< 8-bit single-channel red, unsigned integer. 8 BPP.
			R8S,          //!< 8-bit single-channel red, signed normalized. 8 BPP.
			R16,          //!< 16-bit single-channel red, unsigned normalized. 16 BPP.
			R16I,         //!< 16-bit single-channel red, signed integer. 16 BPP.
			R16U,         //!< 16-bit single-channel red, unsigned integer. 16 BPP.
			R16F,         //!< 16-bit single-channel red, half-precision floating point. 16 BPP.
			R16S,         //!< 16-bit single-channel red, signed normalized. 16 BPP.
			R32I,         //!< 32-bit single-channel red, signed integer. 32 BPP.
			R32U,         //!< 32-bit single-channel red, unsigned integer. 32 BPP.
			R32F,         //!< 32-bit single-channel red, full-precision floating point. 32 BPP.
			RG8,          //!< Two 8-bit channels (red, green), unsigned normalized. 16 BPP.
			RG8I,         //!< Two 8-bit channels (red, green), signed integer. 16 BPP.
			RG8U,         //!< Two 8-bit channels (red, green), unsigned integer. 16 BPP.
			RG8S,         //!< Two 8-bit channels (red, green), signed normalized. 16 BPP.
			RG16,         //!< Two 16-bit channels (red, green), unsigned normalized. 32 BPP.
			RG16I,        //!< Two 16-bit channels (red, green), signed integer. 32 BPP.
			RG16U,        //!< Two 16-bit channels (red, green), unsigned integer. 32 BPP.
			RG16F,        //!< Two 16-bit channels (red, green), half-precision floating point. 32 BPP.
			RG16S,        //!< Two 16-bit channels (red, green), signed normalized. 32 BPP.
			RG32I,        //!< Two 32-bit channels (red, green), signed integer. 64 BPP.
			RG32U,        //!< Two 32-bit channels (red, green), unsigned integer. 64 BPP.
			RG32F,        //!< Two 32-bit channels (red, green), full-precision floating point. 64 BPP.
			RGB8,         //!< Three 8-bit channels (red, green, blue), unsigned normalized. 24 BPP.
			RGB8I,        //!< Three 8-bit channels (red, green, blue), signed integer. 24 BPP.
			RGB8U,        //!< Three 8-bit channels (red, green, blue), unsigned integer. 24 BPP.
			RGB8S,        //!< Three 8-bit channels (red, green, blue), signed normalized. 24 BPP.
			RGB9E5F,      //!< Shared-exponent RGB. 9 bits per RGB channel with a shared 5-bit exponent, floating point. 32 BPP.
			BGRA8,        //!< Four 8-bit channels (blue, green, red, alpha), unsigned normalized. BGRA byte order. 32 BPP.
			RGBA8,        //!< Four 8-bit channels (red, green, blue, alpha), unsigned normalized. 32 BPP.
			RGBA8I,       //!< Four 8-bit channels (red, green, blue, alpha), signed integer. 32 BPP.
			RGBA8U,       //!< Four 8-bit channels (red, green, blue, alpha), unsigned integer. 32 BPP.
			RGBA8S,       //!< Four 8-bit channels (red, green, blue, alpha), signed normalized. 32 BPP.
			RGBA16,       //!< Four 16-bit channels (red, green, blue, alpha), unsigned normalized. 64 BPP.
			RGBA16I,      //!< Four 16-bit channels (red, green, blue, alpha), signed integer. 64 BPP.
			RGBA16U,      //!< Four 16-bit channels (red, green, blue, alpha), unsigned integer. 64 BPP.
			RGBA16F,      //!< Four 16-bit channels (red, green, blue, alpha), half-precision floating point. 64 BPP.
			RGBA16S,      //!< Four 16-bit channels (red, green, blue, alpha), signed normalized. 64 BPP.
			RGBA32I,      //!< Four 32-bit channels (red, green, blue, alpha), signed integer. 128 BPP.
			RGBA32U,      //!< Four 32-bit channels (red, green, blue, alpha), unsigned integer. 128 BPP.
			RGBA32F,      //!< Four 32-bit channels (red, green, blue, alpha), full-precision floating point. 128 BPP.
			B5G6R5,       //!< Packed 16-bit, 5-bit blue, 6-bit green, 5-bit red. BGR byte order, unsigned normalized. 16 BPP.
			R5G6B5,       //!< Packed 16-bit, 5-bit red, 6-bit green, 5-bit blue. RGB byte order, unsigned normalized. 16 BPP.
			BGRA4,        //!< Packed 16-bit, 4-bit per channel (blue, green, red, alpha). BGRA byte order, unsigned normalized. 16 BPP.
			RGBA4,        //!< Packed 16-bit, 4-bit per channel (red, green, blue, alpha), unsigned normalized. 16 BPP.
			BGR5A1,       //!< Packed 16-bit, 5-bit blue, 5-bit green, 5-bit red, 1-bit alpha. BGRA byte order, unsigned normalized. 16 BPP.
			RGB5A1,       //!< Packed 16-bit, 5-bit red, 5-bit green, 5-bit blue, 1-bit alpha, unsigned normalized. 16 BPP.
			RGB10A2,      //!< Packed 32-bit, 10-bit red, 10-bit green, 10-bit blue, 2-bit alpha, unsigned normalized. 32 BPP.
			RGB10A2U,     //!< Packed 32-bit, 10-bit red, 10-bit green, 10-bit blue, 2-bit alpha, unsigned integer. 32 BPP.
			RG11B10F,     //!< Packed 32-bit, 11-bit red, 11-bit green, 10-bit blue, unsigned floating point. No alpha. 32 BPP.

			UnknownDepth, //!< Depth formats below.

			D16,          //!< 16-bit depth, unsigned normalized. 16 BPP.
			D24,          //!< 24-bit depth, unsigned normalized (stored as 32-bit with 8 bits unused). 32 BPP.
			D24S8,        //!< 24-bit depth, unsigned normalized, with 8-bit stencil. 32 BPP.
			D32,          //!< 32-bit depth, unsigned normalized. 32 BPP.
			D16F,         //!< 16-bit depth, floating point. 16 BPP.
			D24F,         //!< 24-bit depth, floating point (stored as 32-bit). 32 BPP.
			D32F,         //!< 32-bit depth, floating point. 32 BPP.
			D32FS8,       //!< 32-bit depth, floating point, with 8-bit stencil (stored as 64-bit). 64 BPP.
			D0S8,         //!< 8-bit stencil only, no depth. 8 BPP.

			Count
		};
	};

	///
	struct Orientation
	{
		///
		enum Enum
		{
			R0,
			R90,
			R180,
			R270,
			HFlip,
			HFlipR90,
			HFlipR270,
			VFlip,
		};
	};

	/// Source image format an `ImageContainer` was parsed from.
	///
	struct ImageParser
	{
		/// Image formats:
		enum Enum
		{
			Avif,  //!< AV1 Image File Format.
			Bmp,   //!< Windows Bitmap.
			Dds,   //!< DirectDraw Surface.
			Exr,   //!< OpenEXR.
			Gif,   //!< Graphics Interchange Format.
			Hdr,   //!< Radiance RGBE.
			Heif,  //!< High Efficiency Image File Format.
			Jpeg,  //!< JPEG.
			Ktx,   //!< Khronos Texture.
			Ktx2,  //!< Khronos Texture 2.
			Pic,   //!< Softimage PIC.
			Png,   //!< Portable Network Graphics.
			Pnm,   //!< Portable AnyMap (PBM/PGM/PPM).
			Psd,   //!< Photoshop Document.
			Pvr3,  //!< PVR (v3).
			Tga,   //!< Truevision TGA.
			Webp,  //!< WebP.

			Count  //!< Unknown / not parsed.
		};
	};

	/// Texture info.
	///
	/// @attention C99 equivalent is `bgfx_texture_info_t`.
	///
	struct TextureInfo
	{
		TextureFormat::Enum format; //!< Texture format.
		uint32_t storageSize;       //!< Total amount of bytes required to store texture.
		uint16_t width;             //!< Texture width.
		uint16_t height;            //!< Texture height.
		uint16_t depth;             //!< Texture depth.
		uint16_t numLayers;         //!< Number of layers in texture array.
		uint8_t numMips;            //!< Number of MIP maps.
		uint8_t bitsPerPixel;       //!< Format bits per pixel.
		bool    cubeMap;            //!< Texture is cubemap.
	};

	/// 
	struct ImageContainer
	{
		bx::AllocatorI* m_allocator;
		void*           m_data;

		TextureFormat::Enum m_format;
		Orientation::Enum m_orientation;
		ImageParser::Enum m_parser;

		uint32_t m_size;
		uint32_t m_offset;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint16_t m_numLayers;
		uint8_t  m_numMips;
		bool     m_hasAlpha;
		bool     m_cubeMap;
		bool     m_ktx;
		bool     m_ktx2;
		bool     m_pvr3;
		bool     m_srgb;
	};

	///
	struct ImageMip
	{
		TextureFormat::Enum m_format;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint32_t m_blockSize;
		uint32_t m_size;
		uint8_t  m_bpp;
		bool     m_hasAlpha;
		const uint8_t* m_data;
	};

	///
	struct ImageBlockInfo
	{
		uint8_t bitsPerPixel;
		uint8_t blockWidth;
		uint8_t blockHeight;
		uint8_t blockSize;
		uint8_t minBlockX;
		uint8_t minBlockY;
		uint8_t depthBits;
		uint8_t stencilBits;
		uint8_t rBits;
		uint8_t gBits;
		uint8_t bBits;
		uint8_t aBits;
		uint8_t encoding;
	};

	/// Returns true if texture format is compressed.
	bool isCompressed(TextureFormat::Enum _format);

	/// Returns true if texture format is uncompressed.
	bool isColor(TextureFormat::Enum _format);

	/// Returns true if texture format is depth.
	bool isDepth(TextureFormat::Enum _format);

	/// Returns true if texture format is valid.
	bool isValid(TextureFormat::Enum _format);

	/// returns true if texture format encoding is float.
	bool isFloat(TextureFormat::Enum _format);

	/// Returns bits per pixel.
	uint8_t getBitsPerPixel(TextureFormat::Enum _format);

	/// Returns texture block info.
	const ImageBlockInfo& getBlockInfo(TextureFormat::Enum _format);

	/// Converts format to string.
	const char* getName(TextureFormat::Enum _format);

	/// Converts string to format.
	TextureFormat::Enum getFormat(const char* _name);

	/// Converts image source format to string.
	const char* getName(ImageParser::Enum _parser);

	/// Returns true if image is a volume (3D) texture.
	inline bool isVolume(const ImageContainer& _imageContainer)
	{
		return 0 != _imageContainer.m_depth;
	}

	/// Returns the number of depth slices; one for anything that is not a volume
	/// (3D) texture.
	inline uint32_t imageGetNumSlices(const ImageContainer& _imageContainer)
	{
		return 0 != _imageContainer.m_depth ? _imageContainer.m_depth : 1;
	}

	/// Returns number of mip-maps required for complete mip-map chain.
	uint8_t imageGetNumMips(
		  TextureFormat::Enum _format
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth = 0
		);

	/// Returns image size.
	uint64_t imageGetSize(
		  TextureInfo* _info
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, bool _cubeMap
		, bool _hasMips
		, uint16_t _numLayers
		, TextureFormat::Enum _format
		);

	///
	void imageSolid(
		  void* _dst
		, uint32_t _width
		, uint32_t _height
		, uint32_t _solid
		);

	///
	void imageCheckerboard(
		  void* _dst
		, uint32_t _width
		, uint32_t _height
		, uint32_t _step
		, uint32_t _0
		, uint32_t _1
		);

	///
	void imageRgba8Downsample2x2(
		  void* _dst
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _srcPitch
		, uint32_t _dstPitch
		, const void* _src
		);

	///
	void imageRgba32fToLinear(
		  void* _dst
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _srcPitch
		, const void* _src
		);

	///
	void imageRgba32fToLinear(ImageContainer* _imageContainer);

	///
	void imageRgba32fToGamma(
		  void* _dst
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _srcPitch
		, const void* _src
		);

	///
	void imageRgba32fToGamma(ImageContainer* _imageContainer);

	///
	void imageRgba32fLinearDownsample2x2(
		  void* _dst
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _srcPitch
		, const void* _src
		);

	///
	void imageRgba32fDownsample2x2(
		  void* _dst
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _srcPitch
		, const void* _src
		);

	///
	void imageRgba32fDownsample2x2NormalMap(
		  void* _dst
		, uint32_t _width
		, uint32_t _height
		, uint32_t _srcPitch
		, uint32_t _dstPitch
		, const void* _src
		);

	///
	void imageSwizzleBgra8(
		  void* _dst
		, uint32_t _dstPitch
		, uint32_t _width
		, uint32_t _height
		, const void* _src
		, uint32_t _srcPitch
		);

	///
	void imageCopy(
		  void* _dst
		, uint32_t _height
		, uint32_t _srcPitch
		, uint32_t _depth
		, const void* _src
		, uint32_t _dstPitch
		);

	///
	void imageCopy(
		  void* _dst
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _bpp
		, uint32_t _srcPitch
		, const void* _src
		);

	///
	PackFn getPack(TextureFormat::Enum _format);

	///
	UnpackFn getUnpack(TextureFormat::Enum _format);

	///
	bool imageConvert(
		  TextureFormat::Enum _dstFormat
		, TextureFormat::Enum _srcFormat
		);

	///
	void imageConvert(
		  void* _dst
		, uint32_t _bpp
		, PackFn _pack
		, const void* _src
		, UnpackFn _unpack
		, uint32_t _size
		);

	///
	void imageConvert(
		  void* _dst
		, uint32_t _dstBpp
		, PackFn _pack
		, const void* _src
		, uint32_t _srcBpp
		, UnpackFn _unpack
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _srcPitch
		, uint32_t _dstPitch
		);

	///
	bool imageConvert(
		  bx::AllocatorI* _allocator
		, void* _dst
		, TextureFormat::Enum _dstFormat
		, const void* _src
		, TextureFormat::Enum _srcFormat
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		);

	///
	ImageContainer* imageConvert(
		  bx::AllocatorI* _allocator
		, TextureFormat::Enum _dstFormat
		, const void* _src
		, uint32_t _size
		);

	///
	ImageContainer* imageConvert(
		  bx::AllocatorI* _allocator
		, TextureFormat::Enum _dstFormat
		, const ImageContainer& _input
		, bool _convertMips = true
		);

	///
	ImageContainer* imageAlloc(
		  bx::AllocatorI* _allocator
		, TextureFormat::Enum _format
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _numLayers
		, bool _cubeMap
		, bool _hasMips
		, const void* _data = NULL
		);

	///
	void imageFree(
		  ImageContainer* _imageContainer
		);

	///
	int32_t imageWriteTga(
		  bx::WriterI* _writer
		, uint32_t _width
		, uint32_t _height
		, uint32_t _srcPitch
		, const void* _src
		, bool _grayscale
		, bool _yflip
		, bx::Error* _err = NULL
		);

	///
	int32_t imageWritePng(
		  bx::WriterI* _writer
		, uint32_t _width
		, uint32_t _height
		, uint32_t _srcPitch
		, const void* _src
		, TextureFormat::Enum _format
		, bool _yflip
		, bx::Error* _err = NULL
		);

	///
	int32_t imageWriteExr(
		  bx::WriterI* _writer
		, uint32_t _width
		, uint32_t _height
		, uint32_t _srcPitch
		, const void* _src
		, TextureFormat::Enum _format
		, bool _yflip
		, bx::Error* _err
		);

	///
	int32_t imageWriteHdr(
		  bx::WriterI* _writer
		, uint32_t _width
		, uint32_t _height
		, uint32_t _srcPitch
		, const void* _src
		, TextureFormat::Enum _format
		, bool _yflip
		, bx::Error* _err
		);

	///
	int32_t imageWriteDds(
		  bx::WriterI* _writer
		, ImageContainer& _imageContainer
		, const void* _data
		, uint32_t _size
		, bx::Error* _err
		);

	///
	int32_t imageWriteKtx(
		  bx::WriterI* _writer
		, TextureFormat::Enum _format
		, bool _cubeMap
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint8_t _numMips
		, uint32_t _numLayers
		, bool _srgb
		, const void* _src
		, bx::Error* _err = NULL
		);

	///
	int32_t imageWriteKtx(
		  bx::WriterI* _writer
		, ImageContainer& _imageContainer
		, const void* _data
		, uint32_t _size
		, bx::Error* _err = NULL
		);

	///
	int32_t imageWriteKtx2(
		  bx::WriterI* _writer
		, TextureFormat::Enum _format
		, bool _cubeMap
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint8_t _numMips
		, uint32_t _numLayers
		, bool _srgb
		, const void* _src
		, bx::Error* _err = NULL
		);

	///
	int32_t imageWriteKtx2(
		  bx::WriterI* _writer
		, ImageContainer& _imageContainer
		, const void* _data
		, uint32_t _size
		, bx::Error* _err = NULL
		);

	///
	bool imageParse(
		  ImageContainer& _imageContainer
		, bx::ReaderSeekerI* _reader
		, bx::Error* _err
		);

	///
	bool imageParse(
		  ImageContainer& _imageContainer
		, const void* _data
		, uint32_t _size
		, bx::Error* _err = NULL
		);

	///
	ImageContainer* imageParseDds(
		  bx::AllocatorI* _allocator
		, const void* _src
		, uint32_t _size
		, bx::Error* _err
		);

	///
	ImageContainer* imageParseKtx(
		  bx::AllocatorI* _allocator
		, const void* _src
		, uint32_t _size
		, bx::Error* _err
		);

	///
	ImageContainer* imageParseKtx2(
		  bx::AllocatorI* _allocator
		, const void* _src
		, uint32_t _size
		, bx::Error* _err
		);

	///
	ImageContainer* imageParsePvr3(
		  bx::AllocatorI* _allocator
		, const void* _src
		, uint32_t _size
		, bx::Error* _err
		);

	///
	void imageDecodeToR8(
		  bx::AllocatorI* _allocator
		, void* _dst
		, const void* _src
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _dstPitch
		, TextureFormat::Enum _srcFormat
	);

	///
	void imageDecodeToBgra8(
		  bx::AllocatorI* _allocator
		, void* _dst
		, const void* _src
		, uint32_t _width
		, uint32_t _height
		, uint32_t _dstPitch
		, TextureFormat::Enum _format
		);

	///
	void imageDecodeToRgba8(
		  bx::AllocatorI* _allocator
		, void* _dst
		, const void* _src
		, uint32_t _width
		, uint32_t _height
		, uint32_t _dstPitch
		, TextureFormat::Enum _format
		);

	///
	void imageDecodeToRgba32f(
		  bx::AllocatorI* _allocator
		, void* _dst
		, const void* _src
		, uint32_t _width
		, uint32_t _height
		, uint32_t _depth
		, uint32_t _dstPitch
		, TextureFormat::Enum _format
		);

	///
	bool imageGetRawData(
		  const ImageContainer& _imageContainer
		, uint16_t _side
		, uint8_t _lod
		, const void* _data
		, uint32_t _size
		, ImageMip& _mip
		);

} // namespace bimg

#endif // BIMG_IMAGE_H_HEADER_GUARD
