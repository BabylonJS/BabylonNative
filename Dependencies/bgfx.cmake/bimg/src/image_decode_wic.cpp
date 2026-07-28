/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bimg/blob/master/LICENSE
 */

#include "bimg_p.h"

#if BIMG_CONFIG_PARSE_WIC && BX_PLATFORM_WINDOWS

#include <bx/os.h>

#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#	define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#include <objbase.h>
#include <wincodec.h>

#define WIC_RELEASE(_ptr)              \
			BX_MACRO_BLOCK_BEGIN       \
				if (NULL != (_ptr) )   \
				{                      \
					(_ptr)->Release(); \
				}                      \
			BX_MACRO_BLOCK_END

namespace bimg
{
	static const GUID kCLSID_WICImagingFactory      = { 0xcacaf262, 0x9370, 0x4615, { 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a } };
	static const GUID kGUID_WICPixelFormat32bppRGBA = { 0xf5c7ad2d, 0x6a8d, 0x43dd, { 0xa7, 0xa8, 0xa2, 0x99, 0x35, 0x26, 0x1a, 0xe9 } };

	static ImageParser::Enum wicDetectFormat(const uint8_t* _data, uint32_t _size)
	{
		if (_size >= 8
		&&  0x89 == _data[0] && 'P' == _data[1] && 'N' == _data[2] && 'G' == _data[3])
		{
			return ImageParser::Png;
		}

		if (_size >= 3
		&&  0xff == _data[0] && 0xd8 == _data[1] && 0xff == _data[2])
		{
			return ImageParser::Jpeg;
		}

		if (_size >= 2
		&&  'B' == _data[0] && 'M' == _data[1])
		{
			return ImageParser::Bmp;
		}

		if (_size >= 6
		&&  'G' == _data[0] && 'I' == _data[1] && 'F' == _data[2])
		{
			return ImageParser::Gif;
		}

		return ImageParser::Count;
	}

	ImageContainer* imageParseWic(bx::AllocatorI* _allocator, const void* _data, uint32_t _size, bx::Error* _err)
	{
		BX_UNUSED(_err);

		const ImageParser::Enum format = wicDetectFormat( (const uint8_t*)_data, _size);

		if (false
		||  (ImageParser::Png  == format && BIMG_CONFIG_PARSE_PNG )
		||  (ImageParser::Jpeg == format && BIMG_CONFIG_PARSE_JPEG)
		||  (ImageParser::Bmp  == format && BIMG_CONFIG_PARSE_BMP )
		||  (ImageParser::Gif  == format && BIMG_CONFIG_PARSE_GIF )
		   )
		{
			return NULL;
		}

		void* wicDll = bx::dlopen("windowscodecs.dll");
		if (NULL == wicDll)
		{
			return NULL;
		}

		typedef HRESULT (WINAPI* PFN_DllGetClassObject)(REFCLSID, REFIID, LPVOID*);
		PFN_DllGetClassObject dllGetClassObject = bx::dlsym<PFN_DllGetClassObject>(wicDll, "DllGetClassObject");
		if (NULL == dllGetClassObject)
		{
			bx::dlclose(wicDll);
			return NULL;
		}

		void* ole32Dll = bx::dlopen("ole32.dll");
		typedef HRESULT (WINAPI* PFN_CoInitializeEx)(LPVOID, DWORD);
		typedef void    (WINAPI* PFN_CoUninitialize)(void);
		PFN_CoUninitialize coUninitialize = NULL;
		bool comNeedsUninit = false;

		if (NULL != ole32Dll)
		{
			PFN_CoInitializeEx coInitializeEx = bx::dlsym<PFN_CoInitializeEx>(ole32Dll, "CoInitializeEx");
			coUninitialize = bx::dlsym<PFN_CoUninitialize>(ole32Dll, "CoUninitialize");

			if (NULL != coInitializeEx)
			{
				const HRESULT hr = coInitializeEx(NULL, COINIT_MULTITHREADED);
				comNeedsUninit = (S_OK == hr || S_FALSE == hr) && NULL != coUninitialize;
			}
		}

		IClassFactory*      classFactory = NULL;
		IWICImagingFactory* factory      = NULL;
		HRESULT hr = dllGetClassObject(kCLSID_WICImagingFactory, __uuidof(IClassFactory), (void**)&classFactory);
		if (SUCCEEDED(hr) && NULL != classFactory)
		{
			classFactory->CreateInstance(NULL, __uuidof(IWICImagingFactory), (void**)&factory);
			classFactory->Release();
		}

		ImageContainer* image = NULL;
		if (NULL != factory)
		{
			IWICStream*            stream    = NULL;
			IWICBitmapDecoder*     decoder   = NULL;
			IWICBitmapFrameDecode* frame     = NULL;
			IWICFormatConverter*   converter = NULL;

			if (true
			&&  SUCCEEDED(factory->CreateStream(&stream) )
			&&  SUCCEEDED(stream->InitializeFromMemory( (WICInProcPointer)const_cast<void*>(_data), _size) )
			&&  SUCCEEDED(factory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnDemand, &decoder) )
			&&  SUCCEEDED(decoder->GetFrame(0, &frame) )
			&&  SUCCEEDED(factory->CreateFormatConverter(&converter) )
			&&  SUCCEEDED(converter->Initialize(frame, kGUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom) )
			   )
			{
				UINT width  = 0;
				UINT height = 0;
				frame->GetSize(&width, &height);

				if (0 < width
				&&  0 < height)
				{
					image = imageAlloc(_allocator, TextureFormat::RGBA8, width, height, 1, 1, false, false);
					if (NULL != image)
					{
						hr = converter->CopyPixels(NULL, width * 4, image->m_size, (BYTE*)image->m_data);
						if (FAILED(hr) )
						{
							imageFree(image);
							image = NULL;
						}
						else
						{
							image->m_parser = format;

							bool hasAlpha = false;
							const uint8_t* rgba = (const uint8_t*)image->m_data;
							for (uint32_t ii = 3; ii < image->m_size; ii += 4)
							{
								if (0xff != rgba[ii])
								{
									hasAlpha = true;
									break;
								}
							}

							image->m_hasAlpha = hasAlpha;
						}
					}
				}
			}

			WIC_RELEASE(converter);
			WIC_RELEASE(frame);
			WIC_RELEASE(decoder);
			WIC_RELEASE(stream);
			WIC_RELEASE(factory);
		}

		if (comNeedsUninit)
		{
			coUninitialize();
		}

		if (NULL != ole32Dll)
		{
			bx::dlclose(ole32Dll);
		}

		bx::dlclose(wicDll);

		return image;
	}

} // namespace bimg

#else

namespace bimg
{
	ImageContainer* imageParseWic(bx::AllocatorI* /*_allocator*/, const void* /*_data*/, uint32_t /*_size*/, bx::Error* /*_err*/)
	{
		return NULL;
	}

} // namespace bimg

#endif // BIMG_CONFIG_PARSE_WIC && BX_PLATFORM_WINDOWS
