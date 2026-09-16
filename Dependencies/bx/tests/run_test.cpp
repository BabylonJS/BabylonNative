/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#define CATCH_CONFIG_RUNNER
#include "test.h"
#include <bx/string.h>
#include <bx/file.h>
#include <bx/simd_t.h>

BX_PRAGMA_DIAGNOSTIC_PUSH();
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4702); // warning C4702: unreachable code

bool testAssertHandler(const bx::Location& _location, uint32_t _skip, const char* _format, va_list _argList)
{
	bx::printf("%s(%d): ", _location.filePath, _location.line);
	bx::vprintf(_format, _argList);
	bx::printf("\n");

	uintptr_t stack[32];
	const uint32_t num = bx::getCallStackExact(2 /* skip self */ + _skip, BX_COUNTOF(stack), stack);
	bx::writeCallstack(bx::getStdOut(), stack, num, bx::ErrorIgnore{});

	// Throwing exceptions is required for testing asserts being trigged.
	// Use REQUIRE_ASSERTS to test asserts.
	throw std::exception();

	return true;
}

BX_PRAGMA_DIAGNOSTIC_POP();

int runAllTests(int32_t _argc, const char* _argv[])
{
	bx::setAssertHandler(testAssertHandler);

	bx::printf(
		"\nCompiler: " BX_COMPILER_NAME
		", CPU: " BX_CPU_NAME
		", Arch: " BX_ARCH_NAME
		", OS: " BX_PLATFORM_NAME
		", CRT: " BX_CRT_NAME
		", Features: " BX_CPP_NAME
#if BX_SIMD_SUPPORTED
		", SIMD"
#	if BX_SIMD_AVX
		", AVX"
#	endif // BX_SIMD_AVX
#	if BX_SIMD_LANGEXT
		", LangExt"
#	endif // BX_SIMD_LANGEXT
#	if BX_SIMD_NEON
		", Neon"
#	endif // BX_SIMD_NEON
#	if BX_SIMD_SSE
		", SSE"
#	endif // BX_SIMD_SSE
#endif // BX_SIMD_SUPPORTED

		", Date: " __DATE__
		", Time: " __TIME__
		"\n\n"
		);

	bx::printf("Args:\n");

	for (int32_t ii = 0; ii < _argc; ++ii)
	{
		bx::printf("\t%2d: \"%s\"\n", ii, _argv[ii]);
	}

	bx::printf("\n");

	using namespace Catch;

	Session session;

	ConfigData config;
	config.defaultColourMode = BX_PLATFORM_EMSCRIPTEN
		? ColourMode::None
		: ColourMode::PlatformDefault
		;
	config.showDurations = ShowDurations::Always;

	session.useConfigData(config);

	const int32_t result = session.run(_argc, _argv);

	return result;
}
