/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#	error "Must be included from bx/simd_t.h!"
#endif // BX_SIMD_T_H_HEADER_GUARD

namespace bx
{
	// 256-bit reference delegates to two 128-bit operations.

#if BX_SIMD_LANGEXT
	typedef float    simd256_f32_langext_t __attribute__((__vector_size__(32), __aligned__(32)));
	typedef double   simd256_f64_langext_t __attribute__((__vector_size__(32), __aligned__(32)));
	typedef int8_t   simd256_i8_langext_t  __attribute__((__vector_size__(32), __aligned__(32)));
	typedef int16_t  simd256_i16_langext_t __attribute__((__vector_size__(32), __aligned__(32)));
	typedef int32_t  simd256_i32_langext_t __attribute__((__vector_size__(32), __aligned__(32)));
	typedef int64_t  simd256_i64_langext_t __attribute__((__vector_size__(32), __aligned__(32)));
	typedef uint8_t  simd256_u8_langext_t  __attribute__((__vector_size__(32), __aligned__(32)));
	typedef uint16_t simd256_u16_langext_t __attribute__((__vector_size__(32), __aligned__(32)));
	typedef uint32_t simd256_u32_langext_t __attribute__((__vector_size__(32), __aligned__(32)));
	typedef uint64_t simd256_u64_langext_t __attribute__((__vector_size__(32), __aligned__(32)));
#endif // BX_SIMD_LANGEXT

#if !BX_SIMD_AVX

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_ld(const void* _ptr)
	{
		simd256_ref_t result;
		memCopy(&result, _ptr, sizeof(simd256_ref_t) );
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_ldu(const void* _ptr)
	{
		return simd256_ld<simd256_ref_t>(_ptr);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd256_st(void* _ptr, simd256_ref_t _a)
	{
		memCopy(_ptr, &_a, sizeof(simd256_ref_t) );
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd256_stu(void* _ptr, simd256_ref_t _a)
	{
		simd256_st(_ptr, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd256_stream(void* _ptr, simd256_ref_t _a)
	{
		simd256_st(_ptr, _a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_ld(float _x0, float _x1, float _x2, float _x3, float _x4, float _x5, float _x6, float _x7)
	{
		simd256_ref_t result;
		result.lo = simd128_ld<simd128_ref_t>(_x0, _x1, _x2, _x3);
		result.hi = simd128_ld<simd128_ref_t>(_x4, _x5, _x6, _x7);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_ld(int32_t _x0, int32_t _x1, int32_t _x2, int32_t _x3, int32_t _x4, int32_t _x5, int32_t _x6, int32_t _x7)
	{
		simd256_ref_t result;
		result.lo = simd128_ld<simd128_ref_t>(_x0, _x1, _x2, _x3);
		result.hi = simd128_ld<simd128_ref_t>(_x4, _x5, _x6, _x7);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_ld(uint32_t _x0, uint32_t _x1, uint32_t _x2, uint32_t _x3, uint32_t _x4, uint32_t _x5, uint32_t _x6, uint32_t _x7)
	{
		simd256_ref_t result;
		result.lo = simd128_ld<simd128_ref_t>(_x0, _x1, _x2, _x3);
		result.hi = simd128_ld<simd128_ref_t>(_x4, _x5, _x6, _x7);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_splat(float _a)
	{
		simd256_ref_t result;
		result.lo = simd128_splat<simd128_ref_t>(_a);
		result.hi = result.lo;
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_splat(int32_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_splat<simd128_ref_t>(_a);
		result.hi = result.lo;
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_splat(uint32_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_splat<simd128_ref_t>(_a);
		result.hi = result.lo;
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_splat(double _a)
	{
		simd256_ref_t result;
		result.lo = simd128_splat<simd128_ref_t>(_a);
		result.hi = result.lo;
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_splat(int16_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_splat<simd128_ref_t>(_a);
		result.hi = result.lo;
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_splat(uint16_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_splat<simd128_ref_t>(_a);
		result.hi = result.lo;
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_zero()
	{
		simd256_ref_t result;
		result.lo = simd128_zero<simd128_ref_t>();
		result.hi = result.lo;
		return result;
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_i32_itof(simd256_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd256_i32_langext_t a         = bitCast<simd256_i32_langext_t>(_a);
		const simd256_f32_langext_t converted = __builtin_convertvector(a, simd256_f32_langext_t);
		const simd256_ref_t         result    = bitCast<simd256_ref_t>(converted);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i32_itof(_a.lo);
		result.hi = simd128_i32_itof(_a.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_f32_ftoi_trunc(simd256_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a         = bitCast<simd256_f32_langext_t>(_a);
		const simd256_i32_langext_t converted = __builtin_convertvector(a, simd256_i32_langext_t);
		const simd256_ref_t         result    = bitCast<simd256_ref_t>(converted);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f32_ftoi_trunc(_a.lo);
		result.hi = simd128_f32_ftoi_trunc(_a.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_ftoi_round(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f32_ftoi_round(_a.lo);
		result.hi = simd128_f32_ftoi_round(_a.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_add(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t sum    = a + b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f32_add(_a.lo, _b.lo);
		result.hi = simd128_f32_add(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_sub(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t diff   = a - b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f32_sub(_a.lo, _b.lo);
		result.hi = simd128_f32_sub(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_mul(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t prod   = a * b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(prod);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f32_mul(_a.lo, _b.lo);
		result.hi = simd128_f32_mul(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_div(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t quot   = a / b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(quot);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f32_div(_a.lo, _b.lo);
		result.hi = simd128_f32_div(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_f32_madd(simd256_ref_t _a, simd256_ref_t _b, simd256_ref_t _c)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t c      = bitCast<simd256_f32_langext_t>(_c);
		const simd256_f32_langext_t prod   = a * b;
		const simd256_f32_langext_t sum    = prod + c;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		return simd_f32_madd_ni(_a, _b, _c);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_f32_msub(simd256_ref_t _a, simd256_ref_t _b, simd256_ref_t _c)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t c      = bitCast<simd256_f32_langext_t>(_c);
		const simd256_f32_langext_t prod   = a * b;
		const simd256_f32_langext_t diff   = prod - c;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		return simd_f32_msub_ni(_a, _b, _c);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_f32_nmsub(simd256_ref_t _a, simd256_ref_t _b, simd256_ref_t _c)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t c      = bitCast<simd256_f32_langext_t>(_c);
		const simd256_f32_langext_t prod   = a * b;
		const simd256_f32_langext_t diff   = c - prod;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		return simd_f32_nmsub_ni(_a, _b, _c);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_f32_neg(simd256_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t neg    = -a;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(neg);
		return result;
#else
		return simd_f32_neg_ni(_a);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_f32_abs(simd256_ref_t _a)
	{
		return simd_f32_abs_ni(_a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_min(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_f32_min(_a.lo, _b.lo);
		result.hi = simd128_f32_min(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_max(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_f32_max(_a.lo, _b.lo);
		result.hi = simd128_f32_max(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_clamp(simd256_ref_t _a, simd256_ref_t _min, simd256_ref_t _max)
	{
		const simd256_ref_t maxed  = simd256_f32_max(_a, _min);
		const simd256_ref_t result = simd256_f32_min(maxed, _max);
		return result;
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_f32_lerp(simd256_ref_t _a, simd256_ref_t _b, simd256_ref_t _s)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a       = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b       = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t s       = bitCast<simd256_f32_langext_t>(_s);
		const simd256_f32_langext_t diff    = b - a;
		const simd256_f32_langext_t scaled  = diff * s;
		const simd256_f32_langext_t sum     = a + scaled;
		const simd256_ref_t    result  = bitCast<simd256_ref_t>(sum);
		return result;
#else
		return simd_f32_lerp_ni(_a, _b, _s);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_f32_rcp(simd256_ref_t _a)
	{
		return simd_f32_rcp_ni(_a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_rcp_est(simd256_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t one = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t quot   = one / a;
		const simd256_ref_t    result = bitCast<simd256_ref_t>(quot);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f32_rcp_est(_a.lo);
		result.hi = simd128_f32_rcp_est(_a.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_f32_sqrt(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f32_sqrt(_a.lo);
		result.hi = simd128_f32_sqrt(_a.hi);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_f32_rsqrt(simd256_ref_t _a)
	{
		return simd_f32_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_f32_rsqrt_est(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f32_rsqrt_est(_a.lo);
		result.hi = simd128_f32_rsqrt_est(_a.hi);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_f32_round(simd256_ref_t _a)
	{
		return simd_f32_round_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_f32_ceil(simd256_ref_t _a)
	{
		return simd_f32_ceil_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_f32_floor(simd256_ref_t _a)
	{
		return simd_f32_floor_ni(_a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_cmpeq(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t cmp    = a == b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f32_cmpeq(_a.lo, _b.lo);
		result.hi = simd128_f32_cmpeq(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_f32_cmpneq(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t cmp    = a != b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		return simd_f32_cmpneq_ni(_a, _b);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_cmplt(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t cmp    = a < b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f32_cmplt(_a.lo, _b.lo);
		result.hi = simd128_f32_cmplt(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f32_cmpgt(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f32_langext_t a      = bitCast<simd256_f32_langext_t>(_a);
		const simd256_f32_langext_t b      = bitCast<simd256_f32_langext_t>(_b);
		const simd256_f32_langext_t cmp    = a > b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f32_cmpgt(_a.lo, _b.lo);
		result.hi = simd128_f32_cmpgt(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i32_add(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i32_langext_t a      = bitCast<simd256_i32_langext_t>(_a);
		const simd256_i32_langext_t b      = bitCast<simd256_i32_langext_t>(_b);
		const simd256_i32_langext_t sum    = a + b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i32_add(_a.lo, _b.lo);
		result.hi = simd128_i32_add(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i32_sub(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i32_langext_t a      = bitCast<simd256_i32_langext_t>(_a);
		const simd256_i32_langext_t b      = bitCast<simd256_i32_langext_t>(_b);
		const simd256_i32_langext_t diff   = a - b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i32_sub(_a.lo, _b.lo);
		result.hi = simd128_i32_sub(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_i32_neg(simd256_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd256_i32_langext_t a      = bitCast<simd256_i32_langext_t>(_a);
		const simd256_i32_langext_t neg    = -a;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(neg);
		return result;
#else
		return simd_i32_neg_ni(_a);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_i32_abs(simd256_ref_t _a)
	{
		return simd_i32_abs_ni(_a);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i32_min(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_i32_min(_a.lo, _b.lo);
		result.hi = simd128_i32_min(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i32_max(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_i32_max(_a.lo, _b.lo);
		result.hi = simd128_i32_max(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i32_clamp(simd256_ref_t _a, simd256_ref_t _min, simd256_ref_t _max)
	{
		const simd256_ref_t maxed  = simd256_i32_max(_a, _min);
		const simd256_ref_t result = simd256_i32_min(maxed, _max);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i32_cmpeq(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i32_langext_t a      = bitCast<simd256_i32_langext_t>(_a);
		const simd256_i32_langext_t b      = bitCast<simd256_i32_langext_t>(_b);
		const simd256_i32_langext_t cmp    = a == b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i32_cmpeq(_a.lo, _b.lo);
		result.hi = simd128_i32_cmpeq(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i32_cmplt(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i32_langext_t a      = bitCast<simd256_i32_langext_t>(_a);
		const simd256_i32_langext_t b      = bitCast<simd256_i32_langext_t>(_b);
		const simd256_i32_langext_t cmp    = a < b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i32_cmplt(_a.lo, _b.lo);
		result.hi = simd128_i32_cmplt(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i32_cmpgt(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i32_langext_t a      = bitCast<simd256_i32_langext_t>(_a);
		const simd256_i32_langext_t b      = bitCast<simd256_i32_langext_t>(_b);
		const simd256_i32_langext_t cmp    = a > b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i32_cmpgt(_a.lo, _b.lo);
		result.hi = simd128_i32_cmpgt(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u32_add(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t sum    = a + b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_u32_add(_a.lo, _b.lo);
		result.hi = simd128_u32_add(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u32_sub(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t diff   = a - b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_u32_sub(_a.lo, _b.lo);
		result.hi = simd128_u32_sub(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u32_mul(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t prod   = a * b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(prod);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_u32_mul(_a.lo, _b.lo);
		result.hi = simd128_u32_mul(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u32_min(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_u32_min(_a.lo, _b.lo);
		result.hi = simd128_u32_min(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u32_max(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_u32_max(_a.lo, _b.lo);
		result.hi = simd128_u32_max(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u32_clamp(simd256_ref_t _a, simd256_ref_t _min, simd256_ref_t _max)
	{
		const simd256_ref_t maxed  = simd256_u32_max(_a, _min);
		const simd256_ref_t result = simd256_u32_min(maxed, _max);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u32_cmpeq(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t cmp    = a == b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_u32_cmpeq(_a.lo, _b.lo);
		result.hi = simd128_u32_cmpeq(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u32_cmplt(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t cmp    = a < b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_u32_cmplt(_a.lo, _b.lo);
		result.hi = simd128_u32_cmplt(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u32_cmpgt(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t cmp    = a > b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_u32_cmpgt(_a.lo, _b.lo);
		result.hi = simd128_u32_cmpgt(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_and(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t masked = a & b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(masked);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_and(_a.lo, _b.lo);
		result.hi = simd128_and(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_or(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t ored   = a | b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(ored);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_or(_a.lo, _b.lo);
		result.hi = simd128_or(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_xor(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t xored  = a ^ b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(xored);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_xor(_a.lo, _b.lo);
		result.hi = simd128_xor(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_not(simd256_ref_t _a)
	{
		return simd_not_ni(_a);
	}

	template<>
	inline BX_CONST_FUNC simd256_ref_t simd256_andc(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a      = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t b      = bitCast<simd256_u32_langext_t>(_b);
		const simd256_u32_langext_t notb   = ~b;
		const simd256_u32_langext_t masked = a & notb;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(masked);
		return result;
#else
		return simd_andc_ni(_a, _b);
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_orc(simd256_ref_t _a, simd256_ref_t _b)
	{
		return simd_orc_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_selb(simd256_ref_t _mask, simd256_ref_t _a, simd256_ref_t _b)
	{
		return simd_selb_ni(_mask, _a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_sels(simd256_ref_t _test, simd256_ref_t _a, simd256_ref_t _b)
	{
		return simd_sels_ni(_test, _a, _b);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_x32_sll(simd256_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a       = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t shifted = a << _count;
		const simd256_ref_t         result  = bitCast<simd256_ref_t>(shifted);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_x32_sll(_a.lo, _count);
		result.hi = simd128_x32_sll(_a.hi, _count);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_x32_srl(simd256_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd256_u32_langext_t a       = bitCast<simd256_u32_langext_t>(_a);
		const simd256_u32_langext_t shifted = a >> _count;
		const simd256_ref_t         result  = bitCast<simd256_ref_t>(shifted);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_x32_srl(_a.lo, _count);
		result.hi = simd128_x32_srl(_a.hi, _count);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_x32_sra(simd256_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd256_i32_langext_t a       = bitCast<simd256_i32_langext_t>(_a);
		const simd256_i32_langext_t shifted = a >> _count;
		const simd256_ref_t         result  = bitCast<simd256_ref_t>(shifted);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_x32_sra(_a.lo, _count);
		result.hi = simd128_x32_sra(_a.hi, _count);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_x32_sll(simd256_ref_t _a, simd256_ref_t _count)
	{
		return simd_x32_sll_ni<simd256_ref_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_x32_srl(simd256_ref_t _a, simd256_ref_t _count)
	{
		return simd_x32_srl_ni<simd256_ref_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_x32_sra(simd256_ref_t _a, simd256_ref_t _count)
	{
		return simd_x32_sra_ni<simd256_ref_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_x8_shuffle(simd256_ref_t _a, simd256_ref_t _indices)
	{
		return simd_x8_shuffle_ni<simd256_ref_t>(_a, _indices);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_ref_t simd256_x8_shuffle(simd256_ref_t _a, simd256_ref_t _b, simd256_ref_t _indices)
	{
		return simd_x8_shuffle_ni<simd256_ref_t>(_a, _b, _indices);
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_add(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t sum    = a + b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_add(_a.lo, _b.lo);
		result.hi = simd128_f64_add(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_sub(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t diff   = a - b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_sub(_a.lo, _b.lo);
		result.hi = simd128_f64_sub(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_mul(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t prod   = a * b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(prod);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_mul(_a.lo, _b.lo);
		result.hi = simd128_f64_mul(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_div(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t quot   = a / b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(quot);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_div(_a.lo, _b.lo);
		result.hi = simd128_f64_div(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_min(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_f64_min(_a.lo, _b.lo);
		result.hi = simd128_f64_min(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_max(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_f64_max(_a.lo, _b.lo);
		result.hi = simd128_f64_max(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_madd(simd256_ref_t _a, simd256_ref_t _b, simd256_ref_t _c)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t c      = bitCast<simd256_f64_langext_t>(_c);
		const simd256_f64_langext_t prod   = a * b;
		const simd256_f64_langext_t sum    = prod + c;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_madd(_a.lo, _b.lo, _c.lo);
		result.hi = simd128_f64_madd(_a.hi, _b.hi, _c.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_nmsub(simd256_ref_t _a, simd256_ref_t _b, simd256_ref_t _c)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t c      = bitCast<simd256_f64_langext_t>(_c);
		const simd256_f64_langext_t prod   = a * b;
		const simd256_f64_langext_t diff   = c - prod;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_nmsub(_a.lo, _b.lo, _c.lo);
		result.hi = simd128_f64_nmsub(_a.hi, _b.hi, _c.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_neg(simd256_ref_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t neg    = -a;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(neg);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_neg(_a.lo);
		result.hi = simd128_f64_neg(_a.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_abs(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f64_abs(_a.lo);
		result.hi = simd128_f64_abs(_a.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_clamp(simd256_ref_t _a, simd256_ref_t _min, simd256_ref_t _max)
	{
		const simd256_ref_t maxed  = simd256_f64_max(_a, _min);
		const simd256_ref_t result = simd256_f64_min(maxed, _max);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_lerp(simd256_ref_t _a, simd256_ref_t _b, simd256_ref_t _s)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a       = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b       = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t s       = bitCast<simd256_f64_langext_t>(_s);
		const simd256_f64_langext_t diff    = b - a;
		const simd256_f64_langext_t scaled  = diff * s;
		const simd256_f64_langext_t sum     = a + scaled;
		const simd256_ref_t    result  = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_lerp(_a.lo, _b.lo, _s.lo);
		result.hi = simd128_f64_lerp(_a.hi, _b.hi, _s.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_rcp(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f64_rcp(_a.lo);
		result.hi = simd128_f64_rcp(_a.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_sqrt(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f64_sqrt(_a.lo);
		result.hi = simd128_f64_sqrt(_a.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_rsqrt(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f64_rsqrt(_a.lo);
		result.hi = simd128_f64_rsqrt(_a.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_round(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f64_round(_a.lo);
		result.hi = simd128_f64_round(_a.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_ceil(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f64_ceil(_a.lo);
		result.hi = simd128_f64_ceil(_a.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_floor(simd256_ref_t _a)
	{
		simd256_ref_t result;
		result.lo = simd128_f64_floor(_a.lo);
		result.hi = simd128_f64_floor(_a.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_cmpeq(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t cmp    = a == b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_cmpeq(_a.lo, _b.lo);
		result.hi = simd128_f64_cmpeq(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_cmpneq(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t cmp    = a != b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_cmpneq(_a.lo, _b.lo);
		result.hi = simd128_f64_cmpneq(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_cmplt(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t cmp    = a < b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_cmplt(_a.lo, _b.lo);
		result.hi = simd128_f64_cmplt(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_cmple(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t cmp    = a <= b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_cmple(_a.lo, _b.lo);
		result.hi = simd128_f64_cmple(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_cmpgt(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t cmp    = a > b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_cmpgt(_a.lo, _b.lo);
		result.hi = simd128_f64_cmpgt(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_f64_cmpge(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_f64_langext_t a      = bitCast<simd256_f64_langext_t>(_a);
		const simd256_f64_langext_t b      = bitCast<simd256_f64_langext_t>(_b);
		const simd256_f64_langext_t cmp    = a >= b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_f64_cmpge(_a.lo, _b.lo);
		result.hi = simd128_f64_cmpge(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i64_add(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i64_langext_t a      = bitCast<simd256_i64_langext_t>(_a);
		const simd256_i64_langext_t b      = bitCast<simd256_i64_langext_t>(_b);
		const simd256_i64_langext_t sum    = a + b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i64_add(_a.lo, _b.lo);
		result.hi = simd128_i64_add(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i64_sub(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i64_langext_t a      = bitCast<simd256_i64_langext_t>(_a);
		const simd256_i64_langext_t b      = bitCast<simd256_i64_langext_t>(_b);
		const simd256_i64_langext_t diff   = a - b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i64_sub(_a.lo, _b.lo);
		result.hi = simd128_i64_sub(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u64_add(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u64_langext_t a      = bitCast<simd256_u64_langext_t>(_a);
		const simd256_u64_langext_t b      = bitCast<simd256_u64_langext_t>(_b);
		const simd256_u64_langext_t sum    = a + b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_u64_add(_a.lo, _b.lo);
		result.hi = simd128_u64_add(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u64_sub(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_u64_langext_t a      = bitCast<simd256_u64_langext_t>(_a);
		const simd256_u64_langext_t b      = bitCast<simd256_u64_langext_t>(_b);
		const simd256_u64_langext_t diff   = a - b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_u64_sub(_a.lo, _b.lo);
		result.hi = simd128_u64_sub(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i16_add(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i16_langext_t a      = bitCast<simd256_i16_langext_t>(_a);
		const simd256_i16_langext_t b      = bitCast<simd256_i16_langext_t>(_b);
		const simd256_i16_langext_t sum    = a + b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i16_add(_a.lo, _b.lo);
		result.hi = simd128_i16_add(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i16_sub(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i16_langext_t a      = bitCast<simd256_i16_langext_t>(_a);
		const simd256_i16_langext_t b      = bitCast<simd256_i16_langext_t>(_b);
		const simd256_i16_langext_t diff   = a - b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i16_sub(_a.lo, _b.lo);
		result.hi = simd128_i16_sub(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i16_mullo(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i16_langext_t a      = bitCast<simd256_i16_langext_t>(_a);
		const simd256_i16_langext_t b      = bitCast<simd256_i16_langext_t>(_b);
		const simd256_i16_langext_t prod   = a * b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(prod);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i16_mullo(_a.lo, _b.lo);
		result.hi = simd128_i16_mullo(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i16_cmpeq(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i16_langext_t a      = bitCast<simd256_i16_langext_t>(_a);
		const simd256_i16_langext_t b      = bitCast<simd256_i16_langext_t>(_b);
		const simd256_i16_langext_t cmp    = a == b;
		const simd256_ref_t         result = bitCast<simd256_ref_t>(cmp);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i16_cmpeq(_a.lo, _b.lo);
		result.hi = simd128_i16_cmpeq(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_x16_sll(simd256_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd256_u16_langext_t a       = bitCast<simd256_u16_langext_t>(_a);
		const simd256_u16_langext_t shifted = a << _count;
		const simd256_ref_t         result  = bitCast<simd256_ref_t>(shifted);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_x16_sll(_a.lo, _count);
		result.hi = simd128_x16_sll(_a.hi, _count);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_x16_srl(simd256_ref_t _a, int _count)
	{
#if BX_SIMD_LANGEXT
		const simd256_u16_langext_t a       = bitCast<simd256_u16_langext_t>(_a);
		const simd256_u16_langext_t shifted = a >> _count;
		const simd256_ref_t         result  = bitCast<simd256_ref_t>(shifted);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_x16_srl(_a.lo, _count);
		result.hi = simd128_x16_srl(_a.hi, _count);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i8_add(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i8_langext_t a      = bitCast<simd256_i8_langext_t>(_a);
		const simd256_i8_langext_t b      = bitCast<simd256_i8_langext_t>(_b);
		const simd256_i8_langext_t sum    = a + b;
		const simd256_ref_t        result = bitCast<simd256_ref_t>(sum);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i8_add(_a.lo, _b.lo);
		result.hi = simd128_i8_add(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_i8_sub(simd256_ref_t _a, simd256_ref_t _b)
	{
#if BX_SIMD_LANGEXT
		const simd256_i8_langext_t a      = bitCast<simd256_i8_langext_t>(_a);
		const simd256_i8_langext_t b      = bitCast<simd256_i8_langext_t>(_b);
		const simd256_i8_langext_t diff   = a - b;
		const simd256_ref_t        result = bitCast<simd256_ref_t>(diff);
		return result;
#else
		simd256_ref_t result;
		result.lo = simd128_i8_sub(_a.lo, _b.lo);
		result.hi = simd128_i8_sub(_a.hi, _b.hi);
		return result;
#endif // BX_SIMD_LANGEXT
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u8_satadd(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_u8_satadd(_a.lo, _b.lo);
		result.hi = simd128_u8_satadd(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u8_satsub(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_u8_satsub(_a.lo, _b.lo);
		result.hi = simd128_u8_satsub(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u16_satadd(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_u16_satadd(_a.lo, _b.lo);
		result.hi = simd128_u16_satadd(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC simd256_ref_t simd256_u16_satsub(simd256_ref_t _a, simd256_ref_t _b)
	{
		simd256_ref_t result;
		result.lo = simd128_u16_satsub(_a.lo, _b.lo);
		result.hi = simd128_u16_satsub(_a.hi, _b.hi);
		return result;
	}

	template<>
	inline BX_CONSTEXPR_FUNC bool simd256_test_any(simd256_ref_t _test)
	{
		return simd128_test_any_xyzw(_test.lo) || simd128_test_any_xyzw(_test.hi);
	}

	template<>
	inline BX_CONSTEXPR_FUNC bool simd256_test_all(simd256_ref_t _test)
	{
		return simd128_test_all_xyzw(_test.lo) && simd128_test_all_xyzw(_test.hi);
	}

	template<>
	inline BX_CONSTEXPR_FUNC bool simd256_test_zero(simd256_ref_t _a, simd256_ref_t _b)
	{
		return simd128_test_zero(_a.lo, _b.lo) && simd128_test_zero(_a.hi, _b.hi);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd256_x32_signbitsmask(simd256_ref_t _a)
	{
		float tmp[8];
		simd128_st(&tmp[0], _a.lo);
		simd128_st(&tmp[4], _a.hi);
		const uint32_t* bits = reinterpret_cast<const uint32_t*>(tmp);
		int result = 0;
		for (int ii = 0; ii < 8; ++ii)
		{
			result |= ((bits[ii] >> 31) << ii);
		}
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd256_x8_signbitsmask(simd256_ref_t _a)
	{
		float tmp[8];
		simd128_st(&tmp[0], _a.lo);
		simd128_st(&tmp[4], _a.hi);
		const uint8_t* bytes = reinterpret_cast<const uint8_t*>(tmp);
		int result = 0;
		for (int ii = 0; ii < 32; ++ii)
		{
			result |= ((bytes[ii] >> 7) << ii);
		}
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd256_x32_st1(void* _ptr, simd256_ref_t _a)
	{
		simd128_x32_st1(_ptr, _a.lo);
	}

#endif // !BX_SIMD_AVX

} // namespace bx
