/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/simd_t.h>

TEST_CASE("uint32_part", "[simd_t]")
{
	REQUIRE(UINT32_C(0x55555555) == bx::simd32_x32_part1by1(bx::simd32_splat(uint32_t(UINT16_MAX))).u32 );
	REQUIRE(UINT32_C(0x09249249) == bx::simd32_x32_part1by2(bx::simd32_splat(uint32_t(0x3ff))).u32 );
}

TEST_CASE("uint32_splat", "[simd_t]")
{
	REQUIRE(UINT32_C(0x01010101) == bx::simd32_splat(uint8_t(0x01)).u32 );
	REQUIRE(UINT32_C(0x55555555) == bx::simd32_splat(uint8_t(0x55)).u32 );
	REQUIRE(UINT32_C(0x13891389) == bx::simd32_splat(uint16_t(0x1389)).u32 );
}

TEST_CASE("uint64_splat", "[simd_t]")
{
	REQUIRE(UINT64_C(0x0101010101010101) == bx::simd64_splat(uint8_t(0x01)).u64 );
	REQUIRE(UINT64_C(0x5555555555555555) == bx::simd64_splat(uint8_t(0x55)).u64 );
	REQUIRE(UINT32_C(0x1389138913891389) == bx::simd64_splat(uint16_t(0x1389)).u64 );
	REQUIRE(UINT32_C(0x1506138915061389) == bx::simd64_splat(uint32_t(0x15061389)).u64 );
}

TEST_CASE("uint32_gcd", "[simd_t]")
{
	REQUIRE(1 == bx::simd32_u32_gcd(bx::simd32_splat(uint32_t(13)), bx::simd32_splat(uint32_t(89))).u32 );
	REQUIRE(3 == bx::simd32_u32_gcd(bx::simd32_splat(uint32_t( 3)), bx::simd32_splat(uint32_t( 9))).u32 );
	REQUIRE(8 == bx::simd32_u32_gcd(bx::simd32_splat(uint32_t( 8)), bx::simd32_splat(uint32_t(64))).u32 );
	REQUIRE(9 == bx::simd32_u32_gcd(bx::simd32_splat(uint32_t(18)), bx::simd32_splat(uint32_t(81))).u32 );
}

TEST_CASE("uint32_lcm", "[simd_t]")
{
	REQUIRE(1157 == bx::simd32_u32_lcm(bx::simd32_splat(uint32_t(13)), bx::simd32_splat(uint32_t(89))).u32 );
	REQUIRE(   9 == bx::simd32_u32_lcm(bx::simd32_splat(uint32_t( 3)), bx::simd32_splat(uint32_t( 9))).u32 );
	REQUIRE(  48 == bx::simd32_u32_lcm(bx::simd32_splat(uint32_t( 6)), bx::simd32_splat(uint32_t(16))).u32 );
	REQUIRE(  80 == bx::simd32_u32_lcm(bx::simd32_splat(uint32_t(16)), bx::simd32_splat(uint32_t(20))).u32 );
}

TEST_CASE("halfTo/FromFloat", "[simd_t]")
{
	for (uint32_t ii = 0; ii < 0x7c00; ++ii)
	{
		const uint16_t orig = uint16_t(ii);
		const float    htf = bx::halfToFloat(orig);
		const uint16_t hff = bx::halfFromFloat(htf);
		REQUIRE(orig == hff);
	}

	for (uint32_t ii = 0x8000; ii < 0xfc00; ++ii)
	{
		const uint16_t orig = uint16_t(ii);
		const float    htf = bx::halfToFloat(orig);
		const uint16_t hff = bx::halfFromFloat(htf);
		REQUIRE(orig == hff);
	}
}

TEST_CASE("uint32_testpow2", "[simd_t]")
{
	uint32_t shift = 0;
	uint32_t nextpow2 = bx::simd32_u32_nextpow2(bx::simd32_splat(uint32_t(1))).u32;

	for (uint32_t ii = 1; ii < 1<<24; ++ii)
	{
		REQUIRE(nextpow2 == bx::simd32_u32_nextpow2(bx::simd32_splat(ii)).u32 );

		if (bx::simd32_u32_testpow2(bx::simd32_splat(ii)).u32 )
		{
			REQUIRE(ii == 1u << shift);
			++shift;

			REQUIRE(ii == nextpow2);
			nextpow2 = bx::simd32_u32_nextpow2(bx::simd32_splat(ii+1)).u32;
		}
	}
}

TEST_CASE("uint32_roX", "[simd_t]")
{
	REQUIRE(bx::simd32_x32_rol(bx::simd32_splat(uint32_t(0x80000000)), 1).u32 == 1);
	REQUIRE(bx::simd32_x32_ror(bx::simd32_splat(uint32_t(1)), 1).u32 == 0x80000000);
}

TEST_CASE("uint64_roX", "[simd_t]")
{
	REQUIRE(bx::simd64_x64_rol(bx::simd64_splat(uint64_t(0x8000000000000000)), 1).u64 == 1);
	REQUIRE(bx::simd64_x64_ror(bx::simd64_splat(uint64_t(1)), 1).u64 == 0x8000000000000000);
}
