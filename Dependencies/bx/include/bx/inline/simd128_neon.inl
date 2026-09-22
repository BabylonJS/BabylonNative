/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#	error "Must be included from bx/simd_t.h!"
#endif // BX_SIMD_T_H_HEADER_GUARD

namespace bx
{

#if BX_COMPILER_CLANG
#	define BX_SIMD_SHUFFLE_A(_a,  _i0, _i1, _i2, _i3)     __builtin_shufflevector(_a, _a, _i0, _i1, _i2, _i3 )
#	define BX_SIMD_SHUFFLE_AB(_a, _b, _i0, _i1, _i2, _i3) __builtin_shufflevector(_a, _b, _i0, _i1, _i2, _i3 )
#else
#	define BX_SIMD_SHUFFLE_A(_a,  _i0, _i1, _i2, _i3)     __builtin_shuffle(_a, (uint32x4_t){ _i0, _i1, _i2, _i3 })
#	define BX_SIMD_SHUFFLE_AB(_a, _b, _i0, _i1, _i2, _i3) __builtin_shuffle(_a, _b, (uint32x4_t){ _i0, _i1, _i2, _i3 })
#endif

#define ELEMx 0
#define ELEMy 1
#define ELEMz 2
#define ELEMw 3
#define BX_SIMD128_IMPLEMENT_SWIZZLE(_x, _y, _z, _w)                                                 \
			template<>                                                                               \
			BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_swiz_##_x##_y##_z##_w(simd128_neon_t _a) \
			{                                                                                        \
				return BX_SIMD_SHUFFLE_A(_a, ELEM##_x, ELEM##_y, ELEM##_z, ELEM##_w);                \
			}

#include "simd128_swizzle.inl"

#undef BX_SIMD128_IMPLEMENT_SWIZZLE
#undef ELEMw
#undef ELEMz
#undef ELEMy
#undef ELEMx

#define BX_SIMD128_IMPLEMENT_TEST(_xyzw, _swizzle)                                    \
			template<>                                                                \
			BX_SIMD_FORCE_INLINE bool simd128_test_any_##_xyzw(simd128_neon_t _test)  \
			{                                                                         \
				const simd128_neon_t tmp0 = simd128_x32_swiz_##_swizzle(_test);       \
				return simd128_test_any_ni(tmp0);                                     \
			}                                                                         \
			                                                                          \
			template<>                                                                \
			BX_SIMD_FORCE_INLINE bool simd128_test_all_##_xyzw(simd128_neon_t _test)  \
			{                                                                         \
				const simd128_neon_t tmp0 = simd128_x32_swiz_##_swizzle(_test);       \
				return simd128_test_all_ni(tmp0);                                     \
			}

	BX_SIMD128_IMPLEMENT_TEST(x,    xxxx);
	BX_SIMD128_IMPLEMENT_TEST(y,    yyyy);
	BX_SIMD128_IMPLEMENT_TEST(xy,   xyyy);
	BX_SIMD128_IMPLEMENT_TEST(z,    zzzz);
	BX_SIMD128_IMPLEMENT_TEST(xz,   xzzz);
	BX_SIMD128_IMPLEMENT_TEST(yz,   yzzz);
	BX_SIMD128_IMPLEMENT_TEST(xyz,  xyzz);
	BX_SIMD128_IMPLEMENT_TEST(w,    wwww);
	BX_SIMD128_IMPLEMENT_TEST(xw,   xwww);
	BX_SIMD128_IMPLEMENT_TEST(yw,   ywww);
	BX_SIMD128_IMPLEMENT_TEST(xyw,  xyww);
	BX_SIMD128_IMPLEMENT_TEST(zw,   zwww);
	BX_SIMD128_IMPLEMENT_TEST(xzw,  xzww);
	BX_SIMD128_IMPLEMENT_TEST(yzw,  yzww);

#undef BX_SIMD128_IMPLEMENT_TEST

	template<>
	BX_SIMD_FORCE_INLINE bool simd128_test_any_xyzw(simd128_neon_t _test)
	{
		return simd128_test_any_ni(_test);
	}

	template<>
	BX_SIMD_FORCE_INLINE bool simd128_test_all_xyzw(simd128_neon_t _test)
	{
		return simd128_test_all_ni(_test);
	}

	template<>
	BX_SIMD_FORCE_INLINE bool simd128_test_zero(simd128_neon_t _a, simd128_neon_t _b)
	{
#if BX_ARCH_64BIT
		const uint32x4_t ai     = vreinterpretq_u32_f32(_a);
		const uint32x4_t bi     = vreinterpretq_u32_f32(_b);
		const uint32x4_t masked = vandq_u32(ai, bi);
		return 0 == vmaxvq_u32(masked);
#else
		return simd128_test_zero_ni(_a, _b);
#endif // BX_ARCH_64BIT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_xyAB(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 0, 1, 4, 5);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_ABxy(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 4, 5, 0, 1);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_CDzw(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 6, 7, 2, 3);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_zwCD(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 2, 3, 6, 7);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_xAyB(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 0, 4, 1, 5);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_AxBy(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 4, 0, 5, 1);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_zCwD(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 2, 6, 3, 7);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_CzDw(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 6, 2, 7, 3);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_xzAC(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 0, 2, 4, 6);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_ywBD(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 1, 3, 5, 7);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_xxAA(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 0, 0, 4, 4);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_yyBB(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 1, 1, 5, 5);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_zzCC(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 2, 2, 6, 6);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_wwDD(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 3, 3, 7, 7);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_xAzC(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 0, 4, 2, 6);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_shuf_yBwD(simd128_neon_t _a, simd128_neon_t _b)
	{
		return BX_SIMD_SHUFFLE_AB(_a, _b, 1, 5, 3, 7);
	}

#undef BX_SIMD_SHUFFLE_A
#undef BX_SIMD_SHUFFLE_AB

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_x(simd128_neon_t _a) { return vgetq_lane_f32(_a, 0); }

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_y(simd128_neon_t _a) { return vgetq_lane_f32(_a, 1); }

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_z(simd128_neon_t _a) { return vgetq_lane_f32(_a, 2); }

	template<>
	BX_SIMD_FORCE_INLINE float simd128_f32_w(simd128_neon_t _a) { return vgetq_lane_f32(_a, 3); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_ld(const void* _ptr)
	{
		return vld1q_f32(reinterpret_cast<const float32_t*>(_ptr));
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_ldu(const void* _ptr)
	{
		return simd128_ld<simd128_neon_t>(_ptr);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_st(void* _ptr, simd128_neon_t _a)
	{
		vst1q_f32(reinterpret_cast<float32_t*>(_ptr), _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_stu(void* _ptr, simd128_neon_t _a)
	{
		simd128_st(_ptr, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_x32_st1(void* _ptr, simd128_neon_t _a)
	{
		vst1q_lane_f32(reinterpret_cast<float32_t*>(_ptr), _a, 0);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd128_stream(void* _ptr, simd128_neon_t _a)
	{
		vst1q_f32(reinterpret_cast<float32_t*>(_ptr), _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_ld(float _x, float _y, float _z, float _w)
	{
		const float32_t val[4] = {_x, _y, _z, _w};
		return vld1q_f32(val);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_ld(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w)
	{
		const uint32_t val[4] = {_x, _y, _z, _w};
		const uint32x4_t tmp  = vld1q_u32(val);
		return vreinterpretq_f32_u32(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_ld(int32_t _x, int32_t _y, int32_t _z, int32_t _w)
	{
		const int32_t val[4] = {_x, _y, _z, _w};
		const int32x4_t tmp  = vld1q_s32(val);
		return vreinterpretq_f32_s32(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_splat(float _a)
	{
		return vdupq_n_f32(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_splat(int32_t _a)
	{
		const int32x4_t tmp = vdupq_n_s32(_a);
		return vreinterpretq_f32_s32(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_splat(uint32_t _a)
	{
		const int32x4_t tmp = vdupq_n_s32(int32_t(_a));
		return vreinterpretq_f32_s32(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_splat(double _a)
	{
#if BX_ARCH_64BIT
		const float64x2_t tmp = vdupq_n_f64(_a);
		return vreinterpretq_f32_f64(tmp);
#else
		const float val = float(_a);
		return vdupq_n_f32(val);
#endif // BX_ARCH_64BIT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_zero()
	{
		return simd128_splat<simd128_neon_t>(0);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_itof(simd128_neon_t _a)
	{
		const int32x4_t itof = vreinterpretq_s32_f32(_a);
		return vcvtq_f32_s32(itof);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_ftoi_trunc(simd128_neon_t _a)
	{
		const int32x4_t ftoi = vcvtq_s32_f32(_a);
		return vreinterpretq_f32_s32(ftoi);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_ftoi_round(simd128_neon_t _a)
	{
#if BX_ARCH_64BIT
		const int32x4_t ftoi = vcvtnq_s32_f32(_a);
		return vreinterpretq_f32_s32(ftoi);
#else
		return simd_f32_ftoi_round_ni(_a);
#endif // BX_ARCH_64BIT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_round(simd128_neon_t _a)
	{
#if BX_ARCH_64BIT
		return vrndnq_f32(_a);
#else
		return simd_f32_round_ni(_a);
#endif
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_ceil(simd128_neon_t _a)
	{
#if BX_ARCH_64BIT
		return vrndpq_f32(_a);
#else
		return simd_f32_ceil_ni(_a);
#endif
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_floor(simd128_neon_t _a)
	{
#if BX_ARCH_64BIT
		return vrndmq_f32(_a);
#else
		return simd_f32_floor_ni(_a);
#endif
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_add(simd128_neon_t _a, simd128_neon_t _b)
	{
		return vaddq_f32(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_sub(simd128_neon_t _a, simd128_neon_t _b)
	{
		return vsubq_f32(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_mul(simd128_neon_t _a, simd128_neon_t _b)
	{
		return vmulq_f32(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_div(simd128_neon_t _a, simd128_neon_t _b)
	{
#if BX_ARCH_64BIT
		return vdivq_f32(_a, _b);
#else
		return simd_f32_div_nr_ni(_a, _b);
#endif
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_rcp_est(simd128_neon_t _a)
	{
		return vrecpeq_f32(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_rcp(simd128_neon_t _a)
	{
		return simd_f32_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_rsqrt_est(simd128_neon_t _a)
	{
		return vrsqrteq_f32(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_sqrt(simd128_neon_t _a)
	{
#if BX_ARCH_64BIT
		return vsqrtq_f32(_a);
#else
		const simd128_neon_t rsqrt = simd128_f32_rsqrt_est<simd128_neon_t>(_a);
		return simd128_f32_mul<simd128_neon_t>(_a, rsqrt);
#endif // BX_ARCH_64BIT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_rsqrt(simd128_neon_t _a)
	{
		return simd_f32_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_min(simd128_neon_t _a, simd128_neon_t _b)
	{
		return vminq_f32(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_max(simd128_neon_t _a, simd128_neon_t _b)
	{
		return vmaxq_f32(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_cmpeq(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t result = vceqq_f32(_a, _b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_cmplt(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t result = vcltq_f32(_a, _b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_cmple(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t result = vcleq_f32(_a, _b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_cmpgt(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t result = vcgtq_f32(_a, _b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_cmpge(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t result = vcgeq_f32(_a, _b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_add(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int32x4_t a   = vreinterpretq_s32_f32(_a);
		const int32x4_t b   = vreinterpretq_s32_f32(_b);
		const int32x4_t add = vaddq_s32(a, b);
		return vreinterpretq_f32_s32(add);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_sub(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int32x4_t a   = vreinterpretq_s32_f32(_a);
		const int32x4_t b   = vreinterpretq_s32_f32(_b);
		const int32x4_t sub = vsubq_s32(a, b);
		return vreinterpretq_f32_s32(sub);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_neg(simd128_neon_t _a)
	{
		const int32x4_t a      = vreinterpretq_s32_f32(_a);
		const int32x4_t result = vnegq_s32(a);
		return vreinterpretq_f32_s32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_abs(simd128_neon_t _a)
	{
		const int32x4_t a      = vreinterpretq_s32_f32(_a);
		const int32x4_t result = vabsq_s32(a);
		return vreinterpretq_f32_s32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_min(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int32x4_t a      = vreinterpretq_s32_f32(_a);
		const int32x4_t b      = vreinterpretq_s32_f32(_b);
		const int32x4_t result = vminq_s32(a, b);
		return vreinterpretq_f32_s32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_max(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int32x4_t a      = vreinterpretq_s32_f32(_a);
		const int32x4_t b      = vreinterpretq_s32_f32(_b);
		const int32x4_t result = vmaxq_s32(a, b);
		return vreinterpretq_f32_s32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_cmpeq(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int32x4_t  a      = vreinterpretq_s32_f32(_a);
		const int32x4_t  b      = vreinterpretq_s32_f32(_b);
		const uint32x4_t result = vceqq_s32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_cmplt(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int32x4_t  a      = vreinterpretq_s32_f32(_a);
		const int32x4_t  b      = vreinterpretq_s32_f32(_b);
		const uint32x4_t result = vcltq_s32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_cmpgt(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int32x4_t  a      = vreinterpretq_s32_f32(_a);
		const int32x4_t  b      = vreinterpretq_s32_f32(_b);
		const uint32x4_t result = vcgtq_s32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i32_clamp(simd128_neon_t _a, simd128_neon_t _min, simd128_neon_t _max)
	{
		return simd128_i32_min(simd128_i32_max(_a, _min), _max);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u32_add(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vaddq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u32_sub(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vsubq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u32_mul(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vmulq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u32_min(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vminq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u32_max(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vmaxq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u32_clamp(simd128_neon_t _a, simd128_neon_t _min, simd128_neon_t _max)
	{
		return simd128_u32_min(simd128_u32_max(_a, _min), _max);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u32_cmpeq(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vceqq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u32_cmplt(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vcltq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u32_cmpgt(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vcgtq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i16_add(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int16x8_t a      = vreinterpretq_s16_f32(_a);
		const int16x8_t b      = vreinterpretq_s16_f32(_b);
		const int16x8_t result = vaddq_s16(a, b);
		return vreinterpretq_f32_s16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i16_sub(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int16x8_t a      = vreinterpretq_s16_f32(_a);
		const int16x8_t b      = vreinterpretq_s16_f32(_b);
		const int16x8_t result = vsubq_s16(a, b);
		return vreinterpretq_f32_s16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i16_mullo(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int16x8_t a      = vreinterpretq_s16_f32(_a);
		const int16x8_t b      = vreinterpretq_s16_f32(_b);
		const int16x8_t result = vmulq_s16(a, b);
		return vreinterpretq_f32_s16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i16_cmpeq(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int16x8_t  a      = vreinterpretq_s16_f32(_a);
		const int16x8_t  b      = vreinterpretq_s16_f32(_b);
		const uint16x8_t result = vceqq_s16(a, b);
		return vreinterpretq_f32_u16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x16_sll(simd128_neon_t _a, int _count)
	{
		const uint16x8_t a       = vreinterpretq_u16_f32(_a);
		const int16x8_t  shift   = vdupq_n_s16((int16_t)_count);
		const uint16x8_t result  = vshlq_u16(a, shift);
		return vreinterpretq_f32_u16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x16_srl(simd128_neon_t _a, int _count)
	{
		const uint16x8_t a       = vreinterpretq_u16_f32(_a);
		const int16x8_t  shift   = vdupq_n_s16((int16_t)-_count);
		const uint16x8_t result  = vshlq_u16(a, shift);
		return vreinterpretq_f32_u16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_splat(int16_t _a)
	{
		const int16x8_t result = vdupq_n_s16(_a);
		return vreinterpretq_f32_s16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_splat(uint16_t _a)
	{
		const uint16x8_t result = vdupq_n_u16(_a);
		return vreinterpretq_f32_u16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i8_add(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int8x16_t a      = vreinterpretq_s8_f32(_a);
		const int8x16_t b      = vreinterpretq_s8_f32(_b);
		const int8x16_t result = vaddq_s8(a, b);
		return vreinterpretq_f32_s8(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i8_sub(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int8x16_t a      = vreinterpretq_s8_f32(_a);
		const int8x16_t b      = vreinterpretq_s8_f32(_b);
		const int8x16_t result = vsubq_s8(a, b);
		return vreinterpretq_f32_s8(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u8_satadd(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint8x16_t a      = vreinterpretq_u8_f32(_a);
		const uint8x16_t b      = vreinterpretq_u8_f32(_b);
		const uint8x16_t result = vqaddq_u8(a, b);
		return vreinterpretq_f32_u8(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u8_satsub(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint8x16_t a      = vreinterpretq_u8_f32(_a);
		const uint8x16_t b      = vreinterpretq_u8_f32(_b);
		const uint8x16_t result = vqsubq_u8(a, b);
		return vreinterpretq_f32_u8(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u16_satadd(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint16x8_t a      = vreinterpretq_u16_f32(_a);
		const uint16x8_t b      = vreinterpretq_u16_f32(_b);
		const uint16x8_t result = vqaddq_u16(a, b);
		return vreinterpretq_f32_u16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u16_satsub(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint16x8_t a      = vreinterpretq_u16_f32(_a);
		const uint16x8_t b      = vreinterpretq_u16_f32(_b);
		const uint16x8_t result = vqsubq_u16(a, b);
		return vreinterpretq_f32_u16(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_and(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vandq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_andc(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vbicq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_or(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vorrq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_xor(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = veorq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_sll(simd128_neon_t _a, int _count)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const int32x4_t  shift  = vdupq_n_s32(_count);
		const uint32x4_t result = vshlq_u32(a, shift);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_srl(simd128_neon_t _a, int _count)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const int32x4_t  shift  = vdupq_n_s32(-_count);
		const uint32x4_t result = vshlq_u32(a, shift);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_sra(simd128_neon_t _a, int _count)
	{
		const int32x4_t a      = vreinterpretq_s32_f32(_a);
		const int32x4_t shift  = vdupq_n_s32(-_count);
		const int32x4_t result = vshlq_s32(a, shift);
		return vreinterpretq_f32_s32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_sll(simd128_neon_t _a, simd128_neon_t _count)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const int32x4_t  c      = vreinterpretq_s32_f32(_count);
		const uint32x4_t result = vshlq_u32(a, c);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_srl(simd128_neon_t _a, simd128_neon_t _count)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const int32x4_t  c      = vreinterpretq_s32_f32(_count);
		const int32x4_t  cneg   = vnegq_s32(c);
		const uint32x4_t result = vshlq_u32(a, cneg);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x32_sra(simd128_neon_t _a, simd128_neon_t _count)
	{
		const int32x4_t a      = vreinterpretq_s32_f32(_a);
		const int32x4_t c      = vreinterpretq_s32_f32(_count);
		const int32x4_t cneg   = vnegq_s32(c);
		const int32x4_t result = vshlq_s32(a, cneg);
		return vreinterpretq_f32_s32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x8_shuffle(simd128_neon_t _a, simd128_neon_t _indices)
	{
#if BX_ARCH_64BIT
		const uint8x16_t a       = vreinterpretq_u8_f32(_a);
		const uint8x16_t indices = vreinterpretq_u8_f32(_indices);
		const uint8x16_t result  = vqtbl1q_u8(a, indices);
		return vreinterpretq_f32_u8(result);
#else
		return simd_x8_shuffle_ni(_a, _indices);
#endif // BX_ARCH_64BIT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_x8_shuffle(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _indices)
	{
#if BX_ARCH_64BIT
		const uint8x16x2_t tbl     = { { vreinterpretq_u8_f32(_a), vreinterpretq_u8_f32(_b) } };
		const uint8x16_t   indices = vreinterpretq_u8_f32(_indices);
		const uint8x16_t   result  = vqtbl2q_u8(tbl, indices);
		return vreinterpretq_f32_u8(result);
#else
		return simd_x8_shuffle_ni(_a, _b, _indices);
#endif // BX_ARCH_64BIT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_selb(simd128_neon_t _mask, simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t mask   = vreinterpretq_u32_f32(_mask);
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vbslq_u32(mask, a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_sels(simd128_neon_t _test, simd128_neon_t _a, simd128_neon_t _b)
	{
		return simd_sels_ni(_test, _a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_not(simd128_neon_t _a)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t result = vmvnq_u32(a);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_orc(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint32x4_t a      = vreinterpretq_u32_f32(_a);
		const uint32x4_t b      = vreinterpretq_u32_f32(_b);
		const uint32x4_t result = vornq_u32(a, b);
		return vreinterpretq_f32_u32(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_orx(simd128_neon_t _a)
	{
		return simd128_orx_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_madd(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _c)
	{
#if BX_ARCH_64BIT
		return vfmaq_f32(_c, _a, _b);
#else
		return simd_f32_madd_ni(_a, _b, _c);
#endif
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_msub(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _c)
	{
		return simd_f32_msub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_nmsub(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _c)
	{
#if BX_ARCH_64BIT
		return vfmsq_f32(_c, _a, _b);
#else
		return simd_f32_nmsub_ni(_a, _b, _c);
#endif
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd128_x32_signbitsmask(simd128_neon_t _a)
	{
		return simd_x32_signbitsmask_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd128_x8_signbitsmask(simd128_neon_t _a)
	{
		return simd_x8_signbitsmask_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_neg(simd128_neon_t _a)
	{
		return vnegq_f32(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_abs(simd128_neon_t _a)
	{
		return vabsq_f32(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_clamp(simd128_neon_t _a, simd128_neon_t _min, simd128_neon_t _max)
	{
		return simd_f32_clamp_ni(_a, _min, _max);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_lerp(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _s)
	{
		return simd_f32_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_sqrt_nr(simd128_neon_t _a)
	{
		return simd_f32_sqrt_nr_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_rsqrt_nr(simd128_neon_t _a)
	{
		return simd_f32_rsqrt_nr_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_rsqrt_carmack(simd128_neon_t _a)
	{
		return simd_f32_rsqrt_carmack_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_div_nr(simd128_neon_t _a, simd128_neon_t _b)
	{
		return simd_f32_div_nr_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_dot3(simd128_neon_t _a, simd128_neon_t _b)
	{
		return simd128_f32_dot3_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_dot(simd128_neon_t _a, simd128_neon_t _b)
	{
		return simd128_f32_dot_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_cross3(simd128_neon_t _a, simd128_neon_t _b)
	{
		return simd128_f32_cross3_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_normalize3(simd128_neon_t _a)
	{
		return simd128_f32_normalize3_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f32_cmpneq(simd128_neon_t _a, simd128_neon_t _b)
	{
		return simd_f32_cmpneq_ni(_a, _b);
	}

#if BX_ARCH_64BIT

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_add(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const float64x2_t result = vaddq_f64(a, b);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_sub(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const float64x2_t result = vsubq_f64(a, b);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_mul(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const float64x2_t result = vmulq_f64(a, b);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_div(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const float64x2_t result = vdivq_f64(a, b);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_min(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const float64x2_t result = vminq_f64(a, b);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_max(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const float64x2_t result = vmaxq_f64(a, b);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_madd(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _c)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const float64x2_t c      = vreinterpretq_f64_f32(_c);
		const float64x2_t result = vmlaq_f64(c, a, b);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_nmsub(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _c)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const float64x2_t c      = vreinterpretq_f64_f32(_c);
		const float64x2_t result = vfmsq_f64(c, a, b);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_neg(simd128_neon_t _a)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t result = vnegq_f64(a);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_abs(simd128_neon_t _a)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t result = vabsq_f64(a);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_clamp(simd128_neon_t _a, simd128_neon_t _min, simd128_neon_t _max)
	{
		return simd128_f64_min(simd128_f64_max(_a, _min), _max);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_lerp(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _s)
	{
		return simd_f64_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_rcp(simd128_neon_t _a)
	{
		return simd_f64_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_sqrt(simd128_neon_t _a)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t result = vsqrtq_f64(a);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_rsqrt(simd128_neon_t _a)
	{
		return simd_f64_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_round(simd128_neon_t _a)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t result = vrndnq_f64(a);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_ceil(simd128_neon_t _a)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t result = vrndpq_f64(a);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_floor(simd128_neon_t _a)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t result = vrndmq_f64(a);
		return vreinterpretq_f32_f64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmpeq(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const uint64x2_t  result = vceqq_f64(a, b);
		return vreinterpretq_f32_u64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmpneq(simd128_neon_t _a, simd128_neon_t _b)
	{
		return simd_f64_cmpneq_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmplt(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const uint64x2_t  result = vcltq_f64(a, b);
		return vreinterpretq_f32_u64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmple(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const uint64x2_t  result = vcleq_f64(a, b);
		return vreinterpretq_f32_u64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmpgt(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const uint64x2_t  result = vcgtq_f64(a, b);
		return vreinterpretq_f32_u64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmpge(simd128_neon_t _a, simd128_neon_t _b)
	{
		const float64x2_t a      = vreinterpretq_f64_f32(_a);
		const float64x2_t b      = vreinterpretq_f64_f32(_b);
		const uint64x2_t  result = vcgeq_f64(a, b);
		return vreinterpretq_f32_u64(result);
	}

#else

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_add(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2], r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] + b[0]; r[1] = a[1] + b[1];
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_sub(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2], r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] - b[0]; r[1] = a[1] - b[1];
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_mul(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2], r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] * b[0]; r[1] = a[1] * b[1];
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_div(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2], r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] / b[0]; r[1] = a[1] / b[1];
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_min(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2], r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] < b[0] ? a[0] : b[0]; r[1] = a[1] < b[1] ? a[1] : b[1];
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_max(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2], r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] > b[0] ? a[0] : b[0]; r[1] = a[1] > b[1] ? a[1] : b[1];
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_madd(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _c)
	{
		return simd_f64_madd_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_nmsub(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _c)
	{
		return simd_f64_nmsub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_neg(simd128_neon_t _a)
	{
		return simd_f64_neg_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_abs(simd128_neon_t _a)
	{
		return simd_f64_abs_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_clamp(simd128_neon_t _a, simd128_neon_t _min, simd128_neon_t _max)
	{
		return simd128_f64_min(simd128_f64_max(_a, _min), _max);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_lerp(simd128_neon_t _a, simd128_neon_t _b, simd128_neon_t _s)
	{
		return simd_f64_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_rcp(simd128_neon_t _a)
	{
		return simd_f64_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_sqrt(simd128_neon_t _a)
	{
		double a[2], r[2];
		vst1q_f32((float*)a, _a);
		r[0] = __builtin_sqrt(a[0]); r[1] = __builtin_sqrt(a[1]);
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_rsqrt(simd128_neon_t _a)
	{
		return simd_f64_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_round(simd128_neon_t _a)
	{
		double a[2], r[2];
		vst1q_f32((float*)a, _a);
		r[0] = __builtin_round(a[0]); r[1] = __builtin_round(a[1]);
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_ceil(simd128_neon_t _a)
	{
		double a[2], r[2];
		vst1q_f32((float*)a, _a);
		r[0] = __builtin_ceil(a[0]); r[1] = __builtin_ceil(a[1]);
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_floor(simd128_neon_t _a)
	{
		double a[2], r[2];
		vst1q_f32((float*)a, _a);
		r[0] = __builtin_floor(a[0]); r[1] = __builtin_floor(a[1]);
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmpeq(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2]; uint64_t r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] == b[0] ? UINT64_MAX : 0; r[1] = a[1] == b[1] ? UINT64_MAX : 0;
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmpneq(simd128_neon_t _a, simd128_neon_t _b)
	{
		return simd_f64_cmpneq_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmplt(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2]; uint64_t r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] < b[0] ? UINT64_MAX : 0; r[1] = a[1] < b[1] ? UINT64_MAX : 0;
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmple(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2]; uint64_t r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] <= b[0] ? UINT64_MAX : 0; r[1] = a[1] <= b[1] ? UINT64_MAX : 0;
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmpgt(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2]; uint64_t r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] > b[0] ? UINT64_MAX : 0; r[1] = a[1] > b[1] ? UINT64_MAX : 0;
		return vld1q_f32((const float*)r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_f64_cmpge(simd128_neon_t _a, simd128_neon_t _b)
	{
		double a[2], b[2]; uint64_t r[2];
		vst1q_f32((float*)a, _a); vst1q_f32((float*)b, _b);
		r[0] = a[0] >= b[0] ? UINT64_MAX : 0; r[1] = a[1] >= b[1] ? UINT64_MAX : 0;
		return vld1q_f32((const float*)r);
	}

#endif // BX_ARCH_64BIT

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i64_add(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int64x2_t a      = vreinterpretq_s64_f32(_a);
		const int64x2_t b      = vreinterpretq_s64_f32(_b);
		const int64x2_t result = vaddq_s64(a, b);
		return vreinterpretq_f32_s64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_i64_sub(simd128_neon_t _a, simd128_neon_t _b)
	{
		const int64x2_t a      = vreinterpretq_s64_f32(_a);
		const int64x2_t b      = vreinterpretq_s64_f32(_b);
		const int64x2_t result = vsubq_s64(a, b);
		return vreinterpretq_f32_s64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u64_add(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint64x2_t a      = vreinterpretq_u64_f32(_a);
		const uint64x2_t b      = vreinterpretq_u64_f32(_b);
		const uint64x2_t result = vaddq_u64(a, b);
		return vreinterpretq_f32_u64(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd128_neon_t simd128_u64_sub(simd128_neon_t _a, simd128_neon_t _b)
	{
		const uint64x2_t a      = vreinterpretq_u64_f32(_a);
		const uint64x2_t b      = vreinterpretq_u64_f32(_b);
		const uint64x2_t result = vsubq_u64(a, b);
		return vreinterpretq_f32_u64(result);
	}

} // namespace bx
