/*
 * Copyright 2012-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/commandline.h>
#include <bx/readerwriter.h>
#include <bx/string.h>

TEST_CASE("commandLine", "")
{
	const char* args[] =
	{
		"-s",
		"--long",
		"--platform",
		"x",
		"--num", "1389",
		"--foo",
		"--", // it should not parse arguments after argument terminator
		"--bar",
	};

	bx::CommandLine cmdLine(BX_COUNTOF(args), args);

	REQUIRE( cmdLine.hasArg("long") );
	REQUIRE( cmdLine.hasArg('s') );

	int32_t num;
	REQUIRE(cmdLine.hasArg(num, '\0', "num") );
	REQUIRE(1389 == num);

	// test argument terminator
	REQUIRE( cmdLine.hasArg("foo") );
	REQUIRE(!cmdLine.hasArg("bar") );

	// non-existing argument
	REQUIRE(!cmdLine.hasArg('x') );
	REQUIRE(!cmdLine.hasArg("preprocess") );
}

static const bx::CommandLineOption s_options[] =
{
	{ 'o',  "output",  1, "<file path>", "Output file path."           },
	{ 'D',  "define",  1, "<define>",    "Define preprocessor macro."  },
	{ '\0', "lang",    1, "<language>",  "Output shading language."    },
	{ 'v',  "verbose", 0, NULL,          "Verbose output."             },
};

TEST_CASE("commandLine positional", "")
{
	{
		const char* args[] =
		{
			"tool", "-o", "out.bin", "--verbose", "in.txt",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(2 == cmdLine.getNumPositional() );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(0), "tool") );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(1), "in.txt") );
		REQUIRE(nullptr == cmdLine.getPositional(2) );

		REQUIRE(0 == bx::strCmp(cmdLine.findOption('o', "output", ""), "out.bin") );
	}

	{
		const char* args[] =
		{
			"tool", "in.txt", "--lang", "hlsl", "-D", "FOO=1", "-o", "out.bin",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(2 == cmdLine.getNumPositional() );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(1), "in.txt") );
	}

	{
		const char* args[] =
		{
			"tool", "-v", "--", "-o", "in.txt",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(3 == cmdLine.getNumPositional() );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(0), "tool") );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(1), "-o") );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(2), "in.txt") );
	}

	{
		const char* args[] =
		{
			"tool", "-", "-o", "-",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(2 == cmdLine.getNumPositional() );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(1), "-") );
		REQUIRE(0 == bx::strCmp(cmdLine.findOption('o', "output", ""), "-") );
	}

	{
		const char* args[] =
		{
			"tool", "-o", "-v",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args);

		REQUIRE(0 == bx::strCmp(cmdLine.findOption('o', "output", ""), "") );
	}

	{
		const char* args[] =
		{
			"tool", "in.txt", "-o",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(2 == cmdLine.getNumPositional() );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(1), "in.txt") );
	}

	{
		const char* args[] =
		{
			"tool", "in.txt", "-v",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(2 == cmdLine.getNumPositional() );
		REQUIRE(nullptr == cmdLine.getPositional(2) );
		REQUIRE(nullptr == cmdLine.findUnknownOption() );
	}

	{
		const char* args[] = { "tool" };

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(1 == cmdLine.getNumPositional() );
		REQUIRE(nullptr == cmdLine.getPositional(1) );
		REQUIRE(nullptr == cmdLine.getPositional(100) );
	}
}

TEST_CASE("commandLine optional param", "")
{
	static const bx::CommandLineOption options[] =
	{
		{ 'o',  "output",  1, "<file path>", "Output file path." },
		{ '\0', "bin2c", bx::kCommandLineOptionalParam, "[array name]",
		                                     "Generate C header. Defaults to input file base name." },
		{ 'v',  "verbose", 0, NULL,          "Verbose output."   },
	};

	{
		const char* args[] =
		{
			"tool", "in.sc", "--bin2c", "shader_glsl", "-o", "out.bin",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, options, BX_COUNTOF(options) );

		REQUIRE(cmdLine.hasArg("bin2c") );
		REQUIRE(0 == bx::strCmp(cmdLine.findOption("bin2c"), "shader_glsl") );

		// It's a parameter, so it's not a positional argument.
		REQUIRE(2 == cmdLine.getNumPositional() );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(1), "in.sc") );
		REQUIRE(nullptr == cmdLine.findUnknownOption() );
	}

	{
		const char* args[] =
		{
			"tool", "in.sc", "--bin2c", "-o", "out.bin",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, options, BX_COUNTOF(options) );

		REQUIRE(cmdLine.hasArg("bin2c") );
		REQUIRE(nullptr == cmdLine.findOption("bin2c") );

		REQUIRE(0 == bx::strCmp(cmdLine.findOption('o', "output", ""), "out.bin") );
		REQUIRE(2 == cmdLine.getNumPositional() );
		REQUIRE(0 == bx::strCmp(cmdLine.getPositional(1), "in.sc") );
		REQUIRE(nullptr == cmdLine.findUnknownOption() );
	}

	{
		const char* args[] =
		{
			"tool", "in.sc", "--bin2c",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, options, BX_COUNTOF(options) );

		REQUIRE(cmdLine.hasArg("bin2c") );
		REQUIRE(nullptr == cmdLine.findOption("bin2c") );
		REQUIRE(2 == cmdLine.getNumPositional() );
	}

	{
		const char* args[] =
		{
			"tool", "--bin2c", "in.sc",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, options, BX_COUNTOF(options) );

		REQUIRE(0 == bx::strCmp(cmdLine.findOption("bin2c"), "in.sc") );
		REQUIRE(1 == cmdLine.getNumPositional() );
	}
}

TEST_CASE("commandLine unknown option", "")
{
	{
		const char* args[] =
		{
			"tool", "-o", "out.bin", "--verbose", "in.txt",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(nullptr == cmdLine.findUnknownOption() );
	}

	{
		const char* args[] =
		{
			"tool", "--outupt", "out.bin", "in.txt",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(0 == bx::strCmp(cmdLine.findUnknownOption(), "--outupt") );
	}

	{
		const char* args[] =
		{
			"tool", "--", "--nonsense",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(nullptr == cmdLine.findUnknownOption() );
	}

	{
		const char* args[] =
		{
			"tool", "-D", "-nonsense",
		};

		bx::CommandLine cmdLine(BX_COUNTOF(args), args, s_options, BX_COUNTOF(s_options) );

		REQUIRE(nullptr == cmdLine.findUnknownOption() );
	}
}

TEST_CASE("commandLine write help", "")
{
	static const bx::CommandLineOption options[] =
	{
		{ 'h',  "help",    0, NULL,          "Help."                                     },
		{ 'f',  NULL,      1, "<file path>", "Input file path."                          },
		{ '\0', "mips",    0, NULL,          "Generate mip-maps."                        },
		{ '\0', "obb",     1, "<num>",       "Number of steps.\nLess steps, less precise." },
		{ '\0', "secret",  0, NULL,          NULL                                        },
		{ 'q',  "quality-of-the-encoding", 1, "<quality>", "Encoding quality."           },
	};

	char buffer[1024];
	bx::StaticMemoryBlockWriter writer(buffer, sizeof(buffer) );

	bx::Error err;
	const int32_t total = bx::write(&writer, options, BX_COUNTOF(options), &err);

	REQUIRE(err.isOk() );

	REQUIRE(0 == bx::strCmp(bx::StringView(buffer, total)
		, "  -h, --help               Help.\n"
		  "  -f <file path>           Input file path.\n"
		  "      --mips               Generate mip-maps.\n"
		  "      --obb <num>          Number of steps.\n"
		  "                           Less steps, less precise.\n"
		  "  -q, --quality-of-the-encoding <quality> Encoding quality.\n"
		) );
}

TEST_CASE("commandLine write help column", "")
{
	static const bx::CommandLineOption options[] =
	{
		{ 'h',  "help",       0, NULL,          "Help."                     },
		{ '\0', "varyingdef", 1, "<file path>", "varying.def.sc file path." },
	};

	char buffer[1024];
	bx::StaticMemoryBlockWriter writer(buffer, sizeof(buffer) );

	bx::Error err;
	const int32_t total = bx::write(&writer, options, BX_COUNTOF(options), &err);

	REQUIRE(err.isOk() );

	REQUIRE(0 == bx::strCmp(bx::StringView(buffer, total)
		, "  -h, --help                    Help.\n"
		  "      --varyingdef <file path>  varying.def.sc file path.\n"
		) );
}

static bool test(const char* _input, int32_t _argc, ...)
{
	char buffer[1024];
	uint32_t len = sizeof(buffer);
	char* argv[32];
	int32_t argc;
	bx::tokenizeCommandLine(_input, buffer, len, argc, argv, BX_COUNTOF(argv) );

	if (_argc != argc)
	{
		return false;
	}

	va_list argList;
	va_start(argList, _argc);

	for (int32_t ii = 0; ii < _argc; ++ii)
	{
		const char* arg = va_arg(argList, const char*);
		if (0 != bx::strCmp(argv[ii], arg) )
		{
			return false;
		}
	}

	va_end(argList);

	return true;
}

TEST_CASE("tokenizeCommandLine", "")
{
	REQUIRE(test("      ", 0, NULL) );
	REQUIRE(test("\\",     0, NULL) );

	REQUIRE(test("a b v g d", 5, "a", "b", "v", "g", "d") );
	REQUIRE(test("a \t b",    2, "a", "b") );

	REQUIRE(test("\"a b\" v",  2, "a b",  "v") );
	REQUIRE(test("a\"b v\"g d", 2, "ab vg", "d") );
	REQUIRE(test("\"\" v",    2, "",     "v") );

	REQUIRE(test("a\\b",   1, "a\\b") );
	REQUIRE(test("\\\\",   1, "\\\\") );

	REQUIRE(test("\"ab\\\"v\" \"\\\\\" g", 3, "ab\"v",    "\\",   "g") );
	REQUIRE(test("a\\\\\\\"b v g",         3, "a\\\"b",   "v",  "g") );
	REQUIRE(test("a\\\\\\\\\"b v\" g d",   3, "a\\\\b v", "g",  "d") );
}

TEST_CASE("tokenizeCommandLine terminator", "")
{
	char buffer[1024];
	char* argv[32];
	int32_t argc;

	const bx::StringView input("a b\nv\n\ng\n");

	bx::StringView next = input;
	uint32_t len = sizeof(buffer);

	next = bx::tokenizeCommandLine(next, buffer, len, argc, argv, BX_COUNTOF(argv), '\n');
	REQUIRE(2 == argc);
	REQUIRE(0 == bx::strCmp(argv[0], "a") );
	REQUIRE(0 == bx::strCmp(argv[1], "b") );
	REQUIRE(0 == bx::strCmp(next, "v\n\ng\n") );

	next = bx::tokenizeCommandLine(next, buffer, len, argc, argv, BX_COUNTOF(argv), '\n');
	REQUIRE(1 == argc);
	REQUIRE(0 == bx::strCmp(argv[0], "v") );

	// empty line
	next = bx::tokenizeCommandLine(next, buffer, len, argc, argv, BX_COUNTOF(argv), '\n');
	REQUIRE(0 == argc);
	REQUIRE(0 == bx::strCmp(next, "g\n") );

	next = bx::tokenizeCommandLine(next, buffer, len, argc, argv, BX_COUNTOF(argv), '\n');
	REQUIRE(1 == argc);
	REQUIRE(0 == bx::strCmp(argv[0], "g") );
	REQUIRE(next.isEmpty() );
}

TEST_CASE("tokenizeCommandLine terminator is ordinary character", "")
{
	char buffer[1024];
	uint32_t len = sizeof(buffer);
	char* argv[32];
	int32_t argc;

	bx::StringView next = bx::tokenizeCommandLine("a b;v g", buffer, len, argc, argv, BX_COUNTOF(argv), ';');
	REQUIRE(2 == argc);
	REQUIRE(0 == bx::strCmp(argv[0], "a") );
	REQUIRE(0 == bx::strCmp(argv[1], "b") );
	REQUIRE(0 == bx::strCmp(next, "v g") );

	const char input[] = { 'a', '\0', 'b', ' ', 'v' };
	next = bx::tokenizeCommandLine(bx::StringView(input, BX_COUNTOF(input) ), buffer, len, argc, argv, BX_COUNTOF(argv), '\n');
	REQUIRE(2 == argc);
	REQUIRE(0 == bx::strCmp(argv[0], "a") );
	REQUIRE(0 == bx::strCmp(argv[1], "v") );
}

TEST_CASE("tokenizeCommandLine maxArgvs", "")
{
	char buffer[1024];
	uint32_t len = sizeof(buffer);
	char* argv[2];
	int32_t argc;

	const bx::StringView next = bx::tokenizeCommandLine("a b v g d", buffer, len, argc, argv, BX_COUNTOF(argv) );
	REQUIRE(2 == argc);
	REQUIRE(0 == bx::strCmp(argv[0], "a") );
	REQUIRE(0 == bx::strCmp(argv[1], "b") );
	REQUIRE(0 == bx::strCmp(next, "v g d") );
}
