/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/simd_t.h>
#include <bx/math.h>
#include <bx/string.h>

#if 0
#	define SIMD_DBG DBG
#else
#	define SIMD_DBG unused
#endif // 0

using namespace bx;

inline void unused(...) {}

union simd_cast
{
	bx::simd256_t simd256;
	bx::simd128_t simd128;
	float    f[8];
	uint32_t ui[8];
	int32_t  i[8];
	char     c[32];
};

void simd_check_bool(const char* _str, bool _a, bool _0)
{
	SIMD_DBG("%s %d == %d"
		, _str
		, _a
		, _0
		);

	REQUIRE(_a == _0);
}

void simd_check_int32(
	  const char* _str
	, bx::simd128_t _a
	, int32_t _0
	, int32_t _1
	, int32_t _2
	, int32_t _3
	)
{
	simd_cast c; c.simd128 = _a;
	SIMD_DBG("%s (%d, %d, %d, %d) == (%d, %d, %d, %d)"
		, _str
		, c.i[0], c.i[1], c.i[2], c.i[3]
		, _0, _1, _2, _3
		);

	REQUIRE(c.i[0] == _0);
	REQUIRE(c.i[1] == _1);
	REQUIRE(c.i[2] == _2);
	REQUIRE(c.i[3] == _3);
}

void simd_check_int32(
	  const char* _str
	, bx::simd256_t _a
	, int32_t _0
	, int32_t _1
	, int32_t _2
	, int32_t _3
	, int32_t _4
	, int32_t _5
	, int32_t _6
	, int32_t _7
	)
{
	simd_cast c; c.simd256 = _a;
	SIMD_DBG("%s (%d, %d, %d, %d, %d, %d, %d, %d) == (%d, %d, %d, %d, %d, %d, %d, %d)"
		, _str
		, c.i[0], c.i[1], c.i[2], c.i[3], c.i[4], c.i[5], c.i[6], c.i[7]
		, _0, _1, _2, _3, _4, _5, _6, _7
		);

	REQUIRE(c.i[0] == _0);
	REQUIRE(c.i[1] == _1);
	REQUIRE(c.i[2] == _2);
	REQUIRE(c.i[3] == _3);
	REQUIRE(c.i[4] == _4);
	REQUIRE(c.i[5] == _5);
	REQUIRE(c.i[6] == _6);
	REQUIRE(c.i[7] == _7);
}

void simd_check_uint32(
	  const char* _str
	, bx::simd128_t _a
	, uint32_t _0
	, uint32_t _1
	, uint32_t _2
	, uint32_t _3
	)
{
	simd_cast c; c.simd128 = _a;

	SIMD_DBG("%s (0x%08x, 0x%08x, 0x%08x, 0x%08x) == (0x%08x, 0x%08x, 0x%08x, 0x%08x)"
		, _str
		, c.ui[0], c.ui[1], c.ui[2], c.ui[3]
		, _0, _1, _2, _3
		);

	REQUIRE(c.ui[0] == _0);
	REQUIRE(c.ui[1] == _1);
	REQUIRE(c.ui[2] == _2);
	REQUIRE(c.ui[3] == _3);
}

void simd_check_uint32(
	  const char* _str
	, bx::simd256_t _a
	, uint32_t _0
	, uint32_t _1
	, uint32_t _2
	, uint32_t _3
	, uint32_t _4
	, uint32_t _5
	, uint32_t _6
	, uint32_t _7
	)
{
	simd_cast c; c.simd256 = _a;

	SIMD_DBG("%s (0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x) == (0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)"
		, _str
		, c.ui[0], c.ui[1], c.ui[2], c.ui[3], c.ui[4], c.ui[5], c.ui[6], c.ui[7]
		, _0, _1, _2, _3, _4, _5, _6, _7
		);

	REQUIRE(c.ui[0] == _0);
	REQUIRE(c.ui[1] == _1);
	REQUIRE(c.ui[2] == _2);
	REQUIRE(c.ui[3] == _3);
	REQUIRE(c.ui[4] == _4);
	REQUIRE(c.ui[5] == _5);
	REQUIRE(c.ui[6] == _6);
	REQUIRE(c.ui[7] == _7);
}

void simd_check_float(
	  const char* _str
	, bx::simd128_t _a
	, float _0
	, float _1
	, float _2
	, float _3
	)
{
	simd_cast c; c.simd128 = _a;

	SIMD_DBG("%s (%f, %f, %f, %f) == (%f, %f, %f, %f)"
		, _str
		, c.f[0], c.f[1], c.f[2], c.f[3]
		, _0, _1, _2, _3
		);

	CHECK(bx::isEqual(c.f[0], _0, 0.0001f) );
	CHECK(bx::isEqual(c.f[1], _1, 0.0001f) );
	CHECK(bx::isEqual(c.f[2], _2, 0.0001f) );
	CHECK(bx::isEqual(c.f[3], _3, 0.0001f) );
}

void simd_check_float(
	  const char* _str
	, bx::simd256_t _a
	, float _0
	, float _1
	, float _2
	, float _3
	, float _4
	, float _5
	, float _6
	, float _7
	)
{
	simd_cast c; c.simd256 = _a;

	SIMD_DBG("%s (%f, %f, %f, %f, %f, %f, %f, %f) == (%f, %f, %f, %f, %f, %f, %f, %f)"
		, _str
		, c.f[0], c.f[1], c.f[2], c.f[3], c.f[4], c.f[5], c.f[6], c.f[7]
		, _0, _1, _2, _3, _4, _5, _6, _7
		);

	CHECK(bx::isEqual(c.f[0], _0, 0.0001f) );
	CHECK(bx::isEqual(c.f[1], _1, 0.0001f) );
	CHECK(bx::isEqual(c.f[2], _2, 0.0001f) );
	CHECK(bx::isEqual(c.f[3], _3, 0.0001f) );
	CHECK(bx::isEqual(c.f[4], _4, 0.0001f) );
	CHECK(bx::isEqual(c.f[5], _5, 0.0001f) );
	CHECK(bx::isEqual(c.f[6], _6, 0.0001f) );
	CHECK(bx::isEqual(c.f[7], _7, 0.0001f) );
}

void simd_check_string(const char* _str, bx::simd128_t _a)
{
	simd_cast c; c.simd128 = _a;
	const char test[5] = { c.c[0], c.c[4], c.c[8], c.c[12], '\0' };

	SIMD_DBG("%s %s", _str, test);

	CHECK(0 == bx::strCmp(_str, test) );
}

TEST_CASE("simd_swizzle", "[simd]")
{
	const simd128_t xyzw = simd128_ld(0x78787878, 0x79797979, 0x7a7a7a7a, 0x77777777);

#define ELEMx 0
#define ELEMy 1
#define ELEMz 2
#define ELEMw 3
#define BX_SIMD128_IMPLEMENT_SWIZZLE(_x, _y, _z, _w) \
			simd_check_string("" #_x #_y #_z #_w "", simd128_x32_swiz_##_x##_y##_z##_w(xyzw) ); \

#include <bx/inline/simd128_swizzle.inl>

#undef BX_SIMD128_IMPLEMENT_SWIZZLE
#undef ELEMw
#undef ELEMz
#undef ELEMy
#undef ELEMx
}

TEST_CASE("simd_compare", "[simd]")
{
	simd_check_uint32("cmpeq"
		, simd128_f32_cmpeq(simd128_ld(1.0f, 2.0f, 3.0f, 4.0f), simd128_ld(0.0f, 2.0f, 0.0f, 3.0f) )
		, 0, 0xffffffff, 0, 0
		);

	simd_check_uint32("cmplt"
		, simd128_f32_cmplt(simd128_ld(1.0f, 2.0f, 3.0f, 4.0f), simd128_ld(0.0f, 2.0f, 0.0f, 3.0f) )
		, 0, 0, 0, 0
		);

	simd_check_uint32("cmple"
		, simd128_f32_cmple(simd128_ld(1.0f, 2.0f, 3.0f, 4.0f), simd128_ld(0.0f, 2.0f, 0.0f, 3.0f) )
		, 0, 0xffffffff, 0, 0
		);

	simd_check_uint32("cmpgt"
		, simd128_f32_cmpgt(simd128_ld(1.0f, 2.0f, 3.0f, 4.0f), simd128_ld(0.0f, 2.0f, 0.0f, 3.0f) )
		, 0xffffffff, 0, 0xffffffff, 0xffffffff
		);

	simd_check_uint32("cmpge"
		, simd128_f32_cmpge(simd128_ld(1.0f, 2.0f, 3.0f, 4.0f), simd128_ld(0.0f, 2.0f, 0.0f, 3.0f) )
		, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
		);

	simd_check_uint32("icmpeq"
		, simd128_i32_cmpeq(simd128_ld(0u, 1u, 2u, 3u), simd128_ld(0u, uint32_t(-2), 1u, 3u) )
		, 0xffffffff, 0, 0, 0xffffffff
		);

	simd_check_uint32("icmplt"
		, simd128_i32_cmplt(simd128_ld(0u, 1u, 2u, 3u), simd128_ld(0u, uint32_t(-2), 1u, 3u) )
		, 0, 0, 0, 0
		);

	simd_check_uint32("icmpgt"
		, simd128_i32_cmpgt(simd128_ld(0u, 1u, 2u, 3u), simd128_ld(0u, uint32_t(-2), 1u, 3u) )
		, 0, 0xffffffff, 0xffffffff, 0
		);
}

TEST_CASE("simd_test", "[simd]")
{
	simd_check_bool("test_any_xyzw"
		, simd128_test_any_xyzw(simd128_ld(0xffffffff, 0u, 0u, 0u) )
		, true
		);

	simd_check_bool("test_all_xyzw"
		, simd128_test_all_xyzw(simd128_ld(0xffffffff, 0u, 0xffffffff, 0u) )
		, false
		);

	simd_check_bool("test_all_xyzw"
		, simd128_test_all_xyzw(simd128_ld(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff) )
		, true
		);

	simd_check_bool("test_all_xw"
		, simd128_test_all_xw(simd128_ld(0xffffffff, 0u, 0u, 0xffffffff) )
		, true
		);

	simd_check_bool("test_all_xzw"
		, simd128_test_all_xzw(simd128_ld(0xffffffff, 0u, 0u, 0xffffffff) )
		, false
		);
}

TEST_CASE("simd_load", "[simd]")
{
	simd_check_float("ld"
		, simd128_ld(0.0f, 1.0f, 2.0f, 3.0f)
		, 0.0f, 1.0f, 2.0f, 3.0f
		);

	simd_check_float("ld"
		, simd256_ld<simd256_t>(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f)
		, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f
		);

	simd_check_int32("ild"
		, simd128_ld(uint32_t(-1), 0u, 1u, 2u)
		, uint32_t(-1), 0, 1, 2
		);

	simd_check_int32("ild"
		, simd256_ld<simd256_t>(uint32_t(-1), 0u, 1u, 2u, 3u, 4u, 5u, 6u)
		, uint32_t(-1), 0, 1, 2, 3, 4, 5, 6
		);

	simd_check_int32("ild"
		, simd128_ld(uint32_t(-1), uint32_t(-2), uint32_t(-3), uint32_t(-4) )
		, uint32_t(-1), uint32_t(-2), uint32_t(-3), uint32_t(-4)
		);

	simd_check_uint32("zero", simd128_zero()
		, 0, 0, 0, 0
		);

	simd_check_uint32("isplat", simd128_splat<simd128_t>(0x80000001)
		, 0x80000001, 0x80000001, 0x80000001, 0x80000001
		);

	simd_check_float("splat", simd128_splat<simd128_t>(1.0f)
		, 1.0f, 1.0f, 1.0f, 1.0f
		);

	simd_check_uint32("isplat", simd256_splat<simd256_t>(0x80000001)
		, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001, 0x80000001
		);

	simd_check_float("splat", simd256_splat<simd256_t>(1.0f)
		, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
		);
}

TEST_CASE("simd128_f32_sqrt_nr", "[simd]")
{
	simd_check_float("simd128_f32_sqrt"
		, simd128_f32_sqrt(simd128_ld(1.0f, 16.0f, 65536.0f, 123456.0f) )
		, 1.0f, 4.0f, 256.0f, 351.363060096f
		);

	simd_check_float("simd_f32_sqrt_nr_ni"
		, simd_f32_sqrt_nr_ni(simd128_ld(1.0f, 16.0f, 65536.0f, 123456.0f) )
		, 1.0f, 4.0f, 256.0f, 351.363060096f
		);

	simd_check_float("simd_f32_sqrt_nr1_ni"
		, simd_f32_sqrt_nr1_ni(simd128_ld(1.0f, 16.0f, 65536.0f, 123456.0f) )
		, 1.0f, 4.0f, 256.0f, 351.363060096f
		);
}

union simd128_cast
{
	simd128_t simd;
	float     f[4];
	uint32_t  u[4];
	int32_t   i[4];
	int16_t   i16[8];
	uint16_t  u16[8];
	int8_t    i8[16];
	uint8_t   u8[16];
};

static void check_f32(const char* _name, simd128_t _a, float _0, float _1, float _2, float _3)
{
	BX_UNUSED(_name);
	simd128_cast c; c.simd = _a;
	REQUIRE(c.f[0] == Catch::Approx(_0).margin(0.0001f));
	REQUIRE(c.f[1] == Catch::Approx(_1).margin(0.0001f));
	REQUIRE(c.f[2] == Catch::Approx(_2).margin(0.0001f));
	REQUIRE(c.f[3] == Catch::Approx(_3).margin(0.0001f));
}

static void check_i32(const char* _name, simd128_t _a, int32_t _0, int32_t _1, int32_t _2, int32_t _3)
{
	BX_UNUSED(_name);
	simd128_cast c; c.simd = _a;
	REQUIRE(c.i[0] == _0);
	REQUIRE(c.i[1] == _1);
	REQUIRE(c.i[2] == _2);
	REQUIRE(c.i[3] == _3);
}

static void check_u32(const char* _name, simd128_t _a, uint32_t _0, uint32_t _1, uint32_t _2, uint32_t _3)
{
	BX_UNUSED(_name);
	simd128_cast c; c.simd = _a;
	REQUIRE(c.u[0] == _0);
	REQUIRE(c.u[1] == _1);
	REQUIRE(c.u[2] == _2);
	REQUIRE(c.u[3] == _3);
}

TEST_CASE("simd128_load_store", "[simd]")
{
	BX_ALIGN_DECL_16(float data[4]) = { 1.0f, 2.0f, 3.0f, 4.0f };
	const simd128_t a = simd128_ld<simd128_t>(data);
	check_f32("ld", a, 1.0f, 2.0f, 3.0f, 4.0f);

	BX_ALIGN_DECL_16(float out[4]);
	simd128_st(out, a);
	REQUIRE(out[0] == 1.0f);
	REQUIRE(out[1] == 2.0f);
	REQUIRE(out[2] == 3.0f);
	REQUIRE(out[3] == 4.0f);
}

TEST_CASE("simd128_f32_ld", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(10.0f, 20.0f, 30.0f, 40.0f);
	check_f32("f32_ld", a, 10.0f, 20.0f, 30.0f, 40.0f);
}

TEST_CASE("simd128_u32_ld", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0x01u, 0x02u, 0x03u, 0x04u);
	check_u32("u32_ld", a, 0x01, 0x02, 0x03, 0x04);
}

TEST_CASE("simd128_f32_splat", "[simd]")
{
	const simd128_t a = simd128_splat<simd128_t>(42.0f);
	check_f32("f32_splat", a, 42.0f, 42.0f, 42.0f, 42.0f);
}

TEST_CASE("simd128_u32_splat", "[simd]")
{
	const simd128_t a = simd128_splat<simd128_t>(0xdeadbeef);
	check_u32("u32_splat", a, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef);
}

TEST_CASE("simd128_zero", "[simd]")
{
	const simd128_t a = simd128_zero<simd128_t>();
	check_u32("zero", a, 0, 0, 0, 0);
}

TEST_CASE("simd128_f32_xyzw", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	REQUIRE(simd128_f32_x(a) == 1.0f);
	REQUIRE(simd128_f32_y(a) == 2.0f);
	REQUIRE(simd128_f32_z(a) == 3.0f);
	REQUIRE(simd128_f32_w(a) == 4.0f);
}

TEST_CASE("simd128_f32_add", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	const simd128_t b = simd128_ld<simd128_t>(5.0f, 6.0f, 7.0f, 8.0f);
	check_f32("f32_add", simd128_f32_add(a, b), 6.0f, 8.0f, 10.0f, 12.0f);
}

TEST_CASE("simd128_f32_sub", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(5.0f, 8.0f, 10.0f, 12.0f);
	const simd128_t b = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	check_f32("f32_sub", simd128_f32_sub(a, b), 4.0f, 6.0f, 7.0f, 8.0f);
}

TEST_CASE("simd128_f32_mul", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(2.0f, 3.0f, 4.0f, 5.0f);
	const simd128_t b = simd128_ld<simd128_t>(3.0f, 4.0f, 5.0f, 6.0f);
	check_f32("f32_mul", simd128_f32_mul(a, b), 6.0f, 12.0f, 20.0f, 30.0f);
}

TEST_CASE("simd128_f32_div", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(10.0f, 20.0f, 30.0f, 40.0f);
	const simd128_t b = simd128_ld<simd128_t>(2.0f, 4.0f, 5.0f, 8.0f);
	check_f32("f32_div", simd128_f32_div(a, b), 5.0f, 5.0f, 6.0f, 5.0f);
}

TEST_CASE("simd128_f32_madd", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(2.0f, 3.0f, 4.0f, 5.0f);
	const simd128_t b = simd128_ld<simd128_t>(3.0f, 4.0f, 5.0f, 6.0f);
	const simd128_t c = simd128_ld<simd128_t>(1.0f, 1.0f, 1.0f, 1.0f);
	// a*b+c
	check_f32("f32_madd", simd128_f32_madd(a, b, c), 7.0f, 13.0f, 21.0f, 31.0f);
}

TEST_CASE("simd128_f32_nmsub", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(2.0f, 3.0f, 4.0f, 5.0f);
	const simd128_t b = simd128_ld<simd128_t>(3.0f, 4.0f, 5.0f, 6.0f);
	const simd128_t c = simd128_ld<simd128_t>(10.0f, 20.0f, 30.0f, 40.0f);
	// c - a*b
	check_f32("f32_nmsub", simd128_f32_nmsub(a, b, c), 4.0f, 8.0f, 10.0f, 10.0f);
}

TEST_CASE("simd128_f32_neg", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, -2.0f, 3.0f, -4.0f);
	check_f32("f32_neg", simd128_f32_neg(a), -1.0f, 2.0f, -3.0f, 4.0f);
}

TEST_CASE("simd128_f32_abs", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(-1.0f, 2.0f, -3.0f, 4.0f);
	check_f32("f32_abs", simd128_f32_abs(a), 1.0f, 2.0f, 3.0f, 4.0f);
}

TEST_CASE("simd128_f32_min_max", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 5.0f, 3.0f, 8.0f);
	const simd128_t b = simd128_ld<simd128_t>(4.0f, 2.0f, 7.0f, 6.0f);
	check_f32("f32_min", simd128_f32_min(a, b), 1.0f, 2.0f, 3.0f, 6.0f);
	check_f32("f32_max", simd128_f32_max(a, b), 4.0f, 5.0f, 7.0f, 8.0f);
}

TEST_CASE("simd128_f32_clamp", "[simd]")
{
	const simd128_t a   = simd128_ld<simd128_t>(-1.0f, 0.5f, 1.5f, 3.0f);
	const simd128_t lo  = simd128_splat<simd128_t>(0.0f);
	const simd128_t hi  = simd128_splat<simd128_t>(1.0f);
	check_f32("f32_clamp", simd128_f32_clamp(a, lo, hi), 0.0f, 0.5f, 1.0f, 1.0f);
}

TEST_CASE("simd128_f32_lerp", "[simd]")
{
	const simd128_t a = simd128_splat<simd128_t>(0.0f);
	const simd128_t b = simd128_splat<simd128_t>(10.0f);
	const simd128_t s = simd128_splat<simd128_t>(0.25f);
	check_f32("f32_lerp", simd128_f32_lerp(a, b, s), 2.5f, 2.5f, 2.5f, 2.5f);
}

TEST_CASE("simd128_f32_sqrt", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(4.0f, 9.0f, 16.0f, 25.0f);
	check_f32("f32_sqrt", simd128_f32_sqrt(a), 2.0f, 3.0f, 4.0f, 5.0f);
}

TEST_CASE("simd128_f32_rcp", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(2.0f, 4.0f, 5.0f, 10.0f);
	const simd128_t r = simd128_f32_rcp(a);
	simd128_cast c; c.simd = r;
	REQUIRE(c.f[0] == Catch::Approx(0.5f).margin(0.001f));
	REQUIRE(c.f[1] == Catch::Approx(0.25f).margin(0.001f));
	REQUIRE(c.f[2] == Catch::Approx(0.2f).margin(0.001f));
	REQUIRE(c.f[3] == Catch::Approx(0.1f).margin(0.001f));
}

TEST_CASE("simd128_f32_round", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.4f, 1.5f, -1.4f, -1.5f);
	const simd128_t r = simd128_f32_round(a);
	simd128_cast c; c.simd = r;
	REQUIRE(c.f[0] == Catch::Approx(1.0f));
	REQUIRE(c.f[1] == Catch::Approx(2.0f));
	REQUIRE(c.f[2] == Catch::Approx(-1.0f));
	REQUIRE(c.f[3] == Catch::Approx(-2.0f).margin(1.0f)); // banker's rounding may differ
}

TEST_CASE("simd128_f32_ceil", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.1f, -1.1f, 2.9f, -2.9f);
	check_f32("f32_ceil", simd128_f32_ceil(a), 2.0f, -1.0f, 3.0f, -2.0f);
}

TEST_CASE("simd128_f32_floor", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.9f, -1.1f, 2.1f, -2.9f);
	check_f32("f32_floor", simd128_f32_floor(a), 1.0f, -2.0f, 2.0f, -3.0f);
}

TEST_CASE("simd128_f32_cmpeq", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	const simd128_t b = simd128_ld<simd128_t>(1.0f, 9.0f, 3.0f, 9.0f);
	check_u32("f32_cmpeq", simd128_f32_cmpeq(a, b), 0xffffffff, 0, 0xffffffff, 0);
}

TEST_CASE("simd128_f32_cmplt", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 5.0f, 3.0f, 8.0f);
	const simd128_t b = simd128_ld<simd128_t>(2.0f, 2.0f, 3.0f, 9.0f);
	check_u32("f32_cmplt", simd128_f32_cmplt(a, b), 0xffffffff, 0, 0, 0xffffffff);
}

TEST_CASE("simd128_f32_cmpneq", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	const simd128_t b = simd128_ld<simd128_t>(1.0f, 9.0f, 3.0f, 9.0f);
	check_u32("f32_cmpneq", simd128_f32_cmpneq(a, b), 0, 0xffffffff, 0, 0xffffffff);
}

TEST_CASE("simd128_i32_itof", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1u, 2u, 3u, 4u);
	// Interpret as int bits, convert to float
	const simd128_t b = simd128_i32_itof(a);
	simd128_cast ai; ai.simd = a;
	simd128_cast bf; bf.simd = b;
	REQUIRE(bf.f[0] == Catch::Approx((float)ai.i[0]));
	REQUIRE(bf.f[1] == Catch::Approx((float)ai.i[1]));
	REQUIRE(bf.f[2] == Catch::Approx((float)ai.i[2]));
	REQUIRE(bf.f[3] == Catch::Approx((float)ai.i[3]));
}

TEST_CASE("simd128_i32_add", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1u, 2u, 3u, 4u);
	const simd128_t b = simd128_ld<simd128_t>(10u, 20u, 30u, 40u);
	check_i32("i32_add", simd128_i32_add(a, b), 11, 22, 33, 44);
}

TEST_CASE("simd128_i32_sub", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(10u, 20u, 30u, 40u);
	const simd128_t b = simd128_ld<simd128_t>(1u, 2u, 3u, 4u);
	check_i32("i32_sub", simd128_i32_sub(a, b), 9, 18, 27, 36);
}

TEST_CASE("simd128_i32_min_max", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>( 1u, 50u, 3u, 80u);
	const simd128_t b = simd128_ld<simd128_t>(40u,  2u, 7u, 60u);
	check_i32("i32_min", simd128_i32_min(a, b), 1, 2, 3, 60);
	check_i32("i32_max", simd128_i32_max(a, b), 40, 50, 7, 80);
}

TEST_CASE("simd128_i32_cmpeq", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1u, 2u, 3u, 4u);
	const simd128_t b = simd128_ld<simd128_t>(1u, 9u, 3u, 9u);
	check_u32("i32_cmpeq", simd128_i32_cmpeq(a, b), 0xffffffff, 0, 0xffffffff, 0);
}

TEST_CASE("simd128_i32_cmpgt", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(5u, 2u, 7u, 4u);
	const simd128_t b = simd128_ld<simd128_t>(1u, 9u, 3u, 4u);
	check_u32("i32_cmpgt", simd128_i32_cmpgt(a, b), 0xffffffff, 0, 0xffffffff, 0);
}

TEST_CASE("simd128_i16_add", "[simd]")
{
	// Pack [1, 2, 3, 4, 5, 6, 7, 8] as 8--i16 into two 32-bit lanes
	const simd128_t a = simd128_ld<simd128_t>(
		  uint32_t((1 & 0xffffu) | (2 << 16))
		, uint32_t((3 & 0xffff) | (4 << 16))
		, uint32_t((5 & 0xffff) | (6 << 16))
		, uint32_t((7 & 0xffff) | (8 << 16))
	);
	const simd128_t b = simd128_ld<simd128_t>(
		  uint32_t((10 & 0xffffu) | (20 << 16))
		, uint32_t((30 & 0xffff) | (40 << 16))
		, uint32_t((50 & 0xffff) | (60 << 16))
		, uint32_t((70 & 0xffff) | (80 << 16))
	);
	const simd128_t r = simd128_i16_add(a, b);
	simd128_cast c; c.simd = r;
	REQUIRE(c.i16[0] == 11);
	REQUIRE(c.i16[1] == 22);
	REQUIRE(c.i16[2] == 33);
	REQUIRE(c.i16[3] == 44);
	REQUIRE(c.i16[4] == 55);
	REQUIRE(c.i16[5] == 66);
	REQUIRE(c.i16[6] == 77);
	REQUIRE(c.i16[7] == 88);
}

TEST_CASE("simd128_i8_add", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(
		0x01020304u, 0x05060708u, 0x090a0b0cu, 0x0d0e0f10u
	);
	const simd128_t b = simd128_ld<simd128_t>(
		0x10101010u, 0x10101010u, 0x10101010u, 0x10101010u
	);
	const simd128_t r = simd128_i8_add(a, b);
	simd128_cast c; c.simd = r;
	REQUIRE(c.u8[0]  == 0x14);
	REQUIRE(c.u8[1]  == 0x13);
	REQUIRE(c.u8[2]  == 0x12);
	REQUIRE(c.u8[3]  == 0x11);
}

TEST_CASE("simd128_u8_satadd", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0xf0f0f0f0u, 0u, 0u, 0u);
	const simd128_t b = simd128_ld<simd128_t>(0x20202020u, 0u, 0u, 0u);
	const simd128_t r = simd128_u8_satadd(a, b);
	simd128_cast c; c.simd = r;
	// 0xf0 + 0x20 = 0x110  saturates to 0xff
	REQUIRE(c.u8[0] == 0xff);
	REQUIRE(c.u8[1] == 0xff);
	REQUIRE(c.u8[2] == 0xff);
	REQUIRE(c.u8[3] == 0xff);
}

TEST_CASE("simd128_u8_satsub", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0x10101010u, 0u, 0u, 0u);
	const simd128_t b = simd128_ld<simd128_t>(0x20202020u, 0u, 0u, 0u);
	const simd128_t r = simd128_u8_satsub(a, b);
	simd128_cast c; c.simd = r;
	// 0x10 - 0x20  saturates to 0x00
	REQUIRE(c.u8[0] == 0x00);
	REQUIRE(c.u8[1] == 0x00);
}

TEST_CASE("simd128_and", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0xff00ff00u, 0xff00ff00u, 0xff00ff00u, 0xff00ff00u);
	const simd128_t b = simd128_ld<simd128_t>(0xffff0000u, 0xffff0000u, 0xffff0000u, 0xffff0000u);
	check_u32("and", simd128_and(a, b), 0xff000000, 0xff000000, 0xff000000, 0xff000000);
}

TEST_CASE("simd128_or", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0xf0f0f0f0u, 0u, 0u, 0u);
	const simd128_t b = simd128_ld<simd128_t>(0x0f0f0f0fu, 0u, 0u, 0u);
	check_u32("or", simd128_or(a, b), 0xffffffff, 0, 0, 0);
}

TEST_CASE("simd128_xor", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0xffffffffu, 0u, 0xffffffffu, 0u);
	const simd128_t b = simd128_ld<simd128_t>(0xffffffffu, 0xffffffffu, 0u, 0u);
	check_u32("xor", simd128_xor(a, b), 0, 0xffffffff, 0xffffffff, 0);
}

TEST_CASE("simd128_not", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0u, 0xffffffffu, 0x0f0f0f0fu, 0xf0f0f0f0u);
	check_u32("not", simd128_not(a), 0xffffffff, 0, 0xf0f0f0f0, 0x0f0f0f0f);
}

TEST_CASE("simd128_andc", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0xffffffffu, 0xffffffffu, 0u, 0u);
	const simd128_t b = simd128_ld<simd128_t>(0x0f0f0f0fu, 0u, 0x0f0f0f0fu, 0u);
	// a & ~b
	check_u32("andc", simd128_andc(a, b), 0xf0f0f0f0, 0xffffffff, 0, 0);
}

TEST_CASE("simd128_x32_sll", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1u, 2u, 4u, 8u);
	check_u32("sll", simd128_x32_sll(a, 2), 4, 8, 16, 32);
}

TEST_CASE("simd128_x32_srl", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(8u, 16u, 32u, 64u);
	check_u32("srl", simd128_x32_srl(a, 2), 2, 4, 8, 16);
}

TEST_CASE("simd128_x32_sra", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(uint32_t(-8), uint32_t(-16), 32u, 64u);
	check_i32("sra", simd128_x32_sra(a, 2), -2, -4, 8, 16);
}

TEST_CASE("simd128_selb", "[simd]")
{
	const simd128_t a    = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	const simd128_t b    = simd128_ld<simd128_t>(5.0f, 6.0f, 7.0f, 8.0f);
	const simd128_t mask = simd128_ld<simd128_t>(0xffffffffu, 0u, 0xffffffffu, 0u);
	// mask ? a : b
	check_f32("selb", simd128_selb(mask, a, b), 1.0f, 6.0f, 3.0f, 8.0f);
}

TEST_CASE("simd128_swizzle", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);

	check_f32("xyzw", a, 1.0f, 2.0f, 3.0f, 4.0f);
	check_f32("xxxx", simd128_x32_swiz_xxxx(a), 1.0f, 1.0f, 1.0f, 1.0f);
	check_f32("yyyy", simd128_x32_swiz_yyyy(a), 2.0f, 2.0f, 2.0f, 2.0f);
	check_f32("zzzz", simd128_x32_swiz_zzzz(a), 3.0f, 3.0f, 3.0f, 3.0f);
	check_f32("wwww", simd128_x32_swiz_wwww(a), 4.0f, 4.0f, 4.0f, 4.0f);
	check_f32("yzxw", simd128_x32_swiz_yzxw(a), 2.0f, 3.0f, 1.0f, 4.0f);
	check_f32("zxyw", simd128_x32_swiz_zxyw(a), 3.0f, 1.0f, 2.0f, 4.0f);
	check_f32("zwxy", simd128_x32_swiz_zwxy(a), 3.0f, 4.0f, 1.0f, 2.0f);
}

TEST_CASE("simd128_shuffle", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	const simd128_t b = simd128_ld<simd128_t>(5.0f, 6.0f, 7.0f, 8.0f);

	check_f32("xyAB", simd128_x32_shuf_xyAB(a, b), 1.0f, 2.0f, 5.0f, 6.0f);
	check_f32("ABxy", simd128_x32_shuf_ABxy(a, b), 5.0f, 6.0f, 1.0f, 2.0f);
	check_f32("CDzw", simd128_x32_shuf_CDzw(a, b), 7.0f, 8.0f, 3.0f, 4.0f);
	check_f32("zwCD", simd128_x32_shuf_zwCD(a, b), 3.0f, 4.0f, 7.0f, 8.0f);
	check_f32("xAyB", simd128_x32_shuf_xAyB(a, b), 1.0f, 5.0f, 2.0f, 6.0f);
	check_f32("AxBy", simd128_x32_shuf_AxBy(a, b), 5.0f, 1.0f, 6.0f, 2.0f);
	check_f32("zCwD", simd128_x32_shuf_zCwD(a, b), 3.0f, 7.0f, 4.0f, 8.0f);
	check_f32("CzDw", simd128_x32_shuf_CzDw(a, b), 7.0f, 3.0f, 8.0f, 4.0f);
	check_f32("xAzC", simd128_x32_shuf_xAzC(a, b), 1.0f, 5.0f, 3.0f, 7.0f);
	check_f32("yBwD", simd128_x32_shuf_yBwD(a, b), 2.0f, 6.0f, 4.0f, 8.0f);
	check_f32("xzAC", simd128_x32_shuf_xzAC(a, b), 1.0f, 3.0f, 5.0f, 7.0f);
	check_f32("ywBD", simd128_x32_shuf_ywBD(a, b), 2.0f, 4.0f, 6.0f, 8.0f);
	check_f32("xxAA", simd128_x32_shuf_xxAA(a, b), 1.0f, 1.0f, 5.0f, 5.0f);
	check_f32("yyBB", simd128_x32_shuf_yyBB(a, b), 2.0f, 2.0f, 6.0f, 6.0f);
	check_f32("zzCC", simd128_x32_shuf_zzCC(a, b), 3.0f, 3.0f, 7.0f, 7.0f);
	check_f32("wwDD", simd128_x32_shuf_wwDD(a, b), 4.0f, 4.0f, 8.0f, 8.0f);
}

TEST_CASE("simd128_test_any_all", "[simd]")
{
	const simd128_t all_set  = simd128_ld<simd128_t>(0x80000000u, 0x80000000u, 0x80000000u, 0x80000000u);
	const simd128_t none_set = simd128_zero<simd128_t>();
	const simd128_t x_set    = simd128_ld<simd128_t>(0x80000000u, 0u, 0u, 0u);
	const simd128_t y_set    = simd128_ld<simd128_t>(0u, 0x80000000u, 0u, 0u);
	const simd128_t z_set    = simd128_ld<simd128_t>(0u, 0u, 0x80000000u, 0u);
	const simd128_t w_set    = simd128_ld<simd128_t>(0u, 0u, 0u, 0x80000000u);

	REQUIRE( simd128_test_any_xyzw(all_set));
	REQUIRE( simd128_test_all_xyzw(all_set));
	REQUIRE(!simd128_test_any_xyzw(none_set));
	REQUIRE(!simd128_test_all_xyzw(none_set));
	REQUIRE( simd128_test_any_xyzw(x_set));
	REQUIRE( simd128_test_any_xyzw(y_set));
	REQUIRE( simd128_test_any_xyzw(z_set));
	REQUIRE( simd128_test_any_xyzw(w_set));
	REQUIRE(!simd128_test_all_xyzw(x_set));
	REQUIRE(!simd128_test_all_xyzw(y_set));
	REQUIRE(!simd128_test_all_xyzw(z_set));
	REQUIRE(!simd128_test_all_xyzw(w_set));
	REQUIRE( simd128_test_any_x(x_set));
	REQUIRE(!simd128_test_any_y(x_set));
	REQUIRE( simd128_test_all_x(x_set));
	REQUIRE(!simd128_test_all_xy(x_set));
}

TEST_CASE("simd128_f32_dot3", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 0.0f);
	const simd128_t b = simd128_ld<simd128_t>(4.0f, 5.0f, 6.0f, 0.0f);
	// 1*4 + 2*5 + 3*6 = 32
	const simd128_t d = simd128_f32_dot3(a, b);
	REQUIRE(simd128_f32_x(d) == Catch::Approx(32.0f) );
	REQUIRE(simd128_f32_y(d) == Catch::Approx(32.0f) );
	REQUIRE(simd128_f32_z(d) == Catch::Approx(32.0f) );
}

TEST_CASE("simd128_f32_dot", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	const simd128_t b = simd128_ld<simd128_t>(5.0f, 6.0f, 7.0f, 8.0f);
	// 1*5 + 2*6 + 3*7 + 4*8 = 70
	const simd128_t d = simd128_f32_dot(a, b);
	REQUIRE(simd128_f32_x(d) == Catch::Approx(70.0f) );
	REQUIRE(simd128_f32_y(d) == Catch::Approx(70.0f) );
	REQUIRE(simd128_f32_z(d) == Catch::Approx(70.0f) );
	REQUIRE(simd128_f32_w(d) == Catch::Approx(70.0f) );
}

TEST_CASE("simd128_f32_cross3", "[simd]")
{
	// x cross y = z
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 0.0f, 0.0f, 0.0f);
	const simd128_t b = simd128_ld<simd128_t>(0.0f, 1.0f, 0.0f, 0.0f);
	const simd128_t c = simd128_f32_cross3(a, b);
	REQUIRE(simd128_f32_x(c) == Catch::Approx(0.0f));
	REQUIRE(simd128_f32_y(c) == Catch::Approx(0.0f));
	REQUIRE(simd128_f32_z(c) == Catch::Approx(1.0f));
}

TEST_CASE("simd128_f32_normalize3", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(3.0f, 0.0f, 4.0f, 0.0f);
	const simd128_t n = simd128_f32_normalize3(a);
	// length = 5, so normalized = (0.6, 0, 0.8)
	REQUIRE(simd128_f32_x(n) == Catch::Approx(0.6f).margin(0.001f));
	REQUIRE(simd128_f32_y(n) == Catch::Approx(0.0f).margin(0.001f));
	REQUIRE(simd128_f32_z(n) == Catch::Approx(0.8f).margin(0.001f));
}

TEST_CASE("simd_f32_add_generic", "[simd]")
{
	// Tests that the width-generic wrapper dispatches correctly.
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	const simd128_t b = simd128_ld<simd128_t>(5.0f, 6.0f, 7.0f, 8.0f);
	const simd128_t r = simd_f32_add(a, b);
	check_f32("generic_f32_add", r, 6.0f, 8.0f, 10.0f, 12.0f);
}

TEST_CASE("simd_i32_add_generic", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1u, 2u, 3u, 4u);
	const simd128_t b = simd128_ld<simd128_t>(10u, 20u, 30u, 40u);
	const simd128_t r = simd_i32_add(a, b);
	check_i32("generic_i32_add", r, 11, 22, 33, 44);
}

TEST_CASE("simd_and_generic", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0xff00ff00u, 0xff00ff00u, 0xff00ff00u, 0xff00ff00u);
	const simd128_t b = simd128_ld<simd128_t>(0xffff0000u, 0xffff0000u, 0xffff0000u, 0xffff0000u);
	check_u32("generic_and", simd_and(a, b), 0xff000000, 0xff000000, 0xff000000, 0xff000000);
}

TEST_CASE("simd128_f32_log2", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(1.0f, 2.0f, 4.0f, 8.0f);
	const simd128_t r = simd_f32_log2(a);
	simd128_cast c; c.simd = r;
	REQUIRE(c.f[0] == Catch::Approx(0.0f).margin(0.01f));
	REQUIRE(c.f[1] == Catch::Approx(1.0f).margin(0.01f));
	REQUIRE(c.f[2] == Catch::Approx(2.0f).margin(0.01f));
	REQUIRE(c.f[3] == Catch::Approx(3.0f).margin(0.01f));
}

TEST_CASE("simd128_f32_exp2", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0.0f, 1.0f, 2.0f, 3.0f);
	const simd128_t r = simd_f32_exp2(a);
	simd128_cast c; c.simd = r;
	REQUIRE(c.f[0] == Catch::Approx(1.0f).margin(0.01f));
	REQUIRE(c.f[1] == Catch::Approx(2.0f).margin(0.01f));
	REQUIRE(c.f[2] == Catch::Approx(4.0f).margin(0.01f));
	REQUIRE(c.f[3] == Catch::Approx(8.0f).margin(0.01f));
}

TEST_CASE("simd128_f32_pow", "[simd]")
{
	const simd128_t a = simd128_splat<simd128_t>(2.0f);
	const simd128_t b = simd128_ld<simd128_t>(1.0f, 2.0f, 3.0f, 4.0f);
	const simd128_t r = simd_f32_pow(a, b);
	simd128_cast c; c.simd = r;
	REQUIRE(c.f[0] == Catch::Approx(2.0f).margin(0.01f));
	REQUIRE(c.f[1] == Catch::Approx(4.0f).margin(0.01f));
	REQUIRE(c.f[2] == Catch::Approx(8.0f).margin(0.01f));
	REQUIRE(c.f[3] == Catch::Approx(16.0f).margin(0.01f));
}

static simd32_t make_f32(float _v)
{
	simd32_t r{ .u32 = bitCast<uint32_t>(_v) }; return r;
}

static simd32_t make_u32(uint32_t _v)
{
	simd32_t r{ .u32 = _v }; return r;
}

static simd32_t make_i32(int32_t _v)
{
	simd32_t r{ .u32 = bitCast<uint32_t>(_v) }; return r;
}

TEST_CASE("simd32_f32_arithmetic", "[simd]")
{
	const simd32_t a = make_f32(10.0f);
	const simd32_t b = make_f32(3.0f);

	REQUIRE(bitCast<float>(simd32_f32_add(a, b).u32) == Catch::Approx(13.0f));
	REQUIRE(bitCast<float>(simd32_f32_sub(a, b).u32) == Catch::Approx(7.0f));
	REQUIRE(bitCast<float>(simd32_f32_mul(a, b).u32) == Catch::Approx(30.0f));
	REQUIRE(bitCast<float>(simd32_f32_div(a, b).u32) == Catch::Approx(10.0f / 3.0f));

	const simd32_t c = make_f32(1.0f);
	REQUIRE(bitCast<float>(simd32_f32_madd(a, b, c).u32) == Catch::Approx(31.0f));
	REQUIRE(bitCast<float>(simd32_f32_nmsub(a, b, c).u32) == Catch::Approx(-29.0f));

	REQUIRE(bitCast<float>(simd32_f32_neg(a).u32) == Catch::Approx(-10.0f));
	REQUIRE(bitCast<float>(simd32_f32_neg(make_f32(-5.0f)).u32) == Catch::Approx(5.0f));
	REQUIRE(bitCast<float>(simd32_f32_abs(make_f32(-7.0f)).u32) == Catch::Approx(7.0f));
	REQUIRE(bitCast<float>(simd32_f32_abs(make_f32(7.0f)).u32) == Catch::Approx(7.0f));
}

TEST_CASE("simd32_f32_min_max_clamp_lerp", "[simd]")
{
	REQUIRE(bitCast<float>(simd32_f32_min(make_f32(3.0f), make_f32(5.0f)).u32) == Catch::Approx(3.0f));
	REQUIRE(bitCast<float>(simd32_f32_max(make_f32(3.0f), make_f32(5.0f)).u32) == Catch::Approx(5.0f));
	REQUIRE(bitCast<float>(simd32_f32_clamp(make_f32(-1.0f), make_f32(0.0f), make_f32(1.0f)).u32) == Catch::Approx(0.0f));
	REQUIRE(bitCast<float>(simd32_f32_clamp(make_f32(0.5f), make_f32(0.0f), make_f32(1.0f)).u32) == Catch::Approx(0.5f));
	REQUIRE(bitCast<float>(simd32_f32_clamp(make_f32(2.0f), make_f32(0.0f), make_f32(1.0f)).u32) == Catch::Approx(1.0f));
	REQUIRE(bitCast<float>(simd32_f32_lerp(make_f32(0.0f), make_f32(10.0f), make_f32(0.25f)).u32) == Catch::Approx(2.5f));
}

TEST_CASE("simd32_f32_rcp_sqrt_rsqrt", "[simd]")
{
	REQUIRE(bitCast<float>(simd32_f32_rcp(make_f32(4.0f)).u32) == Catch::Approx(0.25f));
	REQUIRE(bitCast<float>(simd32_f32_sqrt(make_f32(25.0f)).u32) == Catch::Approx(5.0f));
	REQUIRE(bitCast<float>(simd32_f32_rsqrt(make_f32(4.0f)).u32) == Catch::Approx(0.5f));
}

TEST_CASE("simd32_f32_rounding", "[simd]")
{
	REQUIRE(bitCast<float>(simd32_f32_round(make_f32(1.4f)).u32) == Catch::Approx(1.0f));
	REQUIRE(bitCast<float>(simd32_f32_round(make_f32(1.6f)).u32) == Catch::Approx(2.0f));
	REQUIRE(bitCast<float>(simd32_f32_ceil(make_f32(1.1f)).u32) == Catch::Approx(2.0f));
	REQUIRE(bitCast<float>(simd32_f32_ceil(make_f32(-1.1f)).u32) == Catch::Approx(-1.0f));
	REQUIRE(bitCast<float>(simd32_f32_floor(make_f32(1.9f)).u32) == Catch::Approx(1.0f));
	REQUIRE(bitCast<float>(simd32_f32_floor(make_f32(-1.1f)).u32) == Catch::Approx(-2.0f));
}

TEST_CASE("simd32_f32_comparison", "[simd]")
{
	const simd32_t a = make_f32(5.0f);
	const simd32_t b = make_f32(3.0f);
	const simd32_t c = make_f32(5.0f);

	REQUIRE(simd32_f32_cmpeq(a, c).u32 == 0xffffffff);
	REQUIRE(simd32_f32_cmpeq(a, b).u32 == 0);
	REQUIRE(simd32_f32_cmpneq(a, b).u32 == 0xffffffff);
	REQUIRE(simd32_f32_cmplt(b, a).u32 == 0xffffffff);
	REQUIRE(simd32_f32_cmplt(a, b).u32 == 0);
	REQUIRE(simd32_f32_cmple(b, a).u32 == 0xffffffff);
	REQUIRE(simd32_f32_cmple(a, c).u32 == 0xffffffff);
	REQUIRE(simd32_f32_cmpgt(a, b).u32 == 0xffffffff);
	REQUIRE(simd32_f32_cmpge(a, c).u32 == 0xffffffff);
}

TEST_CASE("simd32_u32_arithmetic", "[simd]")
{
	const simd32_t a = make_u32(10);
	const simd32_t b = make_u32(3);

	REQUIRE(simd32_u32_add(a, b).u32 == 13);
	REQUIRE(simd32_u32_sub(a, b).u32 == 7);
	REQUIRE(simd32_u32_mul(a, b).u32 == 30);
	REQUIRE(simd32_u32_div(a, b).u32 == 3);
	REQUIRE(simd32_u32_mod(a, b).u32 == 1);
	REQUIRE(simd32_u32_min(a, b).u32 == 3);
	REQUIRE(simd32_u32_max(a, b).u32 == 10);
	REQUIRE(simd32_u32_clamp(make_u32(0), make_u32(2), make_u32(8)).u32 == 2);
	REQUIRE(simd32_u32_clamp(make_u32(5), make_u32(2), make_u32(8)).u32 == 5);
	REQUIRE(simd32_u32_clamp(make_u32(20), make_u32(2), make_u32(8)).u32 == 8);
}

TEST_CASE("simd32_i32_arithmetic", "[simd]")
{
	const simd32_t a = make_i32(10);
	const simd32_t b = make_i32(-3);

	REQUIRE(bitCast<int32_t>(simd32_i32_add(a, b).u32) == 7);
	REQUIRE(bitCast<int32_t>(simd32_i32_sub(a, b).u32) == 13);
	REQUIRE(bitCast<int32_t>(simd32_i32_min(a, b).u32) == -3);
	REQUIRE(bitCast<int32_t>(simd32_i32_max(a, b).u32) == 10);
	REQUIRE(bitCast<int32_t>(simd32_i32_clamp(make_i32(-5), make_i32(0), make_i32(10)).u32) == 0);
	REQUIRE(bitCast<int32_t>(simd32_i32_clamp(make_i32(5), make_i32(0), make_i32(10)).u32) == 5);
	REQUIRE(bitCast<int32_t>(simd32_i32_clamp(make_i32(15), make_i32(0), make_i32(10)).u32) == 10);
}

TEST_CASE("simd32_u32_comparison", "[simd]")
{
	REQUIRE(simd32_u32_cmpeq(make_u32(5), make_u32(5)).u32 == 0xffffffff);
	REQUIRE(simd32_u32_cmpeq(make_u32(5), make_u32(3)).u32 == 0);
	REQUIRE(simd32_u32_cmpneq(make_u32(5), make_u32(3)).u32 == 0xffffffff);
	REQUIRE(simd32_u32_cmplt(make_u32(3), make_u32(5)).u32 == 0xffffffff);
	REQUIRE(simd32_u32_cmplt(make_u32(5), make_u32(3)).u32 == 0);
	REQUIRE(simd32_u32_cmple(make_u32(5), make_u32(5)).u32 == 0xffffffff);
	REQUIRE(simd32_u32_cmpgt(make_u32(5), make_u32(3)).u32 == 0xffffffff);
	REQUIRE(simd32_u32_cmpge(make_u32(5), make_u32(5)).u32 == 0xffffffff);
}

TEST_CASE("simd32_i32_comparison", "[simd]")
{
	REQUIRE(simd32_i32_cmpeq(make_i32(5), make_i32(5)).u32 == 0xffffffff);
	REQUIRE(simd32_i32_cmpeq(make_i32(5), make_i32(-3)).u32 == 0);
	REQUIRE(simd32_i32_cmplt(make_i32(-3), make_i32(5)).u32 == 0xffffffff);
	REQUIRE(simd32_i32_cmpgt(make_i32(5), make_i32(-3)).u32 == 0xffffffff);
}

TEST_CASE("simd32_bitwise", "[simd]")
{
	REQUIRE(simd32_and(make_u32(0xff00), make_u32(0x0ff0)).u32 == 0x0f00);
	REQUIRE(simd32_andc(make_u32(0xffff), make_u32(0x0f0f)).u32 == 0xf0f0);
	REQUIRE(simd32_or(make_u32(0xf000), make_u32(0x000f)).u32 == 0xf00f);
	REQUIRE(simd32_orc(make_u32(0xf0f0), make_u32(0x0f0f)).u32 == 0xfffff0f0);
	REQUIRE(simd32_xor(make_u32(0xffff), make_u32(0x0f0f)).u32 == 0xf0f0);
	REQUIRE(simd32_not(make_u32(0)).u32 == 0xffffffff);
}

TEST_CASE("simd32_shifts", "[simd]")
{
	REQUIRE(simd32_x32_sll(make_u32(1), 4).u32 == 16);
	REQUIRE(simd32_x32_srl(make_u32(32), 3).u32 == 4);
	REQUIRE(bitCast<int32_t>(simd32_x32_sra(make_i32(-8), 2).u32) == -2);
}

TEST_CASE("simd32_selection", "[simd]")
{
	const simd32_t a = make_f32(1.0f);
	const simd32_t b = make_f32(2.0f);
	REQUIRE(bitCast<float>(simd32_selb(make_u32(0xffffffff), a, b).u32) == Catch::Approx(1.0f));
	REQUIRE(bitCast<float>(simd32_selb(make_u32(0), a, b).u32) == Catch::Approx(2.0f));
	REQUIRE(bitCast<float>(simd32_sels(make_i32(-1), a, b).u32) == Catch::Approx(1.0f));
	REQUIRE(bitCast<float>(simd32_sels(make_i32(0), a, b).u32) == Catch::Approx(2.0f));
}

TEST_CASE("simd256_f32_add", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
	BX_ALIGN_DECL(32, float bd[8]) = { 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);
	const simd256_t r = simd256_f32_add(a, b);
	BX_ALIGN_DECL(32, float out[8]);
	simd256_st(out, r);
	REQUIRE(out[0] == Catch::Approx(11.0f));
	REQUIRE(out[1] == Catch::Approx(22.0f));
	REQUIRE(out[2] == Catch::Approx(33.0f));
	REQUIRE(out[3] == Catch::Approx(44.0f));
	REQUIRE(out[4] == Catch::Approx(55.0f));
	REQUIRE(out[5] == Catch::Approx(66.0f));
	REQUIRE(out[6] == Catch::Approx(77.0f));
	REQUIRE(out[7] == Catch::Approx(88.0f));
}

TEST_CASE("simd256_f32_mul", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
	BX_ALIGN_DECL(32, float bd[8]) = { 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);
	const simd256_t r = simd256_f32_mul(a, b);
	BX_ALIGN_DECL(32, float out[8]);
	simd256_st(out, r);
	REQUIRE(out[0] == Catch::Approx(6.0f));
	REQUIRE(out[1] == Catch::Approx(12.0f));
	REQUIRE(out[2] == Catch::Approx(20.0f));
	REQUIRE(out[3] == Catch::Approx(30.0f));
	REQUIRE(out[4] == Catch::Approx(42.0f));
	REQUIRE(out[5] == Catch::Approx(56.0f));
	REQUIRE(out[6] == Catch::Approx(72.0f));
	REQUIRE(out[7] == Catch::Approx(90.0f));
}

TEST_CASE("simd256_f32_min_max", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { 1.0f, 5.0f, 3.0f, 8.0f, 2.0f, 7.0f, 4.0f, 9.0f };
	BX_ALIGN_DECL(32, float bd[8]) = { 4.0f, 2.0f, 7.0f, 6.0f, 8.0f, 1.0f, 5.0f, 3.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, float mn[8]);
	simd256_st(mn, simd256_f32_min(a, b));
	REQUIRE(mn[0] == Catch::Approx(1.0f));
	REQUIRE(mn[1] == Catch::Approx(2.0f));
	REQUIRE(mn[2] == Catch::Approx(3.0f));
	REQUIRE(mn[3] == Catch::Approx(6.0f));
	REQUIRE(mn[4] == Catch::Approx(2.0f));
	REQUIRE(mn[5] == Catch::Approx(1.0f));
	REQUIRE(mn[6] == Catch::Approx(4.0f));
	REQUIRE(mn[7] == Catch::Approx(3.0f));

	BX_ALIGN_DECL(32, float mx[8]);
	simd256_st(mx, simd256_f32_max(a, b));
	REQUIRE(mx[0] == Catch::Approx(4.0f));
	REQUIRE(mx[1] == Catch::Approx(5.0f));
	REQUIRE(mx[2] == Catch::Approx(7.0f));
	REQUIRE(mx[3] == Catch::Approx(8.0f));
	REQUIRE(mx[4] == Catch::Approx(8.0f));
	REQUIRE(mx[5] == Catch::Approx(7.0f));
	REQUIRE(mx[6] == Catch::Approx(5.0f));
	REQUIRE(mx[7] == Catch::Approx(9.0f));
}

TEST_CASE("simd256_bitwise", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t ad[8]) = { 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0, 0, 0, 0 };
	BX_ALIGN_DECL(32, uint32_t bd[8]) = { 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);
	const simd256_t r = simd256_and(a, b);
	BX_ALIGN_DECL(32, uint32_t out[8]);
	simd256_st(out, r);
	REQUIRE(out[0] == 0xff000000);
	REQUIRE(out[1] == 0xff000000);
	REQUIRE(out[4] == 0);
}

TEST_CASE("simd256_i32_add", "[simd]")
{
	BX_ALIGN_DECL(32, int32_t ad[8]) = { 1, 2, 3, 4, 5, 6, 7, 8 };
	BX_ALIGN_DECL(32, int32_t bd[8]) = { 10, 20, 30, 40, 50, 60, 70, 80 };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);
	const simd256_t r = simd256_i32_add(a, b);
	BX_ALIGN_DECL(32, int32_t out[8]);
	simd256_st(out, r);
	REQUIRE(out[0] == 11);
	REQUIRE(out[1] == 22);
	REQUIRE(out[2] == 33);
	REQUIRE(out[3] == 44);
	REQUIRE(out[4] == 55);
	REQUIRE(out[5] == 66);
	REQUIRE(out[6] == 77);
	REQUIRE(out[7] == 88);
}

// --- simd32 missing coverage ---

TEST_CASE("simd32_ld_st_zero_splat", "[simd]")
{
	const simd32_t z = simd32_zero();
	REQUIRE(z.u32 == 0);

	const simd32_t sf = simd32_splat(42.0f);
	REQUIRE(bitCast<float>(sf.u32) == Catch::Approx(42.0f));

	const simd32_t su = simd32_splat(0xdeadbeefu);
	REQUIRE(su.u32 == 0xdeadbeef);

	const simd32_t si = simd32_splat(int32_t(-7));
	REQUIRE(bitCast<int32_t>(si.u32) == -7);

	const simd32_t su16 = simd32_splat(uint16_t(0xabcd));
	REQUIRE(su16.u32 == 0xabcdabcd);

	const simd32_t su8 = simd32_splat(uint8_t(0x42));
	REQUIRE(su8.u32 == 0x42424242);

	float fval = 3.14f;
	const simd32_t lf = simd32_ld(&fval);
	REQUIRE(bitCast<float>(lf.u32) == Catch::Approx(3.14f));

	float fout;
	simd32_st(&fout, lf);
	REQUIRE(fout == Catch::Approx(3.14f));

	float fout1;
	simd32_x32_st1(&fout1, lf);
	REQUIRE(fout1 == Catch::Approx(3.14f));
}

TEST_CASE("simd32_f32_msub", "[simd]")
{
	const simd32_t a = make_f32(10.0f);
	const simd32_t b = make_f32(3.0f);
	const simd32_t c = make_f32(1.0f);
	// msub: a*b - c = 30 - 1 = 29
	REQUIRE(bitCast<float>(simd32_f32_msub(a, b, c).u32) == Catch::Approx(29.0f));
}

TEST_CASE("simd32_i32_neg_abs", "[simd]")
{
	REQUIRE(bitCast<int32_t>(simd32_i32_neg(make_i32(5)).u32) == -5);
	REQUIRE(bitCast<int32_t>(simd32_i32_neg(make_i32(-3)).u32) == 3);
	REQUIRE(bitCast<int32_t>(simd32_i32_abs(make_i32(-7)).u32) == 7);
	REQUIRE(bitCast<int32_t>(simd32_i32_abs(make_i32(7)).u32) == 7);
}

TEST_CASE("simd32_i16_add_sub", "[simd]")
{
	// 0x00030005: hi16=3, lo16=5; 0x00010002: hi16=1, lo16=2
	const simd32_t a = make_u32(0x00030005);
	const simd32_t b = make_u32(0x00010002);
	// add: hi16=4, lo16=7 -> 0x00040007
	REQUIRE(simd32_i16_add(a, b).u32 == 0x00040007);
	// sub: hi16=2, lo16=3 -> 0x00020003
	REQUIRE(simd32_i16_sub(a, b).u32 == 0x00020003);
}

TEST_CASE("simd32_i8_add_sub", "[simd]")
{
	const simd32_t a = make_u32(0x01020304);
	const simd32_t b = make_u32(0x10101010);
	REQUIRE(simd32_i8_add(a, b).u32 == 0x11121314);
	REQUIRE(simd32_i8_sub(a, b).u32 == 0xf1f2f3f4);
}

TEST_CASE("simd32_u8_sat", "[simd]")
{
	// satadd: 0xf0 + 0x20 = saturates to 0xff per byte
	REQUIRE(simd32_u8_satadd(make_u32(0xf0f0f0f0), make_u32(0x20202020)).u32 == 0xffffffff);
	// satadd: no saturation
	REQUIRE(simd32_u8_satadd(make_u32(0x01010101), make_u32(0x02020202)).u32 == 0x03030303);
	// satsub: 0x10 - 0x20 = saturates to 0x00
	REQUIRE(simd32_u8_satsub(make_u32(0x10101010), make_u32(0x20202020)).u32 == 0x00000000);
	// satsub: no saturation
	REQUIRE(simd32_u8_satsub(make_u32(0x30303030), make_u32(0x10101010)).u32 == 0x20202020);
}

TEST_CASE("simd32_u16_sat", "[simd]")
{
	// satadd: 0xfff0 + 0x0020 per 16-bit lane saturates to 0xffff
	REQUIRE(simd32_u16_satadd(make_u32(0xfff0fff0), make_u32(0x00200020)).u32 == 0xffffffff);
	// satsub: 0x0010 - 0x0020 saturates to 0
	REQUIRE(simd32_u16_satsub(make_u32(0x00100010), make_u32(0x00200020)).u32 == 0x00000000);
}

TEST_CASE("simd32_u32_satadd_satsub_satmul", "[simd]")
{
	REQUIRE(simd32_u32_satadd(make_u32(0xfffffff0), make_u32(0x20)).u32 == 0xffffffff);
	REQUIRE(simd32_u32_satadd(make_u32(10), make_u32(20)).u32 == 30);
	REQUIRE(simd32_u32_satsub(make_u32(5), make_u32(10)).u32 == 0);
	REQUIRE(simd32_u32_satsub(make_u32(10), make_u32(5)).u32 == 5);
	REQUIRE(simd32_u32_satmul(make_u32(0x80000000), make_u32(3)).u32 == 0xffffffff);
	REQUIRE(simd32_u32_satmul(make_u32(10), make_u32(3)).u32 == 30);
}

TEST_CASE("simd32_u32_misc", "[simd]")
{
	REQUIRE(simd32_u32_setnz(make_u32(0)).u32 == 0);
	REQUIRE(simd32_u32_setnz(make_u32(42)).u32 == 0xffffffff);
	REQUIRE(simd32_u32_xorl(make_u32(0xff00), make_u32(0x0ff0)).u32 == 0);
	REQUIRE(simd32_u32_xorl(make_u32(0xff00), make_u32(0xff00)).u32 == 0);
	REQUIRE(simd32_u32_xorl(make_u32(0xff00), make_u32(0)).u32 == 1);
	REQUIRE(simd32_u32_xorl(make_u32(0), make_u32(0)).u32 == 0);
	REQUIRE(simd32_u32_nextpow2(make_u32(5)).u32 == 8);
	REQUIRE(simd32_u32_nextpow2(make_u32(8)).u32 == 8);
	REQUIRE(simd32_u32_testpow2(make_u32(8)).u32 == 0xffffffff);
	REQUIRE(simd32_u32_testpow2(make_u32(7)).u32 == 0);
	REQUIRE(simd32_u32_incwrap(make_u32(3), make_u32(0), make_u32(5)).u32 == 4);
	REQUIRE(simd32_u32_incwrap(make_u32(5), make_u32(0), make_u32(5)).u32 == 0);
	REQUIRE(simd32_u32_decwrap(make_u32(1), make_u32(0), make_u32(5)).u32 == 0);
	REQUIRE(simd32_u32_decwrap(make_u32(0), make_u32(0), make_u32(5)).u32 == 5);
	REQUIRE(simd32_u32_gcd(make_u32(12), make_u32(8)).u32 == 4);
	REQUIRE(simd32_u32_lcm(make_u32(4), make_u32(6)).u32 == 12);
}

TEST_CASE("simd32_bit_ops", "[simd]")
{
	REQUIRE(simd32_x32_cntbits(make_u32(0xff)).u32 == 8);
	REQUIRE(simd32_x32_cntbits(make_u32(0)).u32 == 0);
	REQUIRE(simd32_x32_cntlz(make_u32(1)).u32 == 31);
	REQUIRE(simd32_x32_cntlz(make_u32(0x80000000)).u32 == 0);
	REQUIRE(simd32_x32_cnttz(make_u32(0x80000000)).u32 == 31);
	REQUIRE(simd32_x32_cnttz(make_u32(1)).u32 == 0);
	REQUIRE(simd32_x32_ffs(make_u32(0x80)).u32 == 8);
	REQUIRE(simd32_x32_rol(make_u32(1), 1).u32 == 2);
	REQUIRE(simd32_x32_ror(make_u32(2), 1).u32 == 1);
}

TEST_CASE("simd32_signbits_test", "[simd]")
{
	REQUIRE(simd32_x32_signbitsmask(make_u32(0x80000000)) == 1);
	REQUIRE(simd32_x32_signbitsmask(make_u32(0x7fffffff)) == 0);
	REQUIRE(simd32_x8_signbitsmask(make_u32(0x80808080)) == 0xf);
	REQUIRE(simd32_x8_signbitsmask(make_u32(0x00000000)) == 0);
	REQUIRE(simd32_test(make_u32(0xffffffff)) == true);
	REQUIRE(simd32_test(make_u32(0)) == false);
}

TEST_CASE("simd32_part1by", "[simd]")
{
	REQUIRE(simd32_x32_part1by1(make_u32(0x3)).u32 == 0x5);
	REQUIRE(simd32_x32_part1by2(make_u32(0x3)).u32 == 0x9);
}

// --- simd128 missing coverage ---

TEST_CASE("simd128_f32_msub", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(10.0f, 20.0f, 30.0f, 40.0f);
	const simd128_t b = simd128_ld<simd128_t>(2.0f, 3.0f, 4.0f, 5.0f);
	const simd128_t c = simd128_ld<simd128_t>(1.0f, 1.0f, 1.0f, 1.0f);
	// msub: a*b - c
	check_f32("f32_msub", simd128_f32_msub(a, b, c), 19.0f, 59.0f, 119.0f, 199.0f);
}

TEST_CASE("simd128_f32_cos_sin", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0.0f, kPiHalf, kPi, kPi * 1.5f);
	simd128_cast rc; rc.simd = simd_f32_cos(a);
	REQUIRE(rc.f[0] == Catch::Approx( 1.0f).margin(0.001f));
	REQUIRE(rc.f[1] == Catch::Approx( 0.0f).margin(0.001f));
	REQUIRE(rc.f[2] == Catch::Approx(-1.0f).margin(0.001f));
	REQUIRE(rc.f[3] == Catch::Approx( 0.0f).margin(0.001f));

	simd128_cast rs; rs.simd = simd_f32_sin(a);
	REQUIRE(rs.f[0] == Catch::Approx( 0.0f).margin(0.001f));
	REQUIRE(rs.f[1] == Catch::Approx( 1.0f).margin(0.001f));
	REQUIRE(rs.f[2] == Catch::Approx( 0.0f).margin(0.001f));
	REQUIRE(rs.f[3] == Catch::Approx(-1.0f).margin(0.001f));
}

TEST_CASE("simd128_f32_ftoi_trunc", "[simd]")
{
	check_i32("ftoi_trunc", simd128_f32_ftoi_trunc(simd128_ld<simd128_t>(1.0f, -2.0f, 3.5f, -4.9f)), 1, -2, 3, -4);
}

TEST_CASE("simd128_f32_rcp_rsqrt_variants", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(4.0f, 16.0f, 25.0f, 100.0f);
	simd128_cast cr; cr.simd = simd128_f32_rcp_est(a);
	REQUIRE(cr.f[0] == Catch::Approx(0.25f).margin(0.01f));

	simd128_cast crsqrt; crsqrt.simd = simd128_f32_rsqrt_est(a);
	REQUIRE(crsqrt.f[0] == Catch::Approx(0.5f).margin(0.01f));

	simd128_cast crsqrt2; crsqrt2.simd = simd128_f32_rsqrt(a);
	REQUIRE(crsqrt2.f[0] == Catch::Approx(0.5f).margin(0.01f));

	simd128_cast crsqrtnr; crsqrtnr.simd = simd128_f32_rsqrt_nr(a);
	REQUIRE(crsqrtnr.f[0] == Catch::Approx(0.5f).margin(0.001f));

	simd128_cast cdivnr; cdivnr.simd = simd128_f32_div_nr(simd128_splat<simd128_t>(1.0f), a);
	REQUIRE(cdivnr.f[0] == Catch::Approx(0.25f).margin(0.01f));
}

TEST_CASE("simd128_i32_neg_abs_clamp", "[simd]")
{
	check_i32("i32_neg", simd128_i32_neg(simd128_ld<simd128_t>(1, -2, 0, 5)), -1, 2, 0, -5);
	check_i32("i32_abs", simd128_i32_abs(simd128_ld<simd128_t>(-7, 3, -1, 0)), 7, 3, 1, 0);
	check_i32("i32_clamp", simd128_i32_clamp(
		simd128_ld<simd128_t>(-5, 5, 15, 0),
		simd128_ld<simd128_t>(0, 0, 0, 0),
		simd128_ld<simd128_t>(10, 10, 10, 10)
	), 0, 5, 10, 0);
}

TEST_CASE("simd128_i16_sub_mullo", "[simd]")
{
	// a = 0x00030005 00070009 000b000d 000f0011 -> i16: 3,5,7,9,11,13,15,17
	// b = 0x00010002 00010002 00010002 00010002 -> i16: 1,2,1,2,1,2,1,2
	const simd128_t a = simd128_ld<simd128_t>(0x00030005u, 0x00070009u, 0x000b000du, 0x000f0011u);
	const simd128_t b = simd128_ld<simd128_t>(0x00010002u, 0x00010002u, 0x00010002u, 0x00010002u);

	// sub: 3-1=2, 5-2=3, 7-1=6, 9-2=7, 11-1=10, 13-2=11, 15-1=14, 17-2=15
	check_u32("i16_sub", simd128_i16_sub(a, b), 0x00020003, 0x00060007, 0x000a000b, 0x000e000f);

	// mullo: 3*1=3, 5*2=10, 7*1=7, 9*2=18, 11*1=11, 13*2=26, 15*1=15, 17*2=34
	check_u32("i16_mullo", simd128_i16_mullo(a, b), 0x0003000a, 0x00070012, 0x000b001a, 0x000f0022);
}

TEST_CASE("simd128_i8_sub", "[simd]")
{
	const simd128_t a = simd128_ld<simd128_t>(0x11121314u, 0u, 0u, 0u);
	const simd128_t b = simd128_ld<simd128_t>(0x10101010u, 0u, 0u, 0u);
	const simd128_t r = simd128_i8_sub(a, b);
	simd128_cast c; c.simd = r;
	REQUIRE(c.u8[0] == 0x04);
	REQUIRE(c.u8[1] == 0x03);
	REQUIRE(c.u8[2] == 0x02);
	REQUIRE(c.u8[3] == 0x01);
}

TEST_CASE("simd128_u16_sat", "[simd]")
{
	// satadd: 0xfff0+0x0020 per lane saturates to 0xffff
	const simd128_t a = simd128_ld<simd128_t>(0xfff0fff0u, 0u, 0u, 0u);
	const simd128_t b = simd128_ld<simd128_t>(0x00200020u, 0u, 0u, 0u);
	simd128_cast cr; cr.simd = simd128_u16_satadd(a, b);
	REQUIRE(cr.u16[0] == 0xffff);
	REQUIRE(cr.u16[1] == 0xffff);

	// satsub: 0x0010 - 0x0020 saturates to 0
	const simd128_t c = simd128_ld<simd128_t>(0x00100010u, 0u, 0u, 0u);
	const simd128_t d = simd128_ld<simd128_t>(0x00200020u, 0u, 0u, 0u);
	simd128_cast cs; cs.simd = simd128_u16_satsub(c, d);
	REQUIRE(cs.u16[0] == 0x0000);
	REQUIRE(cs.u16[1] == 0x0000);
}

TEST_CASE("simd128_u32_ops", "[simd]")
{
	check_u32("u32_add", simd128_u32_add(simd128_ld<simd128_t>(10u, 20u, 30u, 40u), simd128_ld<simd128_t>(1u, 2u, 3u, 4u)), 11, 22, 33, 44);
	check_u32("u32_sub", simd128_u32_sub(simd128_ld<simd128_t>(10u, 20u, 30u, 40u), simd128_ld<simd128_t>(1u, 2u, 3u, 4u)), 9, 18, 27, 36);
	check_u32("u32_mul", simd128_u32_mul(simd128_ld<simd128_t>(2u, 3u, 4u, 5u), simd128_ld<simd128_t>(3u, 4u, 5u, 6u)), 6, 12, 20, 30);
	check_u32("u32_min", simd128_u32_min(simd128_ld<simd128_t>(1u, 5u, 3u, 8u), simd128_ld<simd128_t>(4u, 2u, 7u, 6u)), 1, 2, 3, 6);
	check_u32("u32_max", simd128_u32_max(simd128_ld<simd128_t>(1u, 5u, 3u, 8u), simd128_ld<simd128_t>(4u, 2u, 7u, 6u)), 4, 5, 7, 8);
	check_u32("u32_clamp", simd128_u32_clamp(simd128_ld<simd128_t>(0u, 5u, 15u, 10u), simd128_ld<simd128_t>(2u, 2u, 2u, 2u), simd128_ld<simd128_t>(10u, 10u, 10u, 10u)), 2, 5, 10, 10);
}

TEST_CASE("simd128_u32_cmp", "[simd]")
{
	check_u32("u32_cmpeq", simd128_u32_cmpeq(simd128_ld<simd128_t>(1u, 2u, 3u, 4u), simd128_ld<simd128_t>(1u, 0u, 3u, 0u)), 0xffffffff, 0, 0xffffffff, 0);
	check_u32("u32_cmplt", simd128_u32_cmplt(simd128_ld<simd128_t>(1u, 5u, 3u, 4u), simd128_ld<simd128_t>(2u, 2u, 3u, 5u)), 0xffffffff, 0, 0, 0xffffffff);
	check_u32("u32_cmpgt", simd128_u32_cmpgt(simd128_ld<simd128_t>(5u, 1u, 3u, 4u), simd128_ld<simd128_t>(2u, 2u, 3u, 5u)), 0xffffffff, 0, 0, 0);
}

TEST_CASE("simd128_orc_orx_sels", "[simd]")
{
	check_u32("orc", simd128_orc(simd128_ld<simd128_t>(0xf0f0f0f0u, 0u, 0u, 0u), simd128_ld<simd128_t>(0x0f0f0f0fu, 0u, 0u, 0u)), 0xf0f0f0f0, 0xffffffff, 0xffffffff, 0xffffffff);

	// orx: OR all 4 lanes together into lane 0 (implementation-specific, just test non-zero)
	const simd128_t ox = simd128_orx(simd128_ld<simd128_t>(0x01u, 0x02u, 0x04u, 0x08u));
	simd128_cast co; co.simd = ox;
	REQUIRE((co.u[0] & 0x0f) == 0x0f);

	// sels: select based on sign bit
	const simd128_t neg = simd128_ld<simd128_t>(0x80000000u, 0u, 0x80000000u, 0u);
	const simd128_t sa = simd128_ld<simd128_t>(1u, 1u, 1u, 1u);
	const simd128_t sb = simd128_ld<simd128_t>(2u, 2u, 2u, 2u);
	check_u32("sels", simd128_sels(neg, sa, sb), 1, 2, 1, 2);
}

TEST_CASE("simd128_signbits", "[simd]")
{
	REQUIRE(simd128_x32_signbitsmask(simd128_ld<simd128_t>(0x80000000u, 0u, 0x80000000u, 0u)) == 0x5);
	REQUIRE(simd128_x8_signbitsmask(simd128_zero()) == 0);
	REQUIRE(simd128_x8_signbitsmask(simd128_ld<simd128_t>(0x80808080u, 0x80808080u, 0x80808080u, 0x80808080u)) == 0xffff);
}

// --- simd256 missing coverage ---

TEST_CASE("simd256_zero_splat", "[simd]")
{
	BX_ALIGN_DECL(32, float out[8]);
	simd256_st(out, simd256_zero<simd256_t>());
	for (int ii = 0; ii < 8; ++ii) REQUIRE(out[ii] == 0.0f);

	simd256_st(out, simd256_splat<simd256_t>(7.0f));
	for (int ii = 0; ii < 8; ++ii) REQUIRE(out[ii] == 7.0f);

	BX_ALIGN_DECL(32, uint32_t uout[8]);
	simd256_st(uout, simd256_splat<simd256_t>(0xdeadbeefu));
	for (int ii = 0; ii < 8; ++ii) REQUIRE(uout[ii] == 0xdeadbeef);
}

TEST_CASE("simd256_f32_sub_div", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f };
	BX_ALIGN_DECL(32, float bd[8]) = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, float osub[8]);
	simd256_st(osub, simd256_f32_sub(a, b));
	REQUIRE(osub[0] == Catch::Approx(9.0f));
	REQUIRE(osub[7] == Catch::Approx(72.0f));

	BX_ALIGN_DECL(32, float odiv[8]);
	simd256_st(odiv, simd256_f32_div(a, b));
	REQUIRE(odiv[0] == Catch::Approx(10.0f));
	REQUIRE(odiv[7] == Catch::Approx(10.0f));
}

TEST_CASE("simd256_f32_madd_msub_nmsub", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };
	BX_ALIGN_DECL(32, float bd[8]) = { 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f };
	BX_ALIGN_DECL(32, float cd[8]) = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);
	const simd256_t c = simd256_ld<simd256_t>(cd);

	BX_ALIGN_DECL(32, float out[8]);
	// madd: a*b + c = 7
	simd256_st(out, simd256_f32_madd(a, b, c));
	REQUIRE(out[0] == Catch::Approx(7.0f));
	REQUIRE(out[7] == Catch::Approx(7.0f));

	// msub: a*b - c = 5
	simd256_st(out, simd256_f32_msub(a, b, c));
	REQUIRE(out[0] == Catch::Approx(5.0f));

	// nmsub: c - a*b = -5
	simd256_st(out, simd256_f32_nmsub(a, b, c));
	REQUIRE(out[0] == Catch::Approx(-5.0f));
}

TEST_CASE("simd256_f32_neg_abs", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { -1.0f, 2.0f, -3.0f, 4.0f, -5.0f, 6.0f, -7.0f, 8.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);

	BX_ALIGN_DECL(32, float on[8]);
	simd256_st(on, simd256_f32_neg(a));
	REQUIRE(on[0] == Catch::Approx(1.0f));
	REQUIRE(on[1] == Catch::Approx(-2.0f));

	BX_ALIGN_DECL(32, float oa[8]);
	simd256_st(oa, simd256_f32_abs(a));
	REQUIRE(oa[0] == Catch::Approx(1.0f));
	REQUIRE(oa[2] == Catch::Approx(3.0f));
}

TEST_CASE("simd256_f32_clamp_lerp", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { -1.0f, 0.5f, 2.0f, 0.5f, -1.0f, 0.5f, 2.0f, 0.5f };
	BX_ALIGN_DECL(32, float mn[8]) = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	BX_ALIGN_DECL(32, float mx[8]) = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);

	BX_ALIGN_DECL(32, float oc[8]);
	simd256_st(oc, simd256_f32_clamp(a, simd256_ld<simd256_t>(mn), simd256_ld<simd256_t>(mx)));
	REQUIRE(oc[0] == Catch::Approx(0.0f));
	REQUIRE(oc[1] == Catch::Approx(0.5f));
	REQUIRE(oc[2] == Catch::Approx(1.0f));

	BX_ALIGN_DECL(32, float aa[8]) = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	BX_ALIGN_DECL(32, float bb[8]) = { 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f };
	BX_ALIGN_DECL(32, float ss[8]) = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
	BX_ALIGN_DECL(32, float ol[8]);
	simd256_st(ol, simd256_f32_lerp(simd256_ld<simd256_t>(aa), simd256_ld<simd256_t>(bb), simd256_ld<simd256_t>(ss)));
	REQUIRE(ol[0] == Catch::Approx(5.0f));
	REQUIRE(ol[7] == Catch::Approx(5.0f));
}

TEST_CASE("simd256_f32_rcp_sqrt_rsqrt", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { 4.0f, 16.0f, 25.0f, 100.0f, 4.0f, 16.0f, 25.0f, 100.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);

	BX_ALIGN_DECL(32, float orcp[8]);
	simd256_st(orcp, simd256_f32_rcp(a));
	REQUIRE(orcp[0] == Catch::Approx(0.25f).margin(0.01f));

	BX_ALIGN_DECL(32, float osqrt[8]);
	simd256_st(osqrt, simd256_f32_sqrt(a));
	REQUIRE(osqrt[0] == Catch::Approx(2.0f).margin(0.001f));
	REQUIRE(osqrt[1] == Catch::Approx(4.0f).margin(0.001f));

	BX_ALIGN_DECL(32, float orsqrt[8]);
	simd256_st(orsqrt, simd256_f32_rsqrt(a));
	REQUIRE(orsqrt[0] == Catch::Approx(0.5f).margin(0.01f));
}

TEST_CASE("simd256_f32_rounding", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { 1.3f, 1.5f, 1.7f, -1.3f, -1.5f, -1.7f, 2.0f, 0.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);

	BX_ALIGN_DECL(32, float oc[8]);
	simd256_st(oc, simd256_f32_ceil(a));
	REQUIRE(oc[0] == Catch::Approx(2.0f));
	REQUIRE(oc[3] == Catch::Approx(-1.0f));

	BX_ALIGN_DECL(32, float of[8]);
	simd256_st(of, simd256_f32_floor(a));
	REQUIRE(of[0] == Catch::Approx(1.0f));
	REQUIRE(of[3] == Catch::Approx(-2.0f));

	BX_ALIGN_DECL(32, float orn[8]);
	simd256_st(orn, simd256_f32_round(a));
	REQUIRE(orn[0] == Catch::Approx(1.0f));
	REQUIRE(orn[2] == Catch::Approx(2.0f));
}

TEST_CASE("simd256_f32_cmp", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
	BX_ALIGN_DECL(32, float bd[8]) = { 1.0f, 3.0f, 2.0f, 4.0f, 6.0f, 5.0f, 7.0f, 9.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, uint32_t oeq[8]);
	simd256_st(oeq, simd256_f32_cmpeq(a, b));
	REQUIRE(oeq[0] == 0xffffffff);
	REQUIRE(oeq[1] == 0);

	BX_ALIGN_DECL(32, uint32_t olt[8]);
	simd256_st(olt, simd256_f32_cmplt(a, b));
	REQUIRE(olt[0] == 0);
	REQUIRE(olt[1] == 0xffffffff);

	BX_ALIGN_DECL(32, uint32_t ogt[8]);
	simd256_st(ogt, simd256_f32_cmpgt(a, b));
	REQUIRE(ogt[2] == 0xffffffff);

	BX_ALIGN_DECL(32, uint32_t one[8]);
	simd256_st(one, simd256_f32_cmpneq(a, b));
	REQUIRE(one[0] == 0);
	REQUIRE(one[1] == 0xffffffff);
}

TEST_CASE("simd256_f32_itof_ftoi", "[simd]")
{
	BX_ALIGN_DECL(32, int32_t id[8]) = { 1, -2, 3, -4, 5, -6, 7, -8 };
	const simd256_t a = simd256_ld<simd256_t>(id);
	BX_ALIGN_DECL(32, float of[8]);
	simd256_st(of, simd256_i32_itof(a));
	REQUIRE(of[0] == Catch::Approx(1.0f));
	REQUIRE(of[1] == Catch::Approx(-2.0f));

	BX_ALIGN_DECL(32, int32_t oi[8]);
	simd256_st(oi, simd256_f32_ftoi_trunc(simd256_ld<simd256_t>(of)));
	REQUIRE(oi[0] == 1);
	REQUIRE(oi[1] == -2);
}

TEST_CASE("simd256_i32_sub_neg_abs", "[simd]")
{
	BX_ALIGN_DECL(32, int32_t ad[8]) = { 10, -3, 5, -7, 1, -1, 0, 100 };
	BX_ALIGN_DECL(32, int32_t bd[8]) = { 3, 2, 5, -7, -1, 1, 0, 50 };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, int32_t osub[8]);
	simd256_st(osub, simd256_i32_sub(a, b));
	REQUIRE(osub[0] == 7);
	REQUIRE(osub[1] == -5);

	BX_ALIGN_DECL(32, int32_t oneg[8]);
	simd256_st(oneg, simd256_i32_neg(a));
	REQUIRE(oneg[0] == -10);
	REQUIRE(oneg[1] == 3);

	BX_ALIGN_DECL(32, int32_t oabs[8]);
	simd256_st(oabs, simd256_i32_abs(a));
	REQUIRE(oabs[0] == 10);
	REQUIRE(oabs[1] == 3);
}

TEST_CASE("simd256_i32_min_max_clamp", "[simd]")
{
	BX_ALIGN_DECL(32, int32_t ad[8]) = { -5, 5, 15, 3, -5, 5, 15, 3 };
	BX_ALIGN_DECL(32, int32_t mn[8]) = { 0, 0, 0, 0, 0, 0, 0, 0 };
	BX_ALIGN_DECL(32, int32_t mx[8]) = { 10, 10, 10, 10, 10, 10, 10, 10 };
	const simd256_t a = simd256_ld<simd256_t>(ad);

	BX_ALIGN_DECL(32, int32_t omin[8]);
	simd256_st(omin, simd256_i32_min(a, simd256_ld<simd256_t>(mx)));
	REQUIRE(omin[0] == -5);
	REQUIRE(omin[2] == 10);

	BX_ALIGN_DECL(32, int32_t omax[8]);
	simd256_st(omax, simd256_i32_max(a, simd256_ld<simd256_t>(mn)));
	REQUIRE(omax[0] == 0);
	REQUIRE(omax[1] == 5);

	BX_ALIGN_DECL(32, int32_t ocl[8]);
	simd256_st(ocl, simd256_i32_clamp(a, simd256_ld<simd256_t>(mn), simd256_ld<simd256_t>(mx)));
	REQUIRE(ocl[0] == 0);
	REQUIRE(ocl[1] == 5);
	REQUIRE(ocl[2] == 10);
}

TEST_CASE("simd256_i32_cmp", "[simd]")
{
	BX_ALIGN_DECL(32, int32_t ad[8]) = { 1, 2, 3, 4, 5, 6, 7, 8 };
	BX_ALIGN_DECL(32, int32_t bd[8]) = { 1, 3, 2, 4, 6, 5, 7, 9 };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, uint32_t oeq[8]);
	simd256_st(oeq, simd256_i32_cmpeq(a, b));
	REQUIRE(oeq[0] == 0xffffffff);
	REQUIRE(oeq[1] == 0);

	BX_ALIGN_DECL(32, uint32_t ogt[8]);
	simd256_st(ogt, simd256_i32_cmpgt(a, b));
	REQUIRE(ogt[2] == 0xffffffff);
	REQUIRE(ogt[1] == 0);

	BX_ALIGN_DECL(32, uint32_t olt[8]);
	simd256_st(olt, simd256_i32_cmplt(a, b));
	REQUIRE(olt[1] == 0xffffffff);
	REQUIRE(olt[0] == 0);
}

TEST_CASE("simd256_u32_ops", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t ad[8]) = { 10, 20, 30, 40, 50, 60, 70, 80 };
	BX_ALIGN_DECL(32, uint32_t bd[8]) = { 3, 5, 7, 9, 11, 13, 15, 17 };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, uint32_t oadd[8]);
	simd256_st(oadd, simd256_u32_add(a, b));
	REQUIRE(oadd[0] == 13);

	BX_ALIGN_DECL(32, uint32_t osub[8]);
	simd256_st(osub, simd256_u32_sub(a, b));
	REQUIRE(osub[0] == 7);

	BX_ALIGN_DECL(32, uint32_t omul[8]);
	simd256_st(omul, simd256_u32_mul(a, b));
	REQUIRE(omul[0] == 30);

	BX_ALIGN_DECL(32, uint32_t omin[8]);
	simd256_st(omin, simd256_u32_min(a, b));
	REQUIRE(omin[0] == 3);

	BX_ALIGN_DECL(32, uint32_t omax[8]);
	simd256_st(omax, simd256_u32_max(a, b));
	REQUIRE(omax[0] == 10);
}

TEST_CASE("simd256_u32_cmp", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t ad[8]) = { 1, 5, 3, 4, 1, 5, 3, 4 };
	BX_ALIGN_DECL(32, uint32_t bd[8]) = { 1, 3, 5, 4, 1, 3, 5, 4 };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, uint32_t oeq[8]);
	simd256_st(oeq, simd256_u32_cmpeq(a, b));
	REQUIRE(oeq[0] == 0xffffffff);
	REQUIRE(oeq[1] == 0);

	BX_ALIGN_DECL(32, uint32_t ogt[8]);
	simd256_st(ogt, simd256_u32_cmpgt(a, b));
	REQUIRE(ogt[1] == 0xffffffff);

	BX_ALIGN_DECL(32, uint32_t olt[8]);
	simd256_st(olt, simd256_u32_cmplt(a, b));
	REQUIRE(olt[2] == 0xffffffff);
}

TEST_CASE("simd256_i16_i8_ops", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t ad[8]) = { 0x00030005, 0x00070009, 0x00030005, 0x00070009, 0x00030005, 0x00070009, 0x00030005, 0x00070009 };
	BX_ALIGN_DECL(32, uint32_t bd[8]) = { 0x00010002, 0x00010002, 0x00010002, 0x00010002, 0x00010002, 0x00010002, 0x00010002, 0x00010002 };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, uint32_t oadd[8]);
	simd256_st(oadd, simd256_i16_add(a, b));
	REQUIRE(oadd[0] == 0x00040007);

	BX_ALIGN_DECL(32, uint32_t osub[8]);
	simd256_st(osub, simd256_i16_sub(a, b));
	REQUIRE(osub[0] == 0x00020003);

	BX_ALIGN_DECL(32, uint32_t omul[8]);
	simd256_st(omul, simd256_i16_mullo(a, b));
	REQUIRE(omul[0] == 0x0003000a);

	BX_ALIGN_DECL(32, uint32_t i8a[8]) = { 0x01020304, 0, 0, 0, 0x01020304, 0, 0, 0 };
	BX_ALIGN_DECL(32, uint32_t i8b[8]) = { 0x10101010, 0, 0, 0, 0x10101010, 0, 0, 0 };
	BX_ALIGN_DECL(32, uint32_t oi8a[8]);
	simd256_st(oi8a, simd256_i8_add(simd256_ld<simd256_t>(i8a), simd256_ld<simd256_t>(i8b)));
	REQUIRE(oi8a[0] == 0x11121314);

	BX_ALIGN_DECL(32, uint32_t oi8s[8]);
	simd256_st(oi8s, simd256_i8_sub(simd256_ld<simd256_t>(i8a), simd256_ld<simd256_t>(i8b)));
	REQUIRE(oi8s[0] == 0xf1f2f3f4);
}

TEST_CASE("simd256_u8_u16_sat", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t u8a[8]) = { 0xf0f0f0f0, 0, 0, 0, 0xf0f0f0f0, 0, 0, 0 };
	BX_ALIGN_DECL(32, uint32_t u8b[8]) = { 0x20202020, 0, 0, 0, 0x20202020, 0, 0, 0 };
	BX_ALIGN_DECL(32, uint32_t osat[8]);
	simd256_st(osat, simd256_u8_satadd(simd256_ld<simd256_t>(u8a), simd256_ld<simd256_t>(u8b)));
	REQUIRE(osat[0] == 0xffffffff);

	BX_ALIGN_DECL(32, uint32_t u8c[8]) = { 0x10101010, 0, 0, 0, 0x10101010, 0, 0, 0 };
	BX_ALIGN_DECL(32, uint32_t osats[8]);
	simd256_st(osats, simd256_u8_satsub(simd256_ld<simd256_t>(u8c), simd256_ld<simd256_t>(u8b)));
	REQUIRE(osats[0] == 0);

	BX_ALIGN_DECL(32, uint32_t u16a[8]) = { 0xfff0fff0, 0, 0, 0, 0xfff0fff0, 0, 0, 0 };
	BX_ALIGN_DECL(32, uint32_t u16b[8]) = { 0x00200020, 0, 0, 0, 0x00200020, 0, 0, 0 };
	BX_ALIGN_DECL(32, uint32_t ou16a[8]);
	simd256_st(ou16a, simd256_u16_satadd(simd256_ld<simd256_t>(u16a), simd256_ld<simd256_t>(u16b)));
	REQUIRE(ou16a[0] == 0xffffffff);

	BX_ALIGN_DECL(32, uint32_t u16c[8]) = { 0x00100010, 0, 0, 0, 0x00100010, 0, 0, 0 };
	BX_ALIGN_DECL(32, uint32_t ou16s[8]);
	simd256_st(ou16s, simd256_u16_satsub(simd256_ld<simd256_t>(u16c), simd256_ld<simd256_t>(u16b)));
	REQUIRE(ou16s[0] == 0);
}

TEST_CASE("simd256_bitwise_full", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t ad[8]) = { 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00 };
	BX_ALIGN_DECL(32, uint32_t bd[8]) = { 0x0ff00ff0, 0x0ff00ff0, 0x0ff00ff0, 0x0ff00ff0, 0x0ff00ff0, 0x0ff00ff0, 0x0ff00ff0, 0x0ff00ff0 };
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, uint32_t out[8]);

	simd256_st(out, simd256_andc(a, b));
	REQUIRE(out[0] == 0xf000f000);

	simd256_st(out, simd256_or(a, b));
	REQUIRE(out[0] == 0xfff0fff0);

	simd256_st(out, simd256_orc(a, b));
	REQUIRE(out[0] == 0xff0fff0f);

	simd256_st(out, simd256_xor(a, b));
	REQUIRE(out[0] == 0xf0f0f0f0);

	simd256_st(out, simd256_not(a));
	REQUIRE(out[0] == 0x00ff00ff);
}

TEST_CASE("simd256_shifts", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t ad[8]) = { 1, 2, 4, 8, 16, 32, 64, 128 };
	const simd256_t a = simd256_ld<simd256_t>(ad);

	BX_ALIGN_DECL(32, uint32_t osll[8]);
	simd256_st(osll, simd256_x32_sll(a, 1));
	REQUIRE(osll[0] == 2);
	REQUIRE(osll[1] == 4);

	BX_ALIGN_DECL(32, uint32_t osrl[8]);
	simd256_st(osrl, simd256_x32_srl(a, 1));
	REQUIRE(osrl[1] == 1);

	BX_ALIGN_DECL(32, int32_t srad[8]) = { -8, -4, -2, -1, 8, 4, 2, 1 };
	BX_ALIGN_DECL(32, int32_t osra[8]);
	simd256_st(osra, simd256_x32_sra(simd256_ld<simd256_t>(srad), 1));
	REQUIRE(osra[0] == -4);
	REQUIRE(osra[4] == 4);
}

TEST_CASE("simd256_selb_sels", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t md[8]) = { 0xffffffff, 0, 0xffffffff, 0, 0xffffffff, 0, 0xffffffff, 0 };
	BX_ALIGN_DECL(32, float ad[8]) = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	BX_ALIGN_DECL(32, float bd[8]) = { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };
	const simd256_t mask = simd256_ld<simd256_t>(md);
	const simd256_t a = simd256_ld<simd256_t>(ad);
	const simd256_t b = simd256_ld<simd256_t>(bd);

	BX_ALIGN_DECL(32, float oselb[8]);
	simd256_st(oselb, simd256_selb(mask, a, b));
	REQUIRE(oselb[0] == Catch::Approx(1.0f));
	REQUIRE(oselb[1] == Catch::Approx(2.0f));

	BX_ALIGN_DECL(32, uint32_t sd[8]) = { 0x80000000, 0, 0x80000000, 0, 0x80000000, 0, 0x80000000, 0 };
	BX_ALIGN_DECL(32, float osels[8]);
	simd256_st(osels, simd256_sels(simd256_ld<simd256_t>(sd), a, b));
	REQUIRE(osels[0] == Catch::Approx(1.0f));
	REQUIRE(osels[1] == Catch::Approx(2.0f));
}

TEST_CASE("simd256_test_any_all", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t ad[8]) = { 0xffffffff, 0, 0, 0, 0, 0, 0, 0 };
	REQUIRE(simd256_test_any(simd256_ld<simd256_t>(ad)) == true);

	BX_ALIGN_DECL(32, uint32_t bd[8]) = { 0, 0, 0, 0, 0, 0, 0, 0 };
	REQUIRE(simd256_test_any(simd256_ld<simd256_t>(bd)) == false);

	BX_ALIGN_DECL(32, uint32_t cd[8]) = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
	REQUIRE(simd256_test_all(simd256_ld<simd256_t>(cd)) == true);
	REQUIRE(simd256_test_all(simd256_ld<simd256_t>(ad)) == false);
}

TEST_CASE("simd256_signbits", "[simd]")
{
	BX_ALIGN_DECL(32, uint32_t ad[8]) = { 0x80000000, 0, 0x80000000, 0, 0, 0x80000000, 0, 0x80000000 };
	REQUIRE(simd256_x32_signbitsmask(simd256_ld<simd256_t>(ad)) == 0xa5);
	REQUIRE(simd256_x8_signbitsmask(simd256_zero<simd256_t>()) == 0);
}

TEST_CASE("simd256_f32_transcendental", "[simd]")
{
	BX_ALIGN_DECL(32, float ad[8]) = { 1.0f, 4.0f, 8.0f, 16.0f, 1.0f, 4.0f, 8.0f, 16.0f };
	const simd256_t a = simd256_ld<simd256_t>(ad);

	BX_ALIGN_DECL(32, float olog2[8]);
	simd256_st(olog2, simd_f32_log2(a));
	REQUIRE(olog2[0] == Catch::Approx(0.0f).margin(0.01f));
	REQUIRE(olog2[1] == Catch::Approx(2.0f).margin(0.01f));

	BX_ALIGN_DECL(32, float oexp2[8]);
	simd256_st(oexp2, simd_f32_exp2(simd256_ld<simd256_t>(olog2)));
	REQUIRE(oexp2[0] == Catch::Approx(1.0f).margin(0.01f));
	REQUIRE(oexp2[1] == Catch::Approx(4.0f).margin(0.01f));

	BX_ALIGN_DECL(32, float base[8]) = { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };
	BX_ALIGN_DECL(32, float expo[8]) = { 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f };
	BX_ALIGN_DECL(32, float opow[8]);
	simd256_st(opow, simd_f32_pow(simd256_ld<simd256_t>(base), simd256_ld<simd256_t>(expo)));
	REQUIRE(opow[0] == Catch::Approx(8.0f).margin(0.01f));

	BX_ALIGN_DECL(32, float cosv[8]) = { 0.0f, kPiHalf, 0.0f, kPiHalf, 0.0f, kPiHalf, 0.0f, kPiHalf };
	BX_ALIGN_DECL(32, float ocos[8]);
	simd256_st(ocos, simd_f32_cos(simd256_ld<simd256_t>(cosv)));
	REQUIRE(ocos[0] == Catch::Approx(1.0f).margin(0.001f));
	REQUIRE(ocos[1] == Catch::Approx(0.0f).margin(0.001f));

	BX_ALIGN_DECL(32, float osin[8]);
	simd256_st(osin, simd_f32_sin(simd256_ld<simd256_t>(cosv)));
	REQUIRE(osin[0] == Catch::Approx(0.0f).margin(0.001f));
	REQUIRE(osin[1] == Catch::Approx(1.0f).margin(0.001f));

	BX_ALIGN_DECL(32, float logv[8]) = { 1.0f, kE, 1.0f, kE, 1.0f, kE, 1.0f, kE };
	BX_ALIGN_DECL(32, float olog[8]);
	simd256_st(olog, simd_f32_log(simd256_ld<simd256_t>(logv)));
	REQUIRE(olog[0] == Catch::Approx(0.0f).margin(0.01f));
	REQUIRE(olog[1] == Catch::Approx(1.0f).margin(0.01f));

	BX_ALIGN_DECL(32, float expv[8]) = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
	BX_ALIGN_DECL(32, float oexp[8]);
	simd256_st(oexp, simd_f32_exp(simd256_ld<simd256_t>(expv)));
	REQUIRE(oexp[0] == Catch::Approx(1.0f).margin(0.01f));
	REQUIRE(oexp[1] == Catch::Approx(kE).margin(0.01f));
}
