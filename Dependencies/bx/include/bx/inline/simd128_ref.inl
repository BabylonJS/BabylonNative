/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#	error "Must be included from bx/simd_t.h!"
#endif // BX_SIMD_T_H_HEADER_GUARD

namespace bx
{
	BX_ALIGN_DECL(16, struct) simd128_f32_ref_t { float    f32[4]; };
	BX_ALIGN_DECL(16, struct) simd128_f64_ref_t { double   f64[2]; };
	BX_ALIGN_DECL(16, struct) simd128_i8_ref_t  { int8_t   i8[16]; };
	BX_ALIGN_DECL(16, struct) simd128_i16_ref_t { int16_t  i16[8]; };
	BX_ALIGN_DECL(16, struct) simd128_i32_ref_t { int32_t  i32[4]; };
	BX_ALIGN_DECL(16, struct) simd128_i64_ref_t { int64_t  i64[2]; };
	BX_ALIGN_DECL(16, struct) simd128_u8_ref_t  { uint8_t  u8[16]; };
	BX_ALIGN_DECL(16, struct) simd128_u16_ref_t { uint16_t u16[8]; };
	BX_ALIGN_DECL(16, struct) simd128_u64_ref_t { uint64_t u64[2]; };

#if BX_SIMD_LANGEXT
	typedef float    simd128_f32_langext_t __attribute__((__vector_size__(16), __aligned__(16)));
	typedef double   simd128_f64_langext_t __attribute__((__vector_size__(16), __aligned__(16)));
	typedef int8_t   simd128_i8_langext_t  __attribute__((__vector_size__(16), __aligned__(16)));
	typedef int16_t  simd128_i16_langext_t __attribute__((__vector_size__(16), __aligned__(16)));
	typedef int32_t  simd128_i32_langext_t __attribute__((__vector_size__(16), __aligned__(16)));
	typedef int64_t  simd128_i64_langext_t __attribute__((__vector_size__(16), __aligned__(16)));
	typedef uint8_t  simd128_u8_langext_t  __attribute__((__vector_size__(16), __aligned__(16)));
	typedef uint16_t simd128_u16_langext_t __attribute__((__vector_size__(16), __aligned__(16)));
	typedef uint32_t simd128_u32_langext_t __attribute__((__vector_size__(16), __aligned__(16)));
	typedef uint64_t simd128_u64_langext_t __attribute__((__vector_size__(16), __aligned__(16)));
#endif // BX_SIMD_LANGEXT

#define ELEMx 0
#define ELEMy 1
#define ELEMz 2
#define ELEMw 3
#define BX_SIMD128_IMPLEMENT_SWIZZLE(_x, _y, _z, _w)                                            \
			template<>                                                                          \
			inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_swiz_##_x##_y##_z##_w(simd128_ref_t _a)  \
			{                                                                                   \
				simd128_ref_t result;                                                           \
				result.u32[0] = _a.u32[ELEM##_x];                                           \
				result.u32[1] = _a.u32[ELEM##_y];                                           \
				result.u32[2] = _a.u32[ELEM##_z];                                           \
				result.u32[3] = _a.u32[ELEM##_w];                                           \
				return result;                                                                  \
			}

#include "simd128_swizzle.inl"

#undef BX_SIMD128_IMPLEMENT_SWIZZLE
#undef ELEMw
#undef ELEMz
#undef ELEMy
#undef ELEMx

#define BX_SIMD128_IMPLEMENT_TEST(_xyzw, _mask)                                     \
			template<>                                                              \
			inline BX_CONSTEXPR_FUNC bool simd128_test_any_##_xyzw(simd128_ref_t _test) \
			{                                                                       \
				uint32_t tmp = ( (_test.u32[3]>>31)<<3)                           \
				             | ( (_test.u32[2]>>31)<<2)                           \
				             | ( (_test.u32[1]>>31)<<1)                           \
				             | (  _test.u32[0]>>31)                               \
				             ;                                                      \
				return 0 != (tmp&(_mask) );                                         \
			}                                                                       \
			                                                                        \
			template<>                                                              \
			inline BX_CONSTEXPR_FUNC bool simd128_test_all_##_xyzw(simd128_ref_t _test) \
			{                                                                       \
				uint32_t tmp = ( (_test.u32[3]>>31)<<3)                           \
				             | ( (_test.u32[2]>>31)<<2)                           \
				             | ( (_test.u32[1]>>31)<<1)                           \
				             | (  _test.u32[0]>>31)                               \
				             ;                                                      \
				return (_mask) == (tmp&(_mask) );                                   \
			}

	BX_SIMD128_IMPLEMENT_TEST(x    , 0x1)
	BX_SIMD128_IMPLEMENT_TEST(y    , 0x2)
	BX_SIMD128_IMPLEMENT_TEST(xy   , 0x3)
	BX_SIMD128_IMPLEMENT_TEST(z    , 0x4)
	BX_SIMD128_IMPLEMENT_TEST(xz   , 0x5)
	BX_SIMD128_IMPLEMENT_TEST(yz   , 0x6)
	BX_SIMD128_IMPLEMENT_TEST(xyz  , 0x7)
	BX_SIMD128_IMPLEMENT_TEST(w    , 0x8)
	BX_SIMD128_IMPLEMENT_TEST(xw   , 0x9)
	BX_SIMD128_IMPLEMENT_TEST(yw   , 0xa)
	BX_SIMD128_IMPLEMENT_TEST(xyw  , 0xb)
	BX_SIMD128_IMPLEMENT_TEST(zw   , 0xc)
	BX_SIMD128_IMPLEMENT_TEST(xzw  , 0xd)
	BX_SIMD128_IMPLEMENT_TEST(yzw  , 0xe)
	BX_SIMD128_IMPLEMENT_TEST(xyzw , 0xf)

#undef BX_SIMD128_IMPLEMENT_TEST

	template<>
	inline BX_CONSTEXPR_FUNC bool simd128_test_zero(simd128_ref_t _a, simd128_ref_t _b)
	{
		return 0 == ( (_a.u32[0] & _b.u32[0])
		            | (_a.u32[1] & _b.u32[1])
		            | (_a.u32[2] & _b.u32[2])
		            | (_a.u32[3] & _b.u32[3])
		            );
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_xyAB(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[0];
		result.u32[1] = _a.u32[1];
		result.u32[2] = _b.u32[0];
		result.u32[3] = _b.u32[1];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_ABxy(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _b.u32[0];
		result.u32[1] = _b.u32[1];
		result.u32[2] = _a.u32[0];
		result.u32[3] = _a.u32[1];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_CDzw(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _b.u32[2];
		result.u32[1] = _b.u32[3];
		result.u32[2] = _a.u32[2];
		result.u32[3] = _a.u32[3];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_zwCD(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[2];
		result.u32[1] = _a.u32[3];
		result.u32[2] = _b.u32[2];
		result.u32[3] = _b.u32[3];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_xAyB(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[0];
		result.u32[1] = _b.u32[0];
		result.u32[2] = _a.u32[1];
		result.u32[3] = _b.u32[1];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_AxBy(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _b.u32[0];
		result.u32[1] = _a.u32[0];
		result.u32[2] = _b.u32[1];
		result.u32[3] = _a.u32[1];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_zCwD(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[2];
		result.u32[1] = _b.u32[2];
		result.u32[2] = _a.u32[3];
		result.u32[3] = _b.u32[3];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_CzDw(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _b.u32[2];
		result.u32[1] = _a.u32[2];
		result.u32[2] = _b.u32[3];
		result.u32[3] = _a.u32[3];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_xzAC(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[0];
		result.u32[1] = _a.u32[2];
		result.u32[2] = _b.u32[0];
		result.u32[3] = _b.u32[2];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_ywBD(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[1];
		result.u32[1] = _a.u32[3];
		result.u32[2] = _b.u32[1];
		result.u32[3] = _b.u32[3];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_xxAA(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[0];
		result.u32[1] = _a.u32[0];
		result.u32[2] = _b.u32[0];
		result.u32[3] = _b.u32[0];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_yyBB(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[1];
		result.u32[1] = _a.u32[1];
		result.u32[2] = _b.u32[1];
		result.u32[3] = _b.u32[1];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_zzCC(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[2];
		result.u32[1] = _a.u32[2];
		result.u32[2] = _b.u32[2];
		result.u32[3] = _b.u32[2];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_wwDD(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[3];
		result.u32[1] = _a.u32[3];
		result.u32[2] = _b.u32[3];
		result.u32[3] = _b.u32[3];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC float simd128_f32_x(simd128_ref_t _a) { return bitCast<simd128_f32_ref_t>(_a).f32[0]; }

	template<>
	inline BX_CONSTEXPR_FUNC float simd128_f32_y(simd128_ref_t _a) { return bitCast<simd128_f32_ref_t>(_a).f32[1]; }

	template<>
	inline BX_CONSTEXPR_FUNC float simd128_f32_z(simd128_ref_t _a) { return bitCast<simd128_f32_ref_t>(_a).f32[2]; }

	template<>
	inline BX_CONSTEXPR_FUNC float simd128_f32_w(simd128_ref_t _a) { return bitCast<simd128_f32_ref_t>(_a).f32[3]; }

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_ld(const void* _ptr)
	{
		simd128_ref_t result;
		memCopy(&result, _ptr, sizeof(simd128_ref_t) );
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_ldu(const void* _ptr)
	{
		return simd128_ld<simd128_ref_t>(_ptr);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_st(void* _ptr, simd128_ref_t _a)
	{
		memCopy(_ptr, &_a, sizeof(simd128_ref_t) );
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_stu(void* _ptr, simd128_ref_t _a)
	{
		simd128_st(_ptr, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_x32_st1(void* _ptr, simd128_ref_t _a)
	{
		float* result = reinterpret_cast<float*>(_ptr);
		*result = bitCast<simd128_f32_ref_t>(_a).f32[0];
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_stream(void* _ptr, simd128_ref_t _a)
	{
		simd128_ref_t* result = reinterpret_cast<simd128_ref_t*>(_ptr);
		*result = _a;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_ld(float _x, float _y, float _z, float _w)
	{
		const simd128_f32_ref_t result = { { _x, _y, _z, _w } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_ld(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w)
	{
		simd128_ref_t result;
		result.u32[0] = _x;
		result.u32[1] = _y;
		result.u32[2] = _z;
		result.u32[3] = _w;
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_ld(int32_t _x, int32_t _y, int32_t _z, int32_t _w)
	{
		simd128_ref_t result;
		result.u32[0] = uint32_t(_x);
		result.u32[1] = uint32_t(_y);
		result.u32[2] = uint32_t(_z);
		result.u32[3] = uint32_t(_w);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_splat(float _a)
	{
		return simd128_ld<simd128_ref_t>(_a, _a, _a, _a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_splat(int32_t _a)
	{
		return simd128_ld<simd128_ref_t>(_a, _a, _a, _a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_splat(uint32_t _a)
	{
		return simd128_ld<simd128_ref_t>(_a, _a, _a, _a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_splat(double _a)
	{
		const simd128_f64_ref_t result = { { _a, _a } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_zero()
	{
		return simd128_ld<simd128_ref_t>(0u, 0u, 0u, 0u);
	}

	template<>
	inline BX_CONST_FUNC simd128_ref_t simd128_i32_itof(simd128_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd128_i32_langext_t a      = bitCast<simd128_i32_langext_t>(_a);
		const simd128_f32_langext_t conv   = __builtin_convertvector(a, simd128_f32_langext_t);
		const simd128_ref_t         result = bitCast<simd128_ref_t>(conv);
		return result;
#else
		const simd128_i32_ref_t a = bitCast<simd128_i32_ref_t>(_a);
		const simd128_f32_ref_t result = { { (float)a.i32[0], (float)a.i32[1], (float)a.i32[2], (float)a.i32[3] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONST_FUNC simd128_ref_t simd128_f32_ftoi_trunc(simd128_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_i32_langext_t conv   = __builtin_convertvector(a, simd128_i32_langext_t);
		const simd128_ref_t         result = bitCast<simd128_ref_t>(conv);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_i32_ref_t result = { { (int32_t)a.f32[0], (int32_t)a.f32[1], (int32_t)a.f32[2], (int32_t)a.f32[3] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_ftoi_round(simd128_ref_t _a)
	{
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_i32_ref_t result = { { (int32_t)round(a.f32[0]), (int32_t)round(a.f32[1]), (int32_t)round(a.f32[2]), (int32_t)round(a.f32[3]) } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_add(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t b      = bitCast<simd128_f32_langext_t>(_b);
		const simd128_f32_langext_t sum    = a + b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		const simd128_f32_ref_t result = { { a.f32[0] + b.f32[0], a.f32[1] + b.f32[1], a.f32[2] + b.f32[2], a.f32[3] + b.f32[3] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_sub(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t b      = bitCast<simd128_f32_langext_t>(_b);
		const simd128_f32_langext_t diff   = a - b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(diff);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		const simd128_f32_ref_t result = { { a.f32[0] - b.f32[0], a.f32[1] - b.f32[1], a.f32[2] - b.f32[2], a.f32[3] - b.f32[3] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_mul(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t b      = bitCast<simd128_f32_langext_t>(_b);
		const simd128_f32_langext_t prod   = a * b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(prod);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		const simd128_f32_ref_t result = { { a.f32[0] * b.f32[0], a.f32[1] * b.f32[1], a.f32[2] * b.f32[2], a.f32[3] * b.f32[3] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_div(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t b      = bitCast<simd128_f32_langext_t>(_b);
		const simd128_f32_langext_t quot   = a / b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(quot);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		const simd128_f32_ref_t result = { { a.f32[0] / b.f32[0], a.f32[1] / b.f32[1], a.f32[2] / b.f32[2], a.f32[3] / b.f32[3] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_rcp_est(simd128_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t one    = {1.0f, 1.0f, 1.0f, 1.0f};
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t quot   = one / a;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(quot);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t result = { { 1.0f / a.f32[0], 1.0f / a.f32[1], 1.0f / a.f32[2], 1.0f / a.f32[3] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_sqrt(simd128_ref_t _a)
	{
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t result = { {
			sqrt(a.f32[0]),
			sqrt(a.f32[1]),
			sqrt(a.f32[2]),
			sqrt(a.f32[3]),
		} };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_rsqrt_est(simd128_ref_t _a)
	{
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t result = { {
			1.0f / sqrt(a.f32[0]),
			1.0f / sqrt(a.f32[1]),
			1.0f / sqrt(a.f32[2]),
			1.0f / sqrt(a.f32[3]),
		} };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_min(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		const simd128_f32_ref_t result = { { a.f32[0] < b.f32[0] ? a.f32[0] : b.f32[0], a.f32[1] < b.f32[1] ? a.f32[1] : b.f32[1], a.f32[2] < b.f32[2] ? a.f32[2] : b.f32[2], a.f32[3] < b.f32[3] ? a.f32[3] : b.f32[3] } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_max(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		const simd128_f32_ref_t result = { { a.f32[0] > b.f32[0] ? a.f32[0] : b.f32[0], a.f32[1] > b.f32[1] ? a.f32[1] : b.f32[1], a.f32[2] > b.f32[2] ? a.f32[2] : b.f32[2], a.f32[3] > b.f32[3] ? a.f32[3] : b.f32[3] } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_round(simd128_ref_t _a)
	{
		return simd_f32_round_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_ceil(simd128_ref_t _a)
	{
		return simd_f32_ceil_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_floor(simd128_ref_t _a)
	{
		return simd_f32_floor_ni(_a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_cmpeq(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t b      = bitCast<simd128_f32_langext_t>(_b);
		const auto                  mask   = a == b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		simd128_ref_t result;
		result.u32[0] = a.f32[0] == b.f32[0] ? 0xffffffff : 0;
		result.u32[1] = a.f32[1] == b.f32[1] ? 0xffffffff : 0;
		result.u32[2] = a.f32[2] == b.f32[2] ? 0xffffffff : 0;
		result.u32[3] = a.f32[3] == b.f32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_cmplt(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t b      = bitCast<simd128_f32_langext_t>(_b);
		const auto                  mask   = a < b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		simd128_ref_t result;
		result.u32[0] = a.f32[0] < b.f32[0] ? 0xffffffff : 0;
		result.u32[1] = a.f32[1] < b.f32[1] ? 0xffffffff : 0;
		result.u32[2] = a.f32[2] < b.f32[2] ? 0xffffffff : 0;
		result.u32[3] = a.f32[3] < b.f32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_cmple(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t b      = bitCast<simd128_f32_langext_t>(_b);
		const auto                  mask   = a <= b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		simd128_ref_t result;
		result.u32[0] = a.f32[0] <= b.f32[0] ? 0xffffffff : 0;
		result.u32[1] = a.f32[1] <= b.f32[1] ? 0xffffffff : 0;
		result.u32[2] = a.f32[2] <= b.f32[2] ? 0xffffffff : 0;
		result.u32[3] = a.f32[3] <= b.f32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_cmpgt(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t b      = bitCast<simd128_f32_langext_t>(_b);
		const auto                  mask   = a > b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		simd128_ref_t result;
		result.u32[0] = a.f32[0] > b.f32[0] ? 0xffffffff : 0;
		result.u32[1] = a.f32[1] > b.f32[1] ? 0xffffffff : 0;
		result.u32[2] = a.f32[2] > b.f32[2] ? 0xffffffff : 0;
		result.u32[3] = a.f32[3] > b.f32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f32_cmpge(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f32_langext_t a      = bitCast<simd128_f32_langext_t>(_a);
		const simd128_f32_langext_t b      = bitCast<simd128_f32_langext_t>(_b);
		const auto                  mask   = a >= b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f32_ref_t a = bitCast<simd128_f32_ref_t>(_a);
		const simd128_f32_ref_t b = bitCast<simd128_f32_ref_t>(_b);
		simd128_ref_t result;
		result.u32[0] = a.f32[0] >= b.f32[0] ? 0xffffffff : 0;
		result.u32[1] = a.f32[1] >= b.f32[1] ? 0xffffffff : 0;
		result.u32[2] = a.f32[2] >= b.f32[2] ? 0xffffffff : 0;
		result.u32[3] = a.f32[3] >= b.f32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i32_add(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i32_langext_t a      = bitCast<simd128_i32_langext_t>(_a);
		const simd128_i32_langext_t b      = bitCast<simd128_i32_langext_t>(_b);
		const simd128_i32_langext_t sum    = a + b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		const simd128_i32_ref_t a = bitCast<simd128_i32_ref_t>(_a);
		const simd128_i32_ref_t b = bitCast<simd128_i32_ref_t>(_b);
		const simd128_i32_ref_t result = { { a.i32[0] + b.i32[0], a.i32[1] + b.i32[1], a.i32[2] + b.i32[2], a.i32[3] + b.i32[3] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i32_sub(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i32_langext_t a      = bitCast<simd128_i32_langext_t>(_a);
		const simd128_i32_langext_t b      = bitCast<simd128_i32_langext_t>(_b);
		const simd128_i32_langext_t diff   = a - b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(diff);
		return result;
#else
		const simd128_i32_ref_t a = bitCast<simd128_i32_ref_t>(_a);
		const simd128_i32_ref_t b = bitCast<simd128_i32_ref_t>(_b);
		const simd128_i32_ref_t result = { { a.i32[0] - b.i32[0], a.i32[1] - b.i32[1], a.i32[2] - b.i32[2], a.i32[3] - b.i32[3] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_i32_neg(simd128_ref_t _a)
	{
		return simd_i32_neg_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_i32_abs(simd128_ref_t _a)
	{
		return simd_i32_abs_ni(_a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i32_min(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_i32_ref_t a = bitCast<simd128_i32_ref_t>(_a);
		const simd128_i32_ref_t b = bitCast<simd128_i32_ref_t>(_b);
		const simd128_i32_ref_t result = { {
			a.i32[0] < b.i32[0] ? a.i32[0] : b.i32[0],
			a.i32[1] < b.i32[1] ? a.i32[1] : b.i32[1],
			a.i32[2] < b.i32[2] ? a.i32[2] : b.i32[2],
			a.i32[3] < b.i32[3] ? a.i32[3] : b.i32[3],
		} };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i32_max(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_i32_ref_t a = bitCast<simd128_i32_ref_t>(_a);
		const simd128_i32_ref_t b = bitCast<simd128_i32_ref_t>(_b);
		const simd128_i32_ref_t result = { {
			a.i32[0] > b.i32[0] ? a.i32[0] : b.i32[0],
			a.i32[1] > b.i32[1] ? a.i32[1] : b.i32[1],
			a.i32[2] > b.i32[2] ? a.i32[2] : b.i32[2],
			a.i32[3] > b.i32[3] ? a.i32[3] : b.i32[3],
		} };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i32_cmpeq(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i32_langext_t a      = bitCast<simd128_i32_langext_t>(_a);
		const simd128_i32_langext_t b      = bitCast<simd128_i32_langext_t>(_b);
		const auto                  mask   = a == b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_i32_ref_t a = bitCast<simd128_i32_ref_t>(_a);
		const simd128_i32_ref_t b = bitCast<simd128_i32_ref_t>(_b);
		simd128_ref_t result;
		result.u32[0] = a.i32[0] == b.i32[0] ? 0xffffffff : 0;
		result.u32[1] = a.i32[1] == b.i32[1] ? 0xffffffff : 0;
		result.u32[2] = a.i32[2] == b.i32[2] ? 0xffffffff : 0;
		result.u32[3] = a.i32[3] == b.i32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i32_cmplt(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i32_langext_t a      = bitCast<simd128_i32_langext_t>(_a);
		const simd128_i32_langext_t b      = bitCast<simd128_i32_langext_t>(_b);
		const auto                  mask   = a < b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_i32_ref_t a = bitCast<simd128_i32_ref_t>(_a);
		const simd128_i32_ref_t b = bitCast<simd128_i32_ref_t>(_b);
		simd128_ref_t result;
		result.u32[0] = a.i32[0] < b.i32[0] ? 0xffffffff : 0;
		result.u32[1] = a.i32[1] < b.i32[1] ? 0xffffffff : 0;
		result.u32[2] = a.i32[2] < b.i32[2] ? 0xffffffff : 0;
		result.u32[3] = a.i32[3] < b.i32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i32_cmpgt(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i32_langext_t a      = bitCast<simd128_i32_langext_t>(_a);
		const simd128_i32_langext_t b      = bitCast<simd128_i32_langext_t>(_b);
		const auto                  mask   = a > b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_i32_ref_t a = bitCast<simd128_i32_ref_t>(_a);
		const simd128_i32_ref_t b = bitCast<simd128_i32_ref_t>(_b);
		simd128_ref_t result;
		result.u32[0] = a.i32[0] > b.i32[0] ? 0xffffffff : 0;
		result.u32[1] = a.i32[1] > b.i32[1] ? 0xffffffff : 0;
		result.u32[2] = a.i32[2] > b.i32[2] ? 0xffffffff : 0;
		result.u32[3] = a.i32[3] > b.i32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i32_clamp(simd128_ref_t _a, simd128_ref_t _min, simd128_ref_t _max)
	{
		const simd128_ref_t hi     = simd128_i32_max(_a, _min);
		const simd128_ref_t result = simd128_i32_min(hi, _max);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u32_add(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const simd128_u32_langext_t sum    = a + b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] + _b.u32[0];
		result.u32[1] = _a.u32[1] + _b.u32[1];
		result.u32[2] = _a.u32[2] + _b.u32[2];
		result.u32[3] = _a.u32[3] + _b.u32[3];
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u32_sub(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const simd128_u32_langext_t diff   = a - b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(diff);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] - _b.u32[0];
		result.u32[1] = _a.u32[1] - _b.u32[1];
		result.u32[2] = _a.u32[2] - _b.u32[2];
		result.u32[3] = _a.u32[3] - _b.u32[3];
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u32_mul(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const simd128_u32_langext_t prod   = a * b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(prod);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] * _b.u32[0];
		result.u32[1] = _a.u32[1] * _b.u32[1];
		result.u32[2] = _a.u32[2] * _b.u32[2];
		result.u32[3] = _a.u32[3] * _b.u32[3];
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u32_min(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] < _b.u32[0] ? _a.u32[0] : _b.u32[0];
		result.u32[1] = _a.u32[1] < _b.u32[1] ? _a.u32[1] : _b.u32[1];
		result.u32[2] = _a.u32[2] < _b.u32[2] ? _a.u32[2] : _b.u32[2];
		result.u32[3] = _a.u32[3] < _b.u32[3] ? _a.u32[3] : _b.u32[3];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u32_max(simd128_ref_t _a, simd128_ref_t _b)
	{
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] > _b.u32[0] ? _a.u32[0] : _b.u32[0];
		result.u32[1] = _a.u32[1] > _b.u32[1] ? _a.u32[1] : _b.u32[1];
		result.u32[2] = _a.u32[2] > _b.u32[2] ? _a.u32[2] : _b.u32[2];
		result.u32[3] = _a.u32[3] > _b.u32[3] ? _a.u32[3] : _b.u32[3];
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u32_clamp(simd128_ref_t _a, simd128_ref_t _min, simd128_ref_t _max)
	{
		const simd128_ref_t hi     = simd128_u32_max(_a, _min);
		const simd128_ref_t result = simd128_u32_min(hi, _max);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u32_cmpeq(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const auto                  mask   = a == b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] == _b.u32[0] ? 0xffffffff : 0;
		result.u32[1] = _a.u32[1] == _b.u32[1] ? 0xffffffff : 0;
		result.u32[2] = _a.u32[2] == _b.u32[2] ? 0xffffffff : 0;
		result.u32[3] = _a.u32[3] == _b.u32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u32_cmplt(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const auto                  mask   = a < b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] < _b.u32[0] ? 0xffffffff : 0;
		result.u32[1] = _a.u32[1] < _b.u32[1] ? 0xffffffff : 0;
		result.u32[2] = _a.u32[2] < _b.u32[2] ? 0xffffffff : 0;
		result.u32[3] = _a.u32[3] < _b.u32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u32_cmpgt(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const auto                  mask   = a > b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] > _b.u32[0] ? 0xffffffff : 0;
		result.u32[1] = _a.u32[1] > _b.u32[1] ? 0xffffffff : 0;
		result.u32[2] = _a.u32[2] > _b.u32[2] ? 0xffffffff : 0;
		result.u32[3] = _a.u32[3] > _b.u32[3] ? 0xffffffff : 0;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i16_add(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i16_langext_t a      = bitCast<simd128_i16_langext_t>(_a);
		const simd128_i16_langext_t b      = bitCast<simd128_i16_langext_t>(_b);
		const simd128_i16_langext_t sum    = a + b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		const simd128_i16_ref_t a = bitCast<simd128_i16_ref_t>(_a);
		const simd128_i16_ref_t b = bitCast<simd128_i16_ref_t>(_b);
		simd128_i16_ref_t result;
		for (int ii = 0; ii < 8; ++ii) { result.i16[ii] = a.i16[ii] + b.i16[ii]; }
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i16_sub(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i16_langext_t a      = bitCast<simd128_i16_langext_t>(_a);
		const simd128_i16_langext_t b      = bitCast<simd128_i16_langext_t>(_b);
		const simd128_i16_langext_t diff   = a - b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(diff);
		return result;
#else
		const simd128_i16_ref_t a = bitCast<simd128_i16_ref_t>(_a);
		const simd128_i16_ref_t b = bitCast<simd128_i16_ref_t>(_b);
		simd128_i16_ref_t result;
		for (int ii = 0; ii < 8; ++ii) { result.i16[ii] = a.i16[ii] - b.i16[ii]; }
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i16_mullo(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i16_langext_t a      = bitCast<simd128_i16_langext_t>(_a);
		const simd128_i16_langext_t b      = bitCast<simd128_i16_langext_t>(_b);
		const simd128_i16_langext_t prod   = a * b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(prod);
		return result;
#else
		const simd128_i16_ref_t a = bitCast<simd128_i16_ref_t>(_a);
		const simd128_i16_ref_t b = bitCast<simd128_i16_ref_t>(_b);
		simd128_i16_ref_t result;
		for (int ii = 0; ii < 8; ++ii) { result.i16[ii] = (int16_t)(a.i16[ii] * b.i16[ii]); }
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i16_cmpeq(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i16_langext_t a      = bitCast<simd128_i16_langext_t>(_a);
		const simd128_i16_langext_t b      = bitCast<simd128_i16_langext_t>(_b);
		const auto                  mask   = a == b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_i16_ref_t a = bitCast<simd128_i16_ref_t>(_a);
		const simd128_i16_ref_t b = bitCast<simd128_i16_ref_t>(_b);
		simd128_u16_ref_t result;
		for (int ii = 0; ii < 8; ++ii) { result.u16[ii] = a.i16[ii] == b.i16[ii] ? uint16_t(0xffff) : uint16_t(0); }
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x16_sll(simd128_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd128_u16_langext_t a       = bitCast<simd128_u16_langext_t>(_a);
		const simd128_u16_langext_t shifted = a << _count;
		const simd128_ref_t         result  = bitCast<simd128_ref_t>(shifted);
		return result;
#else
		const simd128_u16_ref_t a = bitCast<simd128_u16_ref_t>(_a);
		simd128_u16_ref_t result;
		for (int ii = 0; ii < 8; ++ii) { result.u16[ii] = uint16_t(a.u16[ii] << _count); }
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x16_srl(simd128_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd128_u16_langext_t a       = bitCast<simd128_u16_langext_t>(_a);
		const simd128_u16_langext_t shifted = a >> _count;
		const simd128_ref_t         result  = bitCast<simd128_ref_t>(shifted);
		return result;
#else
		const simd128_u16_ref_t a = bitCast<simd128_u16_ref_t>(_a);
		simd128_u16_ref_t result;
		for (int ii = 0; ii < 8; ++ii) { result.u16[ii] = uint16_t(a.u16[ii] >> _count); }
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_splat(int16_t _a)
	{
		const simd128_i16_ref_t result = { { _a, _a, _a, _a, _a, _a, _a, _a } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_splat(uint16_t _a)
	{
		const simd128_u16_ref_t result = { { _a, _a, _a, _a, _a, _a, _a, _a } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i8_add(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i8_langext_t a      = bitCast<simd128_i8_langext_t>(_a);
		const simd128_i8_langext_t b      = bitCast<simd128_i8_langext_t>(_b);
		const simd128_i8_langext_t sum    = a + b;
		const simd128_ref_t        result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		const simd128_i8_ref_t a = bitCast<simd128_i8_ref_t>(_a);
		const simd128_i8_ref_t b = bitCast<simd128_i8_ref_t>(_b);
		simd128_i8_ref_t result;
		for (int ii = 0; ii < 16; ++ii) { result.i8[ii] = a.i8[ii] + b.i8[ii]; }
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i8_sub(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i8_langext_t a      = bitCast<simd128_i8_langext_t>(_a);
		const simd128_i8_langext_t b      = bitCast<simd128_i8_langext_t>(_b);
		const simd128_i8_langext_t diff   = a - b;
		const simd128_ref_t        result = bitCast<simd128_ref_t>(diff);
		return result;
#else
		const simd128_i8_ref_t a = bitCast<simd128_i8_ref_t>(_a);
		const simd128_i8_ref_t b = bitCast<simd128_i8_ref_t>(_b);
		simd128_i8_ref_t result;
		for (int ii = 0; ii < 16; ++ii) { result.i8[ii] = a.i8[ii] - b.i8[ii]; }
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u8_satadd(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_u8_ref_t a = bitCast<simd128_u8_ref_t>(_a);
		const simd128_u8_ref_t b = bitCast<simd128_u8_ref_t>(_b);
		simd128_u8_ref_t result;
		for (int ii = 0; ii < 16; ++ii)
		{
			uint16_t sum = (uint16_t)a.u8[ii] + (uint16_t)b.u8[ii];
			result.u8[ii] = sum > 255 ? 255 : (uint8_t)sum;
		}
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u8_satsub(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_u8_ref_t a = bitCast<simd128_u8_ref_t>(_a);
		const simd128_u8_ref_t b = bitCast<simd128_u8_ref_t>(_b);
		simd128_u8_ref_t result;
		for (int ii = 0; ii < 16; ++ii)
		{
			result.u8[ii] = a.u8[ii] > b.u8[ii] ? (uint8_t)(a.u8[ii] - b.u8[ii]) : 0;
		}
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u16_satadd(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_u16_ref_t a = bitCast<simd128_u16_ref_t>(_a);
		const simd128_u16_ref_t b = bitCast<simd128_u16_ref_t>(_b);
		simd128_u16_ref_t result;
		for (int ii = 0; ii < 8; ++ii)
		{
			uint32_t sum = (uint32_t)a.u16[ii] + (uint32_t)b.u16[ii];
			result.u16[ii] = sum > 65535 ? 65535 : (uint16_t)sum;
		}
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u16_satsub(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_u16_ref_t a = bitCast<simd128_u16_ref_t>(_a);
		const simd128_u16_ref_t b = bitCast<simd128_u16_ref_t>(_b);
		simd128_u16_ref_t result;
		for (int ii = 0; ii < 8; ++ii)
		{
			result.u16[ii] = a.u16[ii] > b.u16[ii] ? (uint16_t)(a.u16[ii] - b.u16[ii]) : 0;
		}
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_and(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const simd128_u32_langext_t masked = a & b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(masked);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] & _b.u32[0];
		result.u32[1] = _a.u32[1] & _b.u32[1];
		result.u32[2] = _a.u32[2] & _b.u32[2];
		result.u32[3] = _a.u32[3] & _b.u32[3];
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_andc(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const simd128_u32_langext_t notb   = ~b;
		const simd128_u32_langext_t masked = a & notb;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(masked);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] & ~_b.u32[0];
		result.u32[1] = _a.u32[1] & ~_b.u32[1];
		result.u32[2] = _a.u32[2] & ~_b.u32[2];
		result.u32[3] = _a.u32[3] & ~_b.u32[3];
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_or(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const simd128_u32_langext_t masked = a | b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(masked);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] | _b.u32[0];
		result.u32[1] = _a.u32[1] | _b.u32[1];
		result.u32[2] = _a.u32[2] | _b.u32[2];
		result.u32[3] = _a.u32[3] | _b.u32[3];
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_xor(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a      = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t b      = bitCast<simd128_u32_langext_t>(_b);
		const simd128_u32_langext_t masked = a ^ b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(masked);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] ^ _b.u32[0];
		result.u32[1] = _a.u32[1] ^ _b.u32[1];
		result.u32[2] = _a.u32[2] ^ _b.u32[2];
		result.u32[3] = _a.u32[3] ^ _b.u32[3];
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_sll(simd128_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a       = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t shifted = a << _count;
		const simd128_ref_t         result  = bitCast<simd128_ref_t>(shifted);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] << _count;
		result.u32[1] = _a.u32[1] << _count;
		result.u32[2] = _a.u32[2] << _count;
		result.u32[3] = _a.u32[3] << _count;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_srl(simd128_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd128_u32_langext_t a       = bitCast<simd128_u32_langext_t>(_a);
		const simd128_u32_langext_t shifted = a >> _count;
		const simd128_ref_t         result  = bitCast<simd128_ref_t>(shifted);
		return result;
#else
		simd128_ref_t result;
		result.u32[0] = _a.u32[0] >> _count;
		result.u32[1] = _a.u32[1] >> _count;
		result.u32[2] = _a.u32[2] >> _count;
		result.u32[3] = _a.u32[3] >> _count;
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_sra(simd128_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd128_i32_langext_t a       = bitCast<simd128_i32_langext_t>(_a);
		const simd128_i32_langext_t shifted = a >> _count;
		const simd128_ref_t         result  = bitCast<simd128_ref_t>(shifted);
		return result;
#else
		const simd128_i32_ref_t a = bitCast<simd128_i32_ref_t>(_a);
		const simd128_i32_ref_t result = { { a.i32[0] >> _count, a.i32[1] >> _count, a.i32[2] >> _count, a.i32[3] >> _count } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_x32_sll(simd128_ref_t _a, simd128_ref_t _count)
	{
		return simd_x32_sll_ni<simd128_ref_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_x32_srl(simd128_ref_t _a, simd128_ref_t _count)
	{
		return simd_x32_srl_ni<simd128_ref_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_x32_sra(simd128_ref_t _a, simd128_ref_t _count)
	{
		return simd_x32_sra_ni<simd128_ref_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_x8_shuffle(simd128_ref_t _a, simd128_ref_t _indices)
	{
		return simd_x8_shuffle_ni<simd128_ref_t>(_a, _indices);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_x8_shuffle(simd128_ref_t _a, simd128_ref_t _b, simd128_ref_t _indices)
	{
		return simd_x8_shuffle_ni<simd128_ref_t>(_a, _b, _indices);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_selb(simd128_ref_t _mask, simd128_ref_t _a, simd128_ref_t _b)
	{
		return simd_selb_ni(_mask, _a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_sels(simd128_ref_t _test, simd128_ref_t _a, simd128_ref_t _b)
	{
		return simd_sels_ni(_test, _a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_not(simd128_ref_t _a)
	{
		return simd_not_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_orc(simd128_ref_t _a, simd128_ref_t _b)
	{
		return simd_orc_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_orx(simd128_ref_t _a)
	{
		return simd128_orx_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_madd(simd128_ref_t _a, simd128_ref_t _b, simd128_ref_t _c)
	{
		return simd_f32_madd_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_msub(simd128_ref_t _a, simd128_ref_t _b, simd128_ref_t _c)
	{
		return simd_f32_msub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_nmsub(simd128_ref_t _a, simd128_ref_t _b, simd128_ref_t _c)
	{
		return simd_f32_nmsub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd128_x32_signbitsmask(simd128_ref_t _a)
	{
		return simd_x32_signbitsmask_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd128_x8_signbitsmask(simd128_ref_t _a)
	{
		return simd_x8_signbitsmask_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_neg(simd128_ref_t _a)
	{
		return simd_f32_neg_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_abs(simd128_ref_t _a)
	{
		return simd_f32_abs_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_clamp(simd128_ref_t _a, simd128_ref_t _min, simd128_ref_t _max)
	{
		return simd_f32_clamp_ni(_a, _min, _max);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_lerp(simd128_ref_t _a, simd128_ref_t _b, simd128_ref_t _s)
	{
		return simd_f32_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_rcp(simd128_ref_t _a)
	{
		return simd_f32_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_sqrt_nr(simd128_ref_t _a)
	{
		return simd_f32_sqrt_nr_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_rsqrt(simd128_ref_t _a)
	{
		return simd_f32_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_rsqrt_nr(simd128_ref_t _a)
	{
		return simd_f32_rsqrt_nr_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_rsqrt_carmack(simd128_ref_t _a)
	{
		return simd_f32_rsqrt_carmack_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_div_nr(simd128_ref_t _a, simd128_ref_t _b)
	{
		return simd_f32_div_nr_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_dot3(simd128_ref_t _a, simd128_ref_t _b)
	{
		return simd128_f32_dot3_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_dot(simd128_ref_t _a, simd128_ref_t _b)
	{
		return simd128_f32_dot_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_cross3(simd128_ref_t _a, simd128_ref_t _b)
	{
		return simd128_f32_cross3_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_normalize3(simd128_ref_t _a)
	{
		return simd128_f32_normalize3_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_ref_t simd128_f32_cmpneq(simd128_ref_t _a, simd128_ref_t _b)
	{
		return simd_f32_cmpneq_ni(_a, _b);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_xAzC(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_ref_t xAyB   = simd128_x32_shuf_xAyB(_a, _b);
		const simd128_ref_t zCwD   = simd128_x32_shuf_zCwD(_a, _b);
		const simd128_ref_t result = simd128_x32_shuf_xyAB(xAyB, zCwD);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_x32_shuf_yBwD(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_ref_t xAyB   = simd128_x32_shuf_xAyB(_a, _b);
		const simd128_ref_t zCwD   = simd128_x32_shuf_zCwD(_a, _b);
		const simd128_ref_t result = simd128_x32_shuf_zwCD(xAyB, zCwD);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_add(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const simd128_f64_langext_t sum    = a + b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_f64_ref_t result = { { a.f64[0] + b.f64[0], a.f64[1] + b.f64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_sub(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const simd128_f64_langext_t diff   = a - b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(diff);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_f64_ref_t result = { { a.f64[0] - b.f64[0], a.f64[1] - b.f64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_mul(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const simd128_f64_langext_t prod   = a * b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(prod);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_f64_ref_t result = { { a.f64[0] * b.f64[0], a.f64[1] * b.f64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_div(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const simd128_f64_langext_t quot   = a / b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(quot);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_f64_ref_t result = { { a.f64[0] / b.f64[0], a.f64[1] / b.f64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_min(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_f64_ref_t result = { { a.f64[0] < b.f64[0] ? a.f64[0] : b.f64[0], a.f64[1] < b.f64[1] ? a.f64[1] : b.f64[1] } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_max(simd128_ref_t _a, simd128_ref_t _b)
	{
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_f64_ref_t result = { { a.f64[0] > b.f64[0] ? a.f64[0] : b.f64[0], a.f64[1] > b.f64[1] ? a.f64[1] : b.f64[1] } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_madd(simd128_ref_t _a, simd128_ref_t _b, simd128_ref_t _c)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const simd128_f64_langext_t c      = bitCast<simd128_f64_langext_t>(_c);
		const simd128_f64_langext_t prod   = a * b;
		const simd128_f64_langext_t sum    = prod + c;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_f64_ref_t c = bitCast<simd128_f64_ref_t>(_c);
		const simd128_f64_ref_t result = { { a.f64[0] * b.f64[0] + c.f64[0], a.f64[1] * b.f64[1] + c.f64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_nmsub(simd128_ref_t _a, simd128_ref_t _b, simd128_ref_t _c)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const simd128_f64_langext_t c      = bitCast<simd128_f64_langext_t>(_c);
		const simd128_f64_langext_t prod   = a * b;
		const simd128_f64_langext_t diff   = c - prod;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(diff);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_f64_ref_t c = bitCast<simd128_f64_ref_t>(_c);
		const simd128_f64_ref_t result = { { c.f64[0] - a.f64[0] * b.f64[0], c.f64[1] - a.f64[1] * b.f64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_neg(simd128_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t neg    = -a;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(neg);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t result = { { -a.f64[0], -a.f64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_abs(simd128_ref_t _a)
	{
		const simd128_ref_t a_neg  = simd128_f64_neg(_a);
		const simd128_ref_t result = simd128_f64_max(a_neg, _a);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_clamp(simd128_ref_t _a, simd128_ref_t _min, simd128_ref_t _max)
	{
		const simd128_ref_t hi     = simd128_f64_max(_a, _min);
		const simd128_ref_t result = simd128_f64_min(hi, _max);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_lerp(simd128_ref_t _a, simd128_ref_t _b, simd128_ref_t _s)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const simd128_f64_langext_t s      = bitCast<simd128_f64_langext_t>(_s);
		const simd128_f64_langext_t diff   = b - a;
		const simd128_f64_langext_t scaled = diff * s;
		const simd128_f64_langext_t sum    = a + scaled;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_f64_ref_t s = bitCast<simd128_f64_ref_t>(_s);
		const simd128_f64_ref_t result = { { a.f64[0] + (b.f64[0] - a.f64[0]) * s.f64[0], a.f64[1] + (b.f64[1] - a.f64[1]) * s.f64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_rcp(simd128_ref_t _a)
	{
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t result = { { 1.0 / a.f64[0], 1.0 / a.f64[1] } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_sqrt(simd128_ref_t _a)
	{
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t result = { { simd_sqrt(a.f64[0]), simd_sqrt(a.f64[1]) } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_rsqrt(simd128_ref_t _a)
	{
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t result = { { 1.0 / simd_sqrt(a.f64[0]), 1.0 / simd_sqrt(a.f64[1]) } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_round(simd128_ref_t _a)
	{
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t result = { { simd_round(a.f64[0]), simd_round(a.f64[1]) } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_ceil(simd128_ref_t _a)
	{
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t result = { { simd_ceil(a.f64[0]), simd_ceil(a.f64[1]) } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_floor(simd128_ref_t _a)
	{
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t result = { { simd_floor(a.f64[0]), simd_floor(a.f64[1]) } };
		return bitCast<simd128_ref_t>(result);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_cmpeq(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const auto                  mask   = a == b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_u64_ref_t result = { { a.f64[0] == b.f64[0] ? UINT64_MAX : 0, a.f64[1] == b.f64[1] ? UINT64_MAX : 0 } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_cmpneq(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const auto                  mask   = a != b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_u64_ref_t result = { { a.f64[0] != b.f64[0] ? UINT64_MAX : 0, a.f64[1] != b.f64[1] ? UINT64_MAX : 0 } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_cmplt(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const auto                  mask   = a < b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_u64_ref_t result = { { a.f64[0] < b.f64[0] ? UINT64_MAX : 0, a.f64[1] < b.f64[1] ? UINT64_MAX : 0 } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_cmple(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const auto                  mask   = a <= b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_u64_ref_t result = { { a.f64[0] <= b.f64[0] ? UINT64_MAX : 0, a.f64[1] <= b.f64[1] ? UINT64_MAX : 0 } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_cmpgt(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const auto                  mask   = a > b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_u64_ref_t result = { { a.f64[0] > b.f64[0] ? UINT64_MAX : 0, a.f64[1] > b.f64[1] ? UINT64_MAX : 0 } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_f64_cmpge(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_f64_langext_t a      = bitCast<simd128_f64_langext_t>(_a);
		const simd128_f64_langext_t b      = bitCast<simd128_f64_langext_t>(_b);
		const auto                  mask   = a >= b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(mask);
		return result;
#else
		const simd128_f64_ref_t a = bitCast<simd128_f64_ref_t>(_a);
		const simd128_f64_ref_t b = bitCast<simd128_f64_ref_t>(_b);
		const simd128_u64_ref_t result = { { a.f64[0] >= b.f64[0] ? UINT64_MAX : 0, a.f64[1] >= b.f64[1] ? UINT64_MAX : 0 } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i64_add(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i64_langext_t a      = bitCast<simd128_i64_langext_t>(_a);
		const simd128_i64_langext_t b      = bitCast<simd128_i64_langext_t>(_b);
		const simd128_i64_langext_t sum    = a + b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		const simd128_i64_ref_t a = bitCast<simd128_i64_ref_t>(_a);
		const simd128_i64_ref_t b = bitCast<simd128_i64_ref_t>(_b);
		const simd128_i64_ref_t result = { { a.i64[0] + b.i64[0], a.i64[1] + b.i64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_i64_sub(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_i64_langext_t a      = bitCast<simd128_i64_langext_t>(_a);
		const simd128_i64_langext_t b      = bitCast<simd128_i64_langext_t>(_b);
		const simd128_i64_langext_t diff   = a - b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(diff);
		return result;
#else
		const simd128_i64_ref_t a = bitCast<simd128_i64_ref_t>(_a);
		const simd128_i64_ref_t b = bitCast<simd128_i64_ref_t>(_b);
		const simd128_i64_ref_t result = { { a.i64[0] - b.i64[0], a.i64[1] - b.i64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u64_add(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u64_langext_t a      = bitCast<simd128_u64_langext_t>(_a);
		const simd128_u64_langext_t b      = bitCast<simd128_u64_langext_t>(_b);
		const simd128_u64_langext_t sum    = a + b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(sum);
		return result;
#else
		const simd128_u64_ref_t a = bitCast<simd128_u64_ref_t>(_a);
		const simd128_u64_ref_t b = bitCast<simd128_u64_ref_t>(_b);
		const simd128_u64_ref_t result = { { a.u64[0] + b.u64[0], a.u64[1] + b.u64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd128_ref_t simd128_u64_sub(simd128_ref_t _a, simd128_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd128_u64_langext_t a      = bitCast<simd128_u64_langext_t>(_a);
		const simd128_u64_langext_t b      = bitCast<simd128_u64_langext_t>(_b);
		const simd128_u64_langext_t diff   = a - b;
		const simd128_ref_t         result = bitCast<simd128_ref_t>(diff);
		return result;
#else
		const simd128_u64_ref_t a = bitCast<simd128_u64_ref_t>(_a);
		const simd128_u64_ref_t b = bitCast<simd128_u64_ref_t>(_b);
		const simd128_u64_ref_t result = { { a.u64[0] - b.u64[0], a.u64[1] - b.u64[1] } };
		return bitCast<simd128_ref_t>(result);
#endif // BX_SIMD_LANGEXT
	}

} // namespace bx
