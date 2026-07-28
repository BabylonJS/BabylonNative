/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bimg/blob/master/LICENSE
 */

#include <bx/bx.h>
#include <bx/allocator.h>
#include <bx/commandline.h>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include <bx/string.h>

#include <bimg/bimg.h>
#include <bimg/decode.h>

#define BIMG_META_VERSION_MAJOR 1
#define BIMG_META_VERSION_MINOR 0

static const char* orientationName(bimg::Orientation::Enum _orientation)
{
	switch (_orientation)
	{
	case bimg::Orientation::R0:        return "0";
	case bimg::Orientation::R90:       return "rotate 90";
	case bimg::Orientation::R180:      return "rotate 180";
	case bimg::Orientation::R270:      return "rotate 270";
	case bimg::Orientation::HFlip:     return "h-flip";
	case bimg::Orientation::HFlipR90:  return "h-flip+rotate 90";
	case bimg::Orientation::HFlipR270: return "h-flip+rotate 270";
	case bimg::Orientation::VFlip:     return "v-flip";
	default:                           break;
	}

	return "unknown";
}

static void version()
{
	bx::printf(
		  "meta, bimg image metadata tool, version %d.%d.%d.\n"
		  "Copyright 2011-2026 Branimir Karadzic. All rights reserved.\n"
		  "License: https://github.com/bkaradzic/bimg/blob/master/LICENSE\n"
		, BIMG_META_VERSION_MAJOR
		, BIMG_META_VERSION_MINOR
		, BIMG_API_VERSION
		);
}

static void help(const char* _error = NULL)
{
	if (NULL != _error)
	{
		bx::printf("Error:\n%s\n\n", _error);
	}

	version();

	bx::printf(
		  "\n"
		  "Usage: meta <file> [<file>...]\n"
		  "\n"
		  "Prints image metadata (format, dimensions, mips, etc.) for each input\n"
		  "file without decoding pixel data, similar to the UNIX `file` tool.\n"
		  "\n"
		  "Options:\n"
		  "  -h, --help    Display this help and exit.\n"
		  "  -v, --version Output version information and exit.\n"
		  "\n"
		  "For more information, see https://github.com/bkaradzic/bimg\n"
		);
}

static bool processFile(bx::AllocatorI* _allocator, const char* _filePath)
{
	bx::Error err;

	bx::FileReader reader;
	if (!bx::open(&reader, _filePath, &err) )
	{
		bx::printf("%s: cannot open file\n", _filePath);
		return false;
	}

	const uint32_t size = uint32_t(bx::getSize(&reader) );
	if (0 == size)
	{
		bx::close(&reader);
		bx::printf("%s: empty file\n", _filePath);
		return false;
	}

	void* data = bx::alloc(_allocator, size);
	bx::read(&reader, data, size, &err);
	bx::close(&reader);

	if (!err.isOk() )
	{
		bx::free(_allocator, data);
		bx::printf("%s: failed to read file\n", _filePath);
		return false;
	}

	bimg::ImageContainer info;
	const bool parsed = bimg::imageParseInfo(_allocator, info, data, size, &err);

	bx::free(_allocator, data);

	if (!parsed)
	{
		bx::printf("%s: not a recognized image (%S)\n", _filePath, &err.getMessage() );
		return false;
	}

	bx::printf("%s: %s image, %u x %u"
		, _filePath
		, bimg::getName(info.m_parser)
		, info.m_width
		, info.m_height
		);

	if (info.m_depth > 1)
	{
		bx::printf(" x %u", info.m_depth);
	}

	bx::printf(", %s", bimg::getName(info.m_format) );

	if (info.m_cubeMap)
	{
		bx::printf(", cubemap");
	}

	if (info.m_numLayers > 1)
	{
		bx::printf(", %u layers", info.m_numLayers);
	}

	if (info.m_numMips > 1)
	{
		bx::printf(", %u mips", info.m_numMips);
	}

	if (info.m_hasAlpha)
	{
		bx::printf(", alpha");
	}

	if (info.m_srgb)
	{
		bx::printf(", sRGB");
	}

	if (bimg::Orientation::R0 != info.m_orientation)
	{
		bx::printf(", %s", orientationName(info.m_orientation) );
	}

	bx::printf("\n");

	return true;
}

int main(int _argc, const char* _argv[])
{
	bx::CommandLine cmdLine(_argc, _argv);

	if (cmdLine.hasArg('v', "version") )
	{
		version();
		return bx::kExitSuccess;
	}

	if (cmdLine.hasArg('h', "help") )
	{
		help();
		return bx::kExitSuccess;
	}

	bx::DefaultAllocator allocator;

	bool any = false;
	bool ok  = true;

	for (int32_t ii = 1, num = cmdLine.getNum(); ii < num; ++ii)
	{
		const char* arg = cmdLine.get(ii);

		// Skip options; only positional arguments are treated as files.
		if ('-' == arg[0])
		{
			continue;
		}

		any = true;
		ok &= processFile(&allocator, arg);
	}

	if (!any)
	{
		help("No input file specified.");
		return bx::kExitFailure;
	}

	return ok ? bx::kExitSuccess : bx::kExitFailure;
}
