/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_TEST_H_HEADER_GUARD
#define BX_TEST_H_HEADER_GUARD

#include <exception>

// Override bx asserts in test builds: failing asserts throw std::exception so that Catch2's
// REQUIRE_ASSERTS (REQUIRE_THROWS) can catch them.
#if BX_CONFIG_DEBUG
#	define BX_ASSERT(_condition, ...)                    \
		do {                                             \
			if (!(_condition) )                          \
			{                                            \
				BX_PRAGMA_DIAGNOSTIC_PUSH();             \
				BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4297); \
				throw ::std::exception();                \
				BX_PRAGMA_DIAGNOSTIC_POP();              \
			}                                            \
		} while (false)

#	define BX_ASSERT_LOC(_location, _condition, ...) \
	do {                                             \
		(void)(_location);                           \
		if (!(_condition) )                          \
		{                                            \
			BX_PRAGMA_DIAGNOSTIC_PUSH();             \
			BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4297); \
			throw ::std::exception();                \
			BX_PRAGMA_DIAGNOSTIC_POP();              \
			}                                        \
		} while (false)
#endif // BX_CONFIG_DEBUG

#include <bx/bx.h>

BX_PRAGMA_DIAGNOSTIC_PUSH();
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4312); // warning C4312 : 'reinterpret_cast' : conversion from 'int' to 'const char *' of greater size
//BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG("-Wnan-infinity-disabled");
#include <catch/catch_amalgamated.hpp>
BX_PRAGMA_DIAGNOSTIC_POP();

#if BX_CONFIG_DEBUG
#	define REQUIRE_ASSERTS(_x) REQUIRE_THROWS(_x)
#else
#	define REQUIRE_ASSERTS(_x) BX_UNUSED(_x)
#endif // BX_CONFIG_DEBUG

#include "dbg.h"

#endif // BX_TEST_H_HEADER_GUARD
