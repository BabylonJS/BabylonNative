/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/bx.h>
#include <bx/string.h>

static_assert(false
	|| BX_CRT_BIONIC
	|| BX_CRT_GLIBC
	|| BX_CRT_LIBCXX
	|| BX_CRT_MINGW
	|| BX_CRT_MSVC
	|| BX_CRT_NEWLIB
	|| BX_CRT_NONE
	);

static_assert(1 == BX_VA_ARGS_COUNT(1) );
static_assert(2 == BX_VA_ARGS_COUNT(1, 2) );
static_assert(3 == BX_VA_ARGS_COUNT(1, 2, 3) );
static_assert(4 == BX_VA_ARGS_COUNT(1, 2, 3, 4) );
static_assert(5 == BX_VA_ARGS_COUNT(1, 2, 3, 4, 5) );
static_assert(6 == BX_VA_ARGS_COUNT(1, 2, 3, 4, 5, 6) );

BX_NO_INLINE void unusedFunction()
{
	CHECK(false);
}

void testAssert()
{
BX_PRAGMA_DIAGNOSTIC_PUSH();
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4804); // warning C4804: '%': unsafe use of type 'bool' in operation)

	BX_ASSERT(false % 1, "Assert works!");

BX_PRAGMA_DIAGNOSTIC_POP();
}

TEST_CASE("Macros", "")
{
	uint32_t unused0;
	BX_UNUSED(unused0);

	uint32_t unused1;
	BX_UNUSED(unused0, unused1);

	uint32_t unused2;
	BX_UNUSED(unused0, unused1, unused2, unusedFunction() );

	REQUIRE(1 == BX_VA_ARGS_COUNT(1) );
	REQUIRE(2 == BX_VA_ARGS_COUNT(1, 2) );
	REQUIRE(3 == BX_VA_ARGS_COUNT(1, 2, 3) );
	REQUIRE(4 == BX_VA_ARGS_COUNT(1, 2, 3, 4) );
	REQUIRE(5 == BX_VA_ARGS_COUNT(1, 2, 3, 4, 5) );
	REQUIRE(6 == BX_VA_ARGS_COUNT(1, 2, 3, 4, 5, 6) );

	REQUIRE(0 == bx::strCmp(BX_STRINGIZE(TEST 1234 % 1 ^&*), "TEST 1234 % 1 ^&*") );

	{
		struct PodStruct { int32_t x, y, z; };
		REQUIRE(0 == BX_OFFSETOF(PodStruct, x) );
		REQUIRE(4 == BX_OFFSETOF(PodStruct, y) );
		REQUIRE(8 == BX_OFFSETOF(PodStruct, z) );
	}

	{
		union PodUnion { int32_t x, y, z; };
		REQUIRE(BX_OFFSETOF(PodUnion, x) == BX_OFFSETOF(PodUnion, y) );
		REQUIRE(BX_OFFSETOF(PodUnion, y) == BX_OFFSETOF(PodUnion, z) );
	}

	{
		struct NonPodStruct { NonPodStruct() { } int32_t x, y, z; };
		REQUIRE(0 == BX_OFFSETOF(NonPodStruct, x) );
		REQUIRE(4 == BX_OFFSETOF(NonPodStruct, y) );
		REQUIRE(8 == BX_OFFSETOF(NonPodStruct, z) );
	}

	{
		union NonPodUnion { NonPodUnion() { } int32_t x, y, z; };
		REQUIRE(BX_OFFSETOF(NonPodUnion, x) == BX_OFFSETOF(NonPodUnion, y) );
		REQUIRE(BX_OFFSETOF(NonPodUnion, y) == BX_OFFSETOF(NonPodUnion, z) );
	}

	REQUIRE_ASSERTS(testAssert() );
}
