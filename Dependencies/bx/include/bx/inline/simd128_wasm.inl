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
#define BX_SIMD128_IMPLEMENT_SWIZZLE(_x, _y, _z, _w)                                                \
			template<>                                                                              \
			BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_swiz_##_x##_y##_z##_w(simd128_wasm_t _a) \
			{                                                                                       \
				return wasm_i32x4_shuffle(_a, _a, ELEM##_x, ELEM##_y, ELEM##_z, ELEM##_w);         \
			}

#include "simd128_swizzle.inl"

#undef BX_SIMD128_IMPLEMENT_SWIZZLE
#undef ELEMw
#undef ELEMz
#undef ELEMy
#undef ELEMx

#define BX_SIMD128_IMPLEMENT_TEST(_xyzw, _mask)                                      \
			template<>                                                               \
			BX_SIMD_FORCE_INLINE bool simd128_test_any_##_xyzw(simd128_wasm_t _test) \
			{                                                                        \
				return 0x0 != (wasm_i32x4_bitmask(_test)&(_mask) );                  \
			}                                                                        \
			                                                                         \
			template<>                                                               \
			BX_SIMD_FORCE_INLINE bool simd128_test_all_##_xyzw(simd128_wasm_t _test) \
			{                                                                        \
				return (_mask) == (wasm_i32x4_bitmask(_test)&(_mask) );              \
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
	BX_SIMD_FORCE_INLINE bool simd128_test_zero(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		const simd128_wasm_t masked = wasm_v128_and(_a, _b);
		const bool           result = !wasm_v128_any_true(masked);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_xyAB(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 0, 1, 4, 5);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_ABxy(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 4, 5, 0, 1);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_CDzw(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 6, 7, 2, 3);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_zwCD(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 2, 3, 6, 7);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_xAyB(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 0, 4, 1, 5);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_AxBy(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 4, 0, 5, 1);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_zCwD(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 2, 6, 3, 7);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_CzDw(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 6, 2, 7, 3);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_xzAC(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 0, 2, 4, 6);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_ywBD(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 1, 3, 5, 7);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_xxAA(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 0, 0, 4, 4);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_yyBB(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 1, 1, 5, 5);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_zzCC(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 2, 2, 6, 6);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_wwDD(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 3, 3, 7, 7);
	}

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_x(simd128_wasm_t _a)
	{
		return wasm_f32x4_extract_lane(_a, 0);
	}

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_y(simd128_wasm_t _a)
	{
		return wasm_f32x4_extract_lane(_a, 1);
	}

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_z(simd128_wasm_t _a)
	{
		return wasm_f32x4_extract_lane(_a, 2);
	}

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_w(simd128_wasm_t _a)
	{
		return wasm_f32x4_extract_lane(_a, 3);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_ld(const void* _ptr)
	{
		return wasm_v128_load(_ptr);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_ldu(const void* _ptr)
	{
		return wasm_v128_load(_ptr);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_st(void* _ptr, simd128_wasm_t _a)
	{
		wasm_v128_store(_ptr, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_stu(void* _ptr, simd128_wasm_t _a)
	{
		wasm_v128_store(_ptr, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_x32_st1(void* _ptr, simd128_wasm_t _a)
	{
		float* result = reinterpret_cast<float*>(_ptr);
		*result = wasm_f32x4_extract_lane(_a, 0);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_stream(void* _ptr, simd128_wasm_t _a)
	{
		wasm_v128_store(_ptr, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_ld(float _x, float _y, float _z, float _w)
	{
		return wasm_f32x4_make(_x, _y, _z, _w);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_ld(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w)
	{
		return wasm_i32x4_make(int32_t(_x), int32_t(_y), int32_t(_z), int32_t(_w));
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_ld(int32_t _x, int32_t _y, int32_t _z, int32_t _w)
	{
		return wasm_i32x4_make(_x, _y, _z, _w);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_splat(float _a)
	{
		return wasm_f32x4_splat(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_splat(int32_t _a)
	{
		return wasm_i32x4_splat(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_splat(uint32_t _a)
	{
		return wasm_i32x4_splat(int32_t(_a));
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_splat(double _a)
	{
		return wasm_f64x2_splat(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_zero()
	{
		return wasm_i32x4_splat(0);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_itof(simd128_wasm_t _a)
	{
		return wasm_f32x4_convert_i32x4(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_ftoi_trunc(simd128_wasm_t _a)
	{
		return wasm_i32x4_trunc_sat_f32x4(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_ftoi_round(simd128_wasm_t _a)
	{
		const simd128_wasm_t rounded = wasm_f32x4_nearest(_a);
		const simd128_wasm_t result  = wasm_i32x4_trunc_sat_f32x4(rounded);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_round(simd128_wasm_t _a)
	{
		return wasm_f32x4_nearest(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_ceil(simd128_wasm_t _a)
	{
		return wasm_f32x4_ceil(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_floor(simd128_wasm_t _a)
	{
		return wasm_f32x4_floor(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_add(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_add(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_sub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_sub(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_mul(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_mul(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_div(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_div(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_rcp_est(simd128_wasm_t _a)
	{
		const simd128_wasm_t one    = wasm_f32x4_splat(1.0f);
		const simd128_wasm_t result = wasm_f32x4_div(one, _a);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_sqrt(simd128_wasm_t _a)
	{
		return wasm_f32x4_sqrt(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_rsqrt_est(simd128_wasm_t _a)
	{
		const simd128_wasm_t one    = wasm_f32x4_splat(1.0f);
		const simd128_wasm_t sqrt   = wasm_f32x4_sqrt(_a);
		const simd128_wasm_t result = wasm_f32x4_div(one, sqrt);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_min(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_pmin(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_max(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_pmax(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_dot3(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return simd128_f32_dot3_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_dot(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return simd128_f32_dot_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_cmpeq(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_eq(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_cmplt(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_lt(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_cmple(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_le(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_cmpgt(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_gt(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_cmpge(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_ge(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_add(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_add(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_sub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_sub(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_neg(simd128_wasm_t _a)
	{
		return wasm_i32x4_neg(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_abs(simd128_wasm_t _a)
	{
		return wasm_i32x4_abs(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_min(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_min(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_max(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_max(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_cmpeq(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_eq(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_cmplt(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_lt(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_cmpgt(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_gt(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i32_clamp(simd128_wasm_t _a, simd128_wasm_t _min, simd128_wasm_t _max)
	{
		const simd128_wasm_t lo     = wasm_i32x4_max(_a, _min);
		const simd128_wasm_t result = wasm_i32x4_min(lo, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u32_add(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_add(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u32_sub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_sub(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u32_mul(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_mul(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u32_min(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_u32x4_min(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u32_max(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_u32x4_max(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u32_clamp(simd128_wasm_t _a, simd128_wasm_t _min, simd128_wasm_t _max)
	{
		const simd128_wasm_t lo     = wasm_u32x4_max(_a, _min);
		const simd128_wasm_t result = wasm_u32x4_min(lo, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u32_cmpeq(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_eq(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u32_cmplt(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_u32x4_lt(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u32_cmpgt(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_u32x4_gt(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i16_add(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i16x8_add(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i16_sub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i16x8_sub(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i16_mullo(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i16x8_mul(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i16_cmpeq(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i16x8_eq(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x16_sll(simd128_wasm_t _a, int _count)
	{
		return wasm_i16x8_shl(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x16_srl(simd128_wasm_t _a, int _count)
	{
		return wasm_u16x8_shr(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_splat(int16_t _a)
	{
		return wasm_i16x8_splat(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_splat(uint16_t _a)
	{
		return wasm_i16x8_splat(int16_t(_a));
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i8_add(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i8x16_add(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i8_sub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i8x16_sub(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u8_satadd(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_u8x16_add_sat(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u8_satsub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_u8x16_sub_sat(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u16_satadd(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_u16x8_add_sat(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u16_satsub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_u16x8_sub_sat(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_and(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_v128_and(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_andc(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_v128_andnot(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_or(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_v128_or(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_xor(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_v128_xor(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_sll(simd128_wasm_t _a, int _count)
	{
		return wasm_i32x4_shl(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_srl(simd128_wasm_t _a, int _count)
	{
		return wasm_u32x4_shr(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_sra(simd128_wasm_t _a, int _count)
	{
		return wasm_i32x4_shr(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_sll(simd128_wasm_t _a, simd128_wasm_t _count)
	{
		return simd_x32_sll_ni<simd128_wasm_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_srl(simd128_wasm_t _a, simd128_wasm_t _count)
	{
		return simd_x32_srl_ni<simd128_wasm_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_sra(simd128_wasm_t _a, simd128_wasm_t _count)
	{
		return simd_x32_sra_ni<simd128_wasm_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x8_shuffle(simd128_wasm_t _a, simd128_wasm_t _indices)
	{
		return wasm_i8x16_swizzle(_a, _indices);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x8_shuffle(simd128_wasm_t _a, simd128_wasm_t _b, simd128_wasm_t _indices)
	{
		return simd_x8_shuffle_ni<simd128_wasm_t>(_a, _b, _indices);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_selb(simd128_wasm_t _mask, simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_v128_bitselect(_a, _b, _mask);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_sels(simd128_wasm_t _test, simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_v128_bitselect(_a, _b, _test);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_not(simd128_wasm_t _a)
	{
		return wasm_v128_not(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_orc(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		const simd128_wasm_t notb   = wasm_v128_not(_b);
		const simd128_wasm_t result = wasm_v128_or(_a, notb);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_orx(simd128_wasm_t _a)
	{
		return simd128_orx_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_madd(simd128_wasm_t _a, simd128_wasm_t _b, simd128_wasm_t _c)
	{
		return simd_f32_madd_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_msub(simd128_wasm_t _a, simd128_wasm_t _b, simd128_wasm_t _c)
	{
		return simd_f32_msub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_nmsub(simd128_wasm_t _a, simd128_wasm_t _b, simd128_wasm_t _c)
	{
		return simd_f32_nmsub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd128_x32_signbitsmask(simd128_wasm_t _a)
	{
		return wasm_i32x4_bitmask(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd128_x8_signbitsmask(simd128_wasm_t _a)
	{
		return wasm_i8x16_bitmask(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_neg(simd128_wasm_t _a)
	{
		return wasm_f32x4_neg(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_abs(simd128_wasm_t _a)
	{
		return wasm_f32x4_abs(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_clamp(simd128_wasm_t _a, simd128_wasm_t _min, simd128_wasm_t _max)
	{
		const simd128_wasm_t lo     = wasm_f32x4_pmax(_a, _min);
		const simd128_wasm_t result = wasm_f32x4_pmin(lo, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_lerp(simd128_wasm_t _a, simd128_wasm_t _b, simd128_wasm_t _s)
	{
		return simd_f32_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_rcp(simd128_wasm_t _a)
	{
		return simd_f32_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_sqrt_nr(simd128_wasm_t _a)
	{
		return simd_f32_sqrt_nr_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_rsqrt(simd128_wasm_t _a)
	{
		return simd_f32_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_rsqrt_nr(simd128_wasm_t _a)
	{
		return simd_f32_rsqrt_nr_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_rsqrt_carmack(simd128_wasm_t _a)
	{
		return simd_f32_rsqrt_carmack_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_div_nr(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return simd_f32_div_nr_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_cross3(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return simd128_f32_cross3_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_normalize3(simd128_wasm_t _a)
	{
		return simd128_f32_normalize3_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f32_cmpneq(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f32x4_ne(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_xAzC(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 0, 4, 2, 6);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_x32_shuf_yBwD(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i32x4_shuffle(_a, _b, 1, 5, 3, 7);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_add(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_add(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_sub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_sub(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_mul(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_mul(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_div(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_div(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_min(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_pmin(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_max(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_pmax(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_madd(simd128_wasm_t _a, simd128_wasm_t _b, simd128_wasm_t _c)
	{
		return simd_f64_madd_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_nmsub(simd128_wasm_t _a, simd128_wasm_t _b, simd128_wasm_t _c)
	{
		return simd_f64_nmsub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_neg(simd128_wasm_t _a)
	{
		return wasm_f64x2_neg(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_abs(simd128_wasm_t _a)
	{
		return wasm_f64x2_abs(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_clamp(simd128_wasm_t _a, simd128_wasm_t _min, simd128_wasm_t _max)
	{
		const simd128_wasm_t lo     = wasm_f64x2_pmax(_a, _min);
		const simd128_wasm_t result = wasm_f64x2_pmin(lo, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_lerp(simd128_wasm_t _a, simd128_wasm_t _b, simd128_wasm_t _s)
	{
		return simd_f64_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_rcp(simd128_wasm_t _a)
	{
		return simd_f64_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_sqrt(simd128_wasm_t _a)
	{
		return wasm_f64x2_sqrt(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_rsqrt(simd128_wasm_t _a)
	{
		return simd_f64_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_round(simd128_wasm_t _a)
	{
		return wasm_f64x2_nearest(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_ceil(simd128_wasm_t _a)
	{
		return wasm_f64x2_ceil(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_floor(simd128_wasm_t _a)
	{
		return wasm_f64x2_floor(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_cmpeq(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_eq(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_cmpneq(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_ne(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_cmplt(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_lt(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_cmple(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_le(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_cmpgt(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_gt(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_f64_cmpge(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_f64x2_ge(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i64_add(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i64x2_add(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_i64_sub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i64x2_sub(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u64_add(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i64x2_add(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_wasm_t simd128_u64_sub(simd128_wasm_t _a, simd128_wasm_t _b)
	{
		return wasm_i64x2_sub(_a, _b);
	}

} // namespace bx
