/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#	error "Must be included from bx/simd_t.h!"
#endif // BX_SIMD_T_H_HEADER_GUARD

namespace bx
{

#define ELEMx 0
#define ELEMy 1
#define ELEMz 2
#define ELEMw 3
#define BX_SIMD128_IMPLEMENT_SWIZZLE(_x, _y, _z, _w)                                             \
			template<>                                                                           \
			BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_swiz_##_x##_y##_z##_w(simd128_sse_t _a)   \
			{                                                                                    \
				return _mm_shuffle_ps(_a, _a, _MM_SHUFFLE(ELEM##_w, ELEM##_z, ELEM##_y, ELEM##_x)); \
			}

#include "simd128_swizzle.inl"

#undef BX_SIMD128_IMPLEMENT_SWIZZLE
#undef ELEMw
#undef ELEMz
#undef ELEMy
#undef ELEMx

#define BX_SIMD128_IMPLEMENT_TEST(_xyzw, _mask)                                    \
			template<>                                                             \
			BX_SIMD_FORCE_INLINE bool simd128_test_any_##_xyzw(simd128_sse_t _test)\
			{                                                                      \
				return 0x0 != (_mm_movemask_ps(_test)&(_mask) );                   \
			}                                                                      \
			                                                                       \
			template<>                                                             \
			BX_SIMD_FORCE_INLINE bool simd128_test_all_##_xyzw(simd128_sse_t _test)\
			{                                                                      \
				return (_mask) == (_mm_movemask_ps(_test)&(_mask) );               \
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
	BX_SIMD_FORCE_INLINE bool simd128_test_zero(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i ai = _mm_castps_si128(_a);
		const __m128i bi = _mm_castps_si128(_b);
		return 0 != _mm_testz_si128(ai, bi);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_xyAB(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_movelh_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_ABxy(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_movelh_ps(_b, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_CDzw(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_movehl_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_zwCD(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_movehl_ps(_b, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_xAyB(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_unpacklo_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_AxBy(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_unpacklo_ps(_b, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_zCwD(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_unpackhi_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_CzDw(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_unpackhi_ps(_b, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_xzAC(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_shuffle_ps(_a, _b, _MM_SHUFFLE(2, 0, 2, 0) );
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_ywBD(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_shuffle_ps(_a, _b, _MM_SHUFFLE(3, 1, 3, 1) );
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_xxAA(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_shuffle_ps(_a, _b, _MM_SHUFFLE(0, 0, 0, 0) );
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_yyBB(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_shuffle_ps(_a, _b, _MM_SHUFFLE(1, 1, 1, 1) );
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_zzCC(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_shuffle_ps(_a, _b, _MM_SHUFFLE(2, 2, 2, 2) );
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_wwDD(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_shuffle_ps(_a, _b, _MM_SHUFFLE(3, 3, 3, 3) );
	}

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_x(simd128_sse_t _a)
	{
		return _mm_cvtss_f32(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_y(simd128_sse_t _a)
	{
		const simd128_sse_t tmp = simd128_x32_swiz_yyyy(_a);
		return _mm_cvtss_f32(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_z(simd128_sse_t _a)
	{
		const simd128_sse_t tmp = simd128_x32_swiz_zzzz(_a);
		return _mm_cvtss_f32(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_w(simd128_sse_t _a)
	{
		const simd128_sse_t tmp = simd128_x32_swiz_wwww(_a);
		return _mm_cvtss_f32(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_ld(const void* _ptr)
	{
		return _mm_load_ps(reinterpret_cast<const float*>(_ptr) );
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_ldu(const void* _ptr)
	{
		return _mm_loadu_ps(reinterpret_cast<const float*>(_ptr) );
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_st(void* _ptr, simd128_sse_t _a)
	{
		_mm_store_ps(reinterpret_cast<float*>(_ptr), _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_stu(void* _ptr, simd128_sse_t _a)
	{
		_mm_storeu_ps(reinterpret_cast<float*>(_ptr), _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_x32_st1(void* _ptr, simd128_sse_t _a)
	{
		_mm_store_ss(reinterpret_cast<float*>(_ptr), _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_stream(void* _ptr, simd128_sse_t _a)
	{
		_mm_stream_ps(reinterpret_cast<float*>(_ptr), _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_ld(float _x, float _y, float _z, float _w)
	{
		return _mm_set_ps(_w, _z, _y, _x);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_ld(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w)
	{
		const __m128i tmp = _mm_set_epi32(int32_t(_w), int32_t(_z), int32_t(_y), int32_t(_x));
		return _mm_castsi128_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_ld(int32_t _x, int32_t _y, int32_t _z, int32_t _w)
	{
		const __m128i tmp = _mm_set_epi32(_w, _z, _y, _x);
		return _mm_castsi128_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_splat(float _a)
	{
		return _mm_set1_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_splat(int32_t _a)
	{
		const __m128i tmp = _mm_set1_epi32(_a);
		return _mm_castsi128_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_splat(uint32_t _a)
	{
		const __m128i tmp = _mm_set1_epi32(int32_t(_a));
		return _mm_castsi128_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_splat(double _a)
	{
		const __m128d tmp = _mm_set1_pd(_a);
		return _mm_castpd_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_zero()
	{
		return _mm_setzero_ps();
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_itof(simd128_sse_t _a)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128  result = _mm_cvtepi32_ps(a);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_ftoi_trunc(simd128_sse_t _a)
	{
		const __m128i result = _mm_cvttps_epi32(_a);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_ftoi_round(simd128_sse_t _a)
	{
		const __m128i result = _mm_cvtps_epi32(_a);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_round(simd128_sse_t _a)
	{
		return _mm_round_ps(_a, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_ceil(simd128_sse_t _a)
	{
		return _mm_ceil_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_floor(simd128_sse_t _a)
	{
		return _mm_floor_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_add(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_add_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_sub(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_sub_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_mul(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_mul_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_div(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_div_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_rcp_est(simd128_sse_t _a)
	{
		return _mm_rcp_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_sqrt(simd128_sse_t _a)
	{
		return _mm_sqrt_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_rsqrt_est(simd128_sse_t _a)
	{
		return _mm_rsqrt_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_min(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_min_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_max(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_max_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_dot3(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_dp_ps(_a, _b, 0x77);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_dot(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_dp_ps(_a, _b, 0xff);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_cmpeq(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_cmpeq_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_cmplt(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_cmplt_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_cmple(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_cmple_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_cmpgt(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_cmpgt_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_cmpge(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_cmpge_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_add(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a   = _mm_castps_si128(_a);
		const __m128i b   = _mm_castps_si128(_b);
		const __m128i add = _mm_add_epi32(a, b);
		return _mm_castsi128_ps(add);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_sub(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a   = _mm_castps_si128(_a);
		const __m128i b   = _mm_castps_si128(_b);
		const __m128i sub = _mm_sub_epi32(a, b);
		return _mm_castsi128_ps(sub);
	}
	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_neg(simd128_sse_t _a)
	{
		return simd_i32_neg_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_abs(simd128_sse_t _a)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i result = _mm_abs_epi32(a);
		return _mm_castsi128_ps(result);
	}
	// SSE4.1 â€” always available with SSE4.2 minspec
	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_min(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_min_epi32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_max(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_max_epi32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_cmpeq(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_cmpeq_epi32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_cmplt(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_cmplt_epi32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_cmpgt(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_cmpgt_epi32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i32_clamp(simd128_sse_t _a, simd128_sse_t _min, simd128_sse_t _max)
	{
		const simd128_sse_t lo     = simd128_i32_max(_a, _min);
		const simd128_sse_t result = simd128_i32_min(lo, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u32_add(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_add_epi32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u32_sub(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_sub_epi32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u32_mul(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_mullo_epi32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u32_min(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_min_epu32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u32_max(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_max_epu32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u32_clamp(simd128_sse_t _a, simd128_sse_t _min, simd128_sse_t _max)
	{
		const simd128_sse_t lo     = simd128_u32_max(_a, _min);
		const simd128_sse_t result = simd128_u32_min(lo, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u32_cmpeq(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_cmpeq_epi32(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u32_cmplt(simd128_sse_t _a, simd128_sse_t _b)
	{
		return simd_u32_cmplt_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u32_cmpgt(simd128_sse_t _a, simd128_sse_t _b)
	{
		return simd_u32_cmpgt_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i16_add(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_add_epi16(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i16_sub(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_sub_epi16(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i16_mullo(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_mullo_epi16(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i16_cmpeq(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_cmpeq_epi16(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x16_sll(simd128_sse_t _a, int _count)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i result = _mm_slli_epi16(a, _count);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x16_srl(simd128_sse_t _a, int _count)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i result = _mm_srli_epi16(a, _count);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_splat(int16_t _a)
	{
		const __m128i tmp = _mm_set1_epi16(_a);
		return _mm_castsi128_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_splat(uint16_t _a)
	{
		const __m128i tmp = _mm_set1_epi16(int16_t(_a));
		return _mm_castsi128_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i8_add(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_add_epi8(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i8_sub(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_sub_epi8(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u8_satadd(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_adds_epu8(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u8_satsub(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_subs_epu8(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u16_satadd(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_adds_epu16(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u16_satsub(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_subs_epu16(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_and(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_and_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_andc(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_andnot_ps(_b, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_or(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_or_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_xor(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_xor_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_sll(simd128_sse_t _a, int _count)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i result = _mm_slli_epi32(a, _count);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_srl(simd128_sse_t _a, int _count)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i result = _mm_srli_epi32(a, _count);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_sra(simd128_sse_t _a, int _count)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i result = _mm_srai_epi32(a, _count);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_sll(simd128_sse_t _a, simd128_sse_t _count)
	{
#if BX_SIMD_AVX2
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i c      = _mm_castps_si128(_count);
		const __m128i result = _mm_sllv_epi32(a, c);
		return _mm_castsi128_ps(result);
#else
		return simd_x32_sll_ni<simd128_sse_t>(_a, _count);
#endif // BX_SIMD_AVX2
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_srl(simd128_sse_t _a, simd128_sse_t _count)
	{
#if BX_SIMD_AVX2
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i c      = _mm_castps_si128(_count);
		const __m128i result = _mm_srlv_epi32(a, c);
		return _mm_castsi128_ps(result);
#else
		return simd_x32_srl_ni<simd128_sse_t>(_a, _count);
#endif // BX_SIMD_AVX2
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_sra(simd128_sse_t _a, simd128_sse_t _count)
	{
		// Note: _mm_srav_epi32 is AVX-512VL, not AVX2. Fall back to _ni decomposition.
		return simd_x32_sra_ni<simd128_sse_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_selb(simd128_sse_t _mask, simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_blendv_ps(_b, _a, _mask);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_sels(simd128_sse_t _test, simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_blendv_ps(_b, _a, _test);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x8_shuffle(simd128_sse_t _a, simd128_sse_t _indices)
	{
		const __m128i a       = _mm_castps_si128(_a);
		const __m128i indices = _mm_castps_si128(_indices);
		const __m128i result  = _mm_shuffle_epi8(a, indices);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x8_shuffle(simd128_sse_t _a, simd128_sse_t _b, simd128_sse_t _indices)
	{
		const __m128i a       = _mm_castps_si128(_a);
		const __m128i b       = _mm_castps_si128(_b);
		const __m128i indices = _mm_castps_si128(_indices);
		// PSHUFB ignores bits 4..6 of the index byte; only bits 0..3 select and
		// bit 7 zeroes. Shuffle from a and b independently, then blend per-byte
		// based on bit 4 of the original index.
		const __m128i pa      = _mm_shuffle_epi8(a, indices);
		const __m128i pb      = _mm_shuffle_epi8(b, indices);
		const __m128i bit4    = _mm_set1_epi8(0x10);
		const __m128i masked  = _mm_and_si128(indices, bit4);
		// Convert "bit4 set" into a per-byte high-bit mask for blendv_epi8.
		const __m128i pickb   = _mm_cmpeq_epi8(masked, bit4);
		const __m128i result  = _mm_blendv_epi8(pa, pb, pickb);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_not(simd128_sse_t _a)
	{
		return simd_not_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_orc(simd128_sse_t _a, simd128_sse_t _b)
	{
		return simd_orc_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_orx(simd128_sse_t _a)
	{
		return simd128_orx_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_madd(simd128_sse_t _a, simd128_sse_t _b, simd128_sse_t _c)
	{
		return simd_f32_madd_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_msub(simd128_sse_t _a, simd128_sse_t _b, simd128_sse_t _c)
	{
		return simd_f32_msub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_nmsub(simd128_sse_t _a, simd128_sse_t _b, simd128_sse_t _c)
	{
		return simd_f32_nmsub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd128_x32_signbitsmask(simd128_sse_t _a)
	{
		return _mm_movemask_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd128_x8_signbitsmask(simd128_sse_t _a)
	{
		const __m128i ai     = _mm_castps_si128(_a);
		const int     result = _mm_movemask_epi8(ai);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_neg(simd128_sse_t _a)
	{
		const __m128i signi = _mm_set1_epi32(int32_t(kFloatSignMask) );
		const __m128  signf = _mm_castsi128_ps(signi);
		return _mm_xor_ps(_a, signf);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_abs(simd128_sse_t _a)
	{
		const __m128i maski = _mm_set1_epi32(int32_t(kFloatExponentMask | kFloatMantissaMask) );
		const __m128  maskf = _mm_castsi128_ps(maski);
		return _mm_and_ps(_a, maskf);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_clamp(simd128_sse_t _a, simd128_sse_t _min, simd128_sse_t _max)
	{
		const simd128_sse_t lo     = _mm_max_ps(_a, _min);
		const simd128_sse_t result = _mm_min_ps(lo, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_lerp(simd128_sse_t _a, simd128_sse_t _b, simd128_sse_t _s)
	{
		return simd_f32_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_rcp(simd128_sse_t _a)
	{
		return simd_f32_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_sqrt_nr(simd128_sse_t _a)
	{
		return simd_f32_sqrt_nr_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_rsqrt(simd128_sse_t _a)
	{
		return simd_f32_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_rsqrt_nr(simd128_sse_t _a)
	{
		return simd_f32_rsqrt_nr_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_rsqrt_carmack(simd128_sse_t _a)
	{
		return simd_f32_rsqrt_carmack_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_div_nr(simd128_sse_t _a, simd128_sse_t _b)
	{
		return simd_f32_div_nr_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_cross3(simd128_sse_t _a, simd128_sse_t _b)
	{
		return simd128_f32_cross3_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_normalize3(simd128_sse_t _a)
	{
		return simd128_f32_normalize3_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f32_cmpneq(simd128_sse_t _a, simd128_sse_t _b)
	{
		return _mm_cmpneq_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_xAzC(simd128_sse_t _a, simd128_sse_t _b)
	{
		const simd128_sse_t lo     = _mm_unpacklo_ps(_a, _b);
		const simd128_sse_t hi     = _mm_unpackhi_ps(_a, _b);
		const simd128_sse_t result = _mm_movelh_ps(lo, hi);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_x32_shuf_yBwD(simd128_sse_t _a, simd128_sse_t _b)
	{
		const simd128_sse_t lo     = _mm_unpacklo_ps(_a, _b);
		const simd128_sse_t hi     = _mm_unpackhi_ps(_a, _b);
		const simd128_sse_t result = _mm_movehl_ps(hi, lo);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_add(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_add_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_sub(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_sub_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_mul(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_mul_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_div(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_div_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_min(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_min_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_max(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_max_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_madd(simd128_sse_t _a, simd128_sse_t _b, simd128_sse_t _c)
	{
		return simd_f64_madd_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_nmsub(simd128_sse_t _a, simd128_sse_t _b, simd128_sse_t _c)
	{
		return simd_f64_nmsub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_neg(simd128_sse_t _a)
	{
		const __m128i signi = _mm_set1_epi64x(int64_t(kDoubleSignMask) );
		const __m128d signd = _mm_castsi128_pd(signi);
		const __m128d a     = _mm_castps_pd(_a);
		const __m128d r     = _mm_xor_pd(a, signd);
		return _mm_castpd_ps(r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_abs(simd128_sse_t _a)
	{
		const __m128i maski = _mm_set1_epi64x(int64_t(kDoubleExponentMask | kDoubleMantissaMask) );
		const __m128d maskd = _mm_castsi128_pd(maski);
		const __m128d a     = _mm_castps_pd(_a);
		const __m128d r     = _mm_and_pd(a, maskd);
		return _mm_castpd_ps(r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_clamp(simd128_sse_t _a, simd128_sse_t _min, simd128_sse_t _max)
	{
		const simd128_sse_t lo     = simd128_f64_max(_a, _min);
		const simd128_sse_t result = simd128_f64_min(lo, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_lerp(simd128_sse_t _a, simd128_sse_t _b, simd128_sse_t _s)
	{
		return simd_f64_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_rcp(simd128_sse_t _a)
	{
		return simd_f64_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_sqrt(simd128_sse_t _a)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d result = _mm_sqrt_pd(a);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_rsqrt(simd128_sse_t _a)
	{
		return simd_f64_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_round(simd128_sse_t _a)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d result = _mm_round_pd(a, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_ceil(simd128_sse_t _a)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d result = _mm_ceil_pd(a);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_floor(simd128_sse_t _a)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d result = _mm_floor_pd(a);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_cmpeq(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_cmpeq_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_cmpneq(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_cmpneq_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_cmplt(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_cmplt_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_cmple(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_cmple_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_cmpgt(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_cmpgt_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_f64_cmpge(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128d a      = _mm_castps_pd(_a);
		const __m128d b      = _mm_castps_pd(_b);
		const __m128d result = _mm_cmpge_pd(a, b);
		return _mm_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i64_add(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_add_epi64(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_i64_sub(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_sub_epi64(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u64_add(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_add_epi64(a, b);
		return _mm_castsi128_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_sse_t simd128_u64_sub(simd128_sse_t _a, simd128_sse_t _b)
	{
		const __m128i a      = _mm_castps_si128(_a);
		const __m128i b      = _mm_castps_si128(_b);
		const __m128i result = _mm_sub_epi64(a, b);
		return _mm_castsi128_ps(result);
	}

} // namespace bx
