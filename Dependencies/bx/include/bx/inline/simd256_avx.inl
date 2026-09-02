/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#	error "Must be included from bx/simd_t.h!"
#endif // BX_SIMD_T_H_HEADER_GUARD

namespace bx
{

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_ld(const void* _ptr)
	{
		return _mm256_load_ps(reinterpret_cast<const float*>(_ptr));
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_ldu(const void* _ptr)
	{
		return _mm256_loadu_ps(reinterpret_cast<const float*>(_ptr));
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd256_st(void* _ptr, simd256_avx_t _a)
	{
		_mm256_store_ps(reinterpret_cast<float*>(_ptr), _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd256_stu(void* _ptr, simd256_avx_t _a)
	{
		_mm256_storeu_ps(reinterpret_cast<float*>(_ptr), _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd256_x32_st1(void* _ptr, simd256_avx_t _a)
	{
		const __m128 lo = _mm256_castps256_ps128(_a);
		_mm_store_ss(reinterpret_cast<float*>(_ptr), lo);
	}

	template<>
	BX_SIMD_FORCE_INLINE void simd256_stream(void* _ptr, simd256_avx_t _a)
	{
		_mm256_stream_ps(reinterpret_cast<float*>(_ptr), _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_splat(float _a)
	{
		return _mm256_set1_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_splat(uint32_t _a)
	{
		const __m256i tmp = _mm256_set1_epi32(int32_t(_a));
		return _mm256_castsi256_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_splat(double _a)
	{
		const __m256d tmp = _mm256_set1_pd(_a);
		return _mm256_castpd_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_splat(int32_t _a)
	{
		const __m256i tmp = _mm256_set1_epi32(_a);
		return _mm256_castsi256_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_splat(int16_t _a)
	{
		const __m256i tmp = _mm256_set1_epi16(_a);
		return _mm256_castsi256_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_splat(uint16_t _a)
	{
		const __m256i tmp = _mm256_set1_epi16(int16_t(_a));
		return _mm256_castsi256_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_ld(float _x0, float _x1, float _x2, float _x3, float _x4, float _x5, float _x6, float _x7)
	{
		return _mm256_setr_ps(_x0, _x1, _x2, _x3, _x4, _x5, _x6, _x7);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_ld(uint32_t _x0, uint32_t _x1, uint32_t _x2, uint32_t _x3, uint32_t _x4, uint32_t _x5, uint32_t _x6, uint32_t _x7)
	{
		const __m256i tmp = _mm256_setr_epi32(int32_t(_x0), int32_t(_x1), int32_t(_x2), int32_t(_x3), int32_t(_x4), int32_t(_x5), int32_t(_x6), int32_t(_x7));
		return _mm256_castsi256_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_ld(int32_t _x0, int32_t _x1, int32_t _x2, int32_t _x3, int32_t _x4, int32_t _x5, int32_t _x6, int32_t _x7)
	{
		const __m256i tmp = _mm256_setr_epi32(_x0, _x1, _x2, _x3, _x4, _x5, _x6, _x7);
		return _mm256_castsi256_ps(tmp);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_zero()
	{
		return _mm256_setzero_ps();
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_itof(simd256_avx_t _a)
	{
		const __m256i a      = _mm256_castps_si256(_a);
		const __m256  result = _mm256_cvtepi32_ps(a);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_ftoi_trunc(simd256_avx_t _a)
	{
		const __m256i result = _mm256_cvttps_epi32(_a);
		return _mm256_castsi256_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_ftoi_round(simd256_avx_t _a)
	{
		const __m256i result = _mm256_cvtps_epi32(_a);
		return _mm256_castsi256_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_add_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_sub_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_mul(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_mul_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_div(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_div_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_rcp_est(simd256_avx_t _a)
	{
		return _mm256_rcp_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_sqrt(simd256_avx_t _a)
	{
		return _mm256_sqrt_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_rsqrt_est(simd256_avx_t _a)
	{
		return _mm256_rsqrt_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_madd(simd256_avx_t _a, simd256_avx_t _b, simd256_avx_t _c)
	{
		return simd_f32_madd_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_msub(simd256_avx_t _a, simd256_avx_t _b, simd256_avx_t _c)
	{
		return simd_f32_msub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_nmsub(simd256_avx_t _a, simd256_avx_t _b, simd256_avx_t _c)
	{
		return simd_f32_nmsub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_neg(simd256_avx_t _a)
	{
		const __m256i signi = _mm256_set1_epi32(int32_t(kFloatSignMask) );
		const __m256  signf = _mm256_castsi256_ps(signi);
		return _mm256_xor_ps(_a, signf);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_abs(simd256_avx_t _a)
	{
		const __m256i maski = _mm256_set1_epi32(int32_t(kFloatExponentMask | kFloatMantissaMask) );
		const __m256  maskf = _mm256_castsi256_ps(maski);
		return _mm256_and_ps(_a, maskf);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_rcp(simd256_avx_t _a)
	{
		return simd_f32_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_rsqrt(simd256_avx_t _a)
	{
		return simd_f32_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_clamp(simd256_avx_t _a, simd256_avx_t _min, simd256_avx_t _max)
	{
		const simd256_avx_t maxed  = _mm256_max_ps(_a, _min);
		const simd256_avx_t result = _mm256_min_ps(maxed, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_lerp(simd256_avx_t _a, simd256_avx_t _b, simd256_avx_t _s)
	{
		return simd_f32_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_min(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_min_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_max(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_max_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_round(simd256_avx_t _a)
	{
		return _mm256_round_ps(_a, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_ceil(simd256_avx_t _a)
	{
		return _mm256_ceil_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_floor(simd256_avx_t _a)
	{
		return _mm256_floor_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_cmpeq(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_cmp_ps(_a, _b, _CMP_EQ_OQ);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_cmpneq(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_cmp_ps(_a, _b, _CMP_NEQ_OQ);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_cmplt(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_cmp_ps(_a, _b, _CMP_LT_OQ);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_cmple(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_cmp_ps(_a, _b, _CMP_LE_OQ);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_cmpgt(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_cmp_ps(_a, _b, _CMP_GT_OQ);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f32_cmpge(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_cmp_ps(_a, _b, _CMP_GE_OQ);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_and(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_and_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_andc(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_andnot_ps(_b, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_or(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_or_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_xor(simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_xor_ps(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_selb(simd256_avx_t _mask, simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_blendv_ps(_b, _a, _mask);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_not(simd256_avx_t _a)
	{
		return simd_not_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_orc(simd256_avx_t _a, simd256_avx_t _b)
	{
		return simd_orc_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_sels(simd256_avx_t _test, simd256_avx_t _a, simd256_avx_t _b)
	{
		return _mm256_blendv_ps(_b, _a, _test);
	}

#if BX_SIMD_AVX2
	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a   = _mm256_castps_si256(_a);
		const __m256i b   = _mm256_castps_si256(_b);
		const __m256i sum    = _mm256_add_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(sum);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a   = _mm256_castps_si256(_a);
		const __m256i b   = _mm256_castps_si256(_b);
		const __m256i diff   = _mm256_sub_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(diff);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_neg(simd256_avx_t _a)
	{
		const __m256i a    = _mm256_castps_si256(_a);
		const __m256i zero = _mm256_setzero_si256();
		const __m256i diff   = _mm256_sub_epi32(zero, a);
		const __m256  result = _mm256_castsi256_ps(diff);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_abs(simd256_avx_t _a)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i absa   = _mm256_abs_epi32(a);
		const __m256  result = _mm256_castsi256_ps(absa);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_min(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i minv   = _mm256_min_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(minv);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_max(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i maxv   = _mm256_max_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(maxv);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_cmpeq(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i cmp    = _mm256_cmpeq_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(cmp);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_cmpgt(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i cmp    = _mm256_cmpgt_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(cmp);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_cmplt(simd256_avx_t _a, simd256_avx_t _b)
	{
		return simd256_i32_cmpgt(_b, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_clamp(simd256_avx_t _a, simd256_avx_t _min, simd256_avx_t _max)
	{
		const simd256_avx_t maxed  = simd256_i32_max(_a, _min);
		const simd256_avx_t result = simd256_i32_min(maxed, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i sum    = _mm256_add_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(sum);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i diff   = _mm256_sub_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(diff);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_mul(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i prod   = _mm256_mullo_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(prod);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_min(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i minv   = _mm256_min_epu32(a, b);
		const __m256  result = _mm256_castsi256_ps(minv);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_max(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i maxv   = _mm256_max_epu32(a, b);
		const __m256  result = _mm256_castsi256_ps(maxv);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_clamp(simd256_avx_t _a, simd256_avx_t _min, simd256_avx_t _max)
	{
		const simd256_avx_t maxed  = simd256_u32_max(_a, _min);
		const simd256_avx_t result = simd256_u32_min(maxed, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_cmpeq(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i cmp    = _mm256_cmpeq_epi32(a, b);
		const __m256  result = _mm256_castsi256_ps(cmp);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_cmplt(simd256_avx_t _a, simd256_avx_t _b)
	{
		return simd_u32_cmplt_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_cmpgt(simd256_avx_t _a, simd256_avx_t _b)
	{
		return simd_u32_cmpgt_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_sll(simd256_avx_t _a, int _count)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i shifted = _mm256_slli_epi32(a, _count);
		const __m256  result = _mm256_castsi256_ps(shifted);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_srl(simd256_avx_t _a, int _count)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i shifted = _mm256_srli_epi32(a, _count);
		const __m256  result = _mm256_castsi256_ps(shifted);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_sra(simd256_avx_t _a, int _count)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i shifted = _mm256_srai_epi32(a, _count);
		const __m256  result = _mm256_castsi256_ps(shifted);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_sll(simd256_avx_t _a, simd256_avx_t _count)
	{
		const __m256i a      = _mm256_castps_si256(_a);
		const __m256i c      = _mm256_castps_si256(_count);
		const __m256i result = _mm256_sllv_epi32(a, c);
		return _mm256_castsi256_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_srl(simd256_avx_t _a, simd256_avx_t _count)
	{
		const __m256i a      = _mm256_castps_si256(_a);
		const __m256i c      = _mm256_castps_si256(_count);
		const __m256i result = _mm256_srlv_epi32(a, c);
		return _mm256_castsi256_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_sra(simd256_avx_t _a, simd256_avx_t _count)
	{
		const __m256i a      = _mm256_castps_si256(_a);
		const __m256i c      = _mm256_castps_si256(_count);
		const __m256i result = _mm256_srav_epi32(a, c);
		return _mm256_castsi256_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i16_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i sum    = _mm256_add_epi16(a, b);
		const __m256  result = _mm256_castsi256_ps(sum);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i16_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i diff   = _mm256_sub_epi16(a, b);
		const __m256  result = _mm256_castsi256_ps(diff);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i16_mullo(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i prod   = _mm256_mullo_epi16(a, b);
		const __m256  result = _mm256_castsi256_ps(prod);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i16_cmpeq(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i cmp    = _mm256_cmpeq_epi16(a, b);
		const __m256  result = _mm256_castsi256_ps(cmp);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x16_sll(simd256_avx_t _a, int _count)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i shifted = _mm256_slli_epi16(a, _count);
		const __m256  result = _mm256_castsi256_ps(shifted);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x16_srl(simd256_avx_t _a, int _count)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i shifted = _mm256_srli_epi16(a, _count);
		const __m256  result = _mm256_castsi256_ps(shifted);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i8_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i sum    = _mm256_add_epi8(a, b);
		const __m256  result = _mm256_castsi256_ps(sum);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i8_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i diff   = _mm256_sub_epi8(a, b);
		const __m256  result = _mm256_castsi256_ps(diff);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u8_satadd(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i sat    = _mm256_adds_epu8(a, b);
		const __m256  result = _mm256_castsi256_ps(sat);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u8_satsub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i sat    = _mm256_subs_epu8(a, b);
		const __m256  result = _mm256_castsi256_ps(sat);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u16_satadd(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i sat    = _mm256_adds_epu16(a, b);
		const __m256  result = _mm256_castsi256_ps(sat);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u16_satsub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a = _mm256_castps_si256(_a);
		const __m256i b = _mm256_castps_si256(_b);
		const __m256i sat    = _mm256_subs_epu16(a, b);
		const __m256  result = _mm256_castsi256_ps(sat);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x8_shuffle(simd256_avx_t _a, simd256_avx_t _indices)
	{
		const __m256i a       = _mm256_castps_si256(_a);
		const __m256i indices = _mm256_castps_si256(_indices);
		const __m256i result  = _mm256_shuffle_epi8(a, indices);
		return _mm256_castsi256_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x8_shuffle(simd256_avx_t _a, simd256_avx_t _b, simd256_avx_t _indices)
	{
		const __m256i a       = _mm256_castps_si256(_a);
		const __m256i b       = _mm256_castps_si256(_b);
		const __m256i indices = _mm256_castps_si256(_indices);
		const __m256i pa      = _mm256_shuffle_epi8(a, indices);
		const __m256i pb      = _mm256_shuffle_epi8(b, indices);
		const __m256i bit4    = _mm256_set1_epi8(0x10);
		const __m256i masked  = _mm256_and_si256(indices, bit4);
		const __m256i pickb   = _mm256_cmpeq_epi8(masked, bit4);
		const __m256i result  = _mm256_blendv_epi8(pa, pb, pickb);
		return _mm256_castsi256_ps(result);
	}

#else // !BX_SIMD_AVX2

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i sumlo  = _mm_add_epi32(aloi, bloi);
		const __m128i sumhi  = _mm_add_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(sumlo);
		const __m128  rhi    = _mm_castsi128_ps(sumhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i difflo  = _mm_sub_epi32(aloi, bloi);
		const __m128i diffhi  = _mm_sub_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(difflo);
		const __m128  rhi    = _mm_castsi128_ps(diffhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_neg(simd256_avx_t _a)
	{
		const __m128i zero = _mm_setzero_si128();
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i difflo  = _mm_sub_epi32(zero, aloi);
		const __m128i diffhi  = _mm_sub_epi32(zero, ahii);
		const __m128  rlo    = _mm_castsi128_ps(difflo);
		const __m128  rhi    = _mm_castsi128_ps(diffhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_abs(simd256_avx_t _a)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i abslo  = _mm_abs_epi32(aloi);
		const __m128i abshi  = _mm_abs_epi32(ahii);
		const __m128  rlo    = _mm_castsi128_ps(abslo);
		const __m128  rhi    = _mm_castsi128_ps(abshi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_min(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i minvlo  = _mm_min_epi32(aloi, bloi);
		const __m128i minvhi  = _mm_min_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(minvlo);
		const __m128  rhi    = _mm_castsi128_ps(minvhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_max(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i maxvlo  = _mm_max_epi32(aloi, bloi);
		const __m128i maxvhi  = _mm_max_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(maxvlo);
		const __m128  rhi    = _mm_castsi128_ps(maxvhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_cmpeq(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i cmplo  = _mm_cmpeq_epi32(aloi, bloi);
		const __m128i cmphi  = _mm_cmpeq_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(cmplo);
		const __m128  rhi    = _mm_castsi128_ps(cmphi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_cmpgt(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i cmplo  = _mm_cmpgt_epi32(aloi, bloi);
		const __m128i cmphi  = _mm_cmpgt_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(cmplo);
		const __m128  rhi    = _mm_castsi128_ps(cmphi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_cmplt(simd256_avx_t _a, simd256_avx_t _b)
	{
		return simd256_i32_cmpgt(_b, _a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i32_clamp(simd256_avx_t _a, simd256_avx_t _min, simd256_avx_t _max)
	{
		const simd256_avx_t maxed  = simd256_i32_max(_a, _min);
		const simd256_avx_t result = simd256_i32_min(maxed, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i sumlo  = _mm_add_epi32(aloi, bloi);
		const __m128i sumhi  = _mm_add_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(sumlo);
		const __m128  rhi    = _mm_castsi128_ps(sumhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i difflo  = _mm_sub_epi32(aloi, bloi);
		const __m128i diffhi  = _mm_sub_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(difflo);
		const __m128  rhi    = _mm_castsi128_ps(diffhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_mul(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i prodlo  = _mm_mullo_epi32(aloi, bloi);
		const __m128i prodhi  = _mm_mullo_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(prodlo);
		const __m128  rhi    = _mm_castsi128_ps(prodhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_min(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i minvlo  = _mm_min_epu32(aloi, bloi);
		const __m128i minvhi  = _mm_min_epu32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(minvlo);
		const __m128  rhi    = _mm_castsi128_ps(minvhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_max(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i maxvlo  = _mm_max_epu32(aloi, bloi);
		const __m128i maxvhi  = _mm_max_epu32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(maxvlo);
		const __m128  rhi    = _mm_castsi128_ps(maxvhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_clamp(simd256_avx_t _a, simd256_avx_t _min, simd256_avx_t _max)
	{
		const simd256_avx_t maxed  = simd256_u32_max(_a, _min);
		const simd256_avx_t result = simd256_u32_min(maxed, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_cmpeq(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i cmplo  = _mm_cmpeq_epi32(aloi, bloi);
		const __m128i cmphi  = _mm_cmpeq_epi32(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(cmplo);
		const __m128  rhi    = _mm_castsi128_ps(cmphi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_cmplt(simd256_avx_t _a, simd256_avx_t _b)
	{
		return simd_u32_cmplt_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u32_cmpgt(simd256_avx_t _a, simd256_avx_t _b)
	{
		return simd_u32_cmpgt_ni(_a, _b);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_sll(simd256_avx_t _a, int _count)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i shiftedlo  = _mm_slli_epi32(aloi, _count);
		const __m128i shiftedhi  = _mm_slli_epi32(ahii, _count);
		const __m128  rlo    = _mm_castsi128_ps(shiftedlo);
		const __m128  rhi    = _mm_castsi128_ps(shiftedhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_srl(simd256_avx_t _a, int _count)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i shiftedlo  = _mm_srli_epi32(aloi, _count);
		const __m128i shiftedhi  = _mm_srli_epi32(ahii, _count);
		const __m128  rlo    = _mm_castsi128_ps(shiftedlo);
		const __m128  rhi    = _mm_castsi128_ps(shiftedhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_sra(simd256_avx_t _a, int _count)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i shiftedlo  = _mm_srai_epi32(aloi, _count);
		const __m128i shiftedhi  = _mm_srai_epi32(ahii, _count);
		const __m128  rlo    = _mm_castsi128_ps(shiftedlo);
		const __m128  rhi    = _mm_castsi128_ps(shiftedhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_sll(simd256_avx_t _a, simd256_avx_t _count)
	{
		return simd_x32_sll_ni<simd256_avx_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_srl(simd256_avx_t _a, simd256_avx_t _count)
	{
		return simd_x32_srl_ni<simd256_avx_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x32_sra(simd256_avx_t _a, simd256_avx_t _count)
	{
		return simd_x32_sra_ni<simd256_avx_t>(_a, _count);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x8_shuffle(simd256_avx_t _a, simd256_avx_t _indices)
	{
		// No AVX2: do PSHUFB on each 128-bit half independently.
		const __m128  aloF   = _mm256_castps256_ps128(_a);
		const __m128  ahiF   = _mm256_extractf128_ps(_a, 1);
		const __m128  iloF   = _mm256_castps256_ps128(_indices);
		const __m128  ihiF   = _mm256_extractf128_ps(_indices, 1);
		const __m128i alo    = _mm_castps_si128(aloF);
		const __m128i ahi    = _mm_castps_si128(ahiF);
		const __m128i ilo    = _mm_castps_si128(iloF);
		const __m128i ihi    = _mm_castps_si128(ihiF);
		const __m128i shuflo = _mm_shuffle_epi8(alo, ilo);
		const __m128i shufhi = _mm_shuffle_epi8(ahi, ihi);
		const __m128  rlo    = _mm_castsi128_ps(shuflo);
		const __m128  rhi    = _mm_castsi128_ps(shufhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x8_shuffle(simd256_avx_t _a, simd256_avx_t _b, simd256_avx_t _indices)
	{
		return simd_x8_shuffle_ni<simd256_avx_t>(_a, _b, _indices);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i16_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i sumlo  = _mm_add_epi16(aloi, bloi);
		const __m128i sumhi  = _mm_add_epi16(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(sumlo);
		const __m128  rhi    = _mm_castsi128_ps(sumhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i16_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i difflo  = _mm_sub_epi16(aloi, bloi);
		const __m128i diffhi  = _mm_sub_epi16(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(difflo);
		const __m128  rhi    = _mm_castsi128_ps(diffhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i16_mullo(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i prodlo  = _mm_mullo_epi16(aloi, bloi);
		const __m128i prodhi  = _mm_mullo_epi16(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(prodlo);
		const __m128  rhi    = _mm_castsi128_ps(prodhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i16_cmpeq(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i cmplo  = _mm_cmpeq_epi16(aloi, bloi);
		const __m128i cmphi  = _mm_cmpeq_epi16(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(cmplo);
		const __m128  rhi    = _mm_castsi128_ps(cmphi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x16_sll(simd256_avx_t _a, int _count)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i shiftedlo  = _mm_slli_epi16(aloi, _count);
		const __m128i shiftedhi  = _mm_slli_epi16(ahii, _count);
		const __m128  rlo    = _mm_castsi128_ps(shiftedlo);
		const __m128  rhi    = _mm_castsi128_ps(shiftedhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_x16_srl(simd256_avx_t _a, int _count)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i shiftedlo  = _mm_srli_epi16(aloi, _count);
		const __m128i shiftedhi  = _mm_srli_epi16(ahii, _count);
		const __m128  rlo    = _mm_castsi128_ps(shiftedlo);
		const __m128  rhi    = _mm_castsi128_ps(shiftedhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i8_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i sumlo  = _mm_add_epi8(aloi, bloi);
		const __m128i sumhi  = _mm_add_epi8(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(sumlo);
		const __m128  rhi    = _mm_castsi128_ps(sumhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i8_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i difflo  = _mm_sub_epi8(aloi, bloi);
		const __m128i diffhi  = _mm_sub_epi8(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(difflo);
		const __m128  rhi    = _mm_castsi128_ps(diffhi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u8_satadd(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i satlo  = _mm_adds_epu8(aloi, bloi);
		const __m128i sathi  = _mm_adds_epu8(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(satlo);
		const __m128  rhi    = _mm_castsi128_ps(sathi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u8_satsub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i satlo  = _mm_subs_epu8(aloi, bloi);
		const __m128i sathi  = _mm_subs_epu8(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(satlo);
		const __m128  rhi    = _mm_castsi128_ps(sathi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u16_satadd(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i satlo  = _mm_adds_epu16(aloi, bloi);
		const __m128i sathi  = _mm_adds_epu16(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(satlo);
		const __m128  rhi    = _mm_castsi128_ps(sathi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u16_satsub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m128  alo    = _mm256_castps256_ps128(_a);
		const __m128  ahi    = _mm256_extractf128_ps(_a, 1);
		const __m128  blo    = _mm256_castps256_ps128(_b);
		const __m128  bhi    = _mm256_extractf128_ps(_b, 1);
		const __m128i aloi   = _mm_castps_si128(alo);
		const __m128i ahii   = _mm_castps_si128(ahi);
		const __m128i bloi   = _mm_castps_si128(blo);
		const __m128i bhii   = _mm_castps_si128(bhi);
		const __m128i satlo  = _mm_subs_epu16(aloi, bloi);
		const __m128i sathi  = _mm_subs_epu16(ahii, bhii);
		const __m128  rlo    = _mm_castsi128_ps(satlo);
		const __m128  rhi    = _mm_castsi128_ps(sathi);
		const __m256  rlo256 = _mm256_castps128_ps256(rlo);
		const __m256  result = _mm256_insertf128_ps(rlo256, rhi, 1);
		return result;
	}

#endif // BX_SIMD_AVX2

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_add_pd(a, b);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_sub_pd(a, b);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_mul(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_mul_pd(a, b);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_div(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_div_pd(a, b);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_min(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_min_pd(a, b);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_max(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_max_pd(a, b);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_madd(simd256_avx_t _a, simd256_avx_t _b, simd256_avx_t _c)
	{
		return simd_f64_madd_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_nmsub(simd256_avx_t _a, simd256_avx_t _b, simd256_avx_t _c)
	{
		return simd_f64_nmsub_ni(_a, _b, _c);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_neg(simd256_avx_t _a)
	{
		const __m256i signi = _mm256_set1_epi64x(int64_t(kDoubleSignMask) );
		const __m256d signd = _mm256_castsi256_pd(signi);
		const __m256d a     = _mm256_castps_pd(_a);
		const __m256d r     = _mm256_xor_pd(a, signd);
		return _mm256_castpd_ps(r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_abs(simd256_avx_t _a)
	{
		const __m256i maski = _mm256_set1_epi64x(int64_t(kDoubleExponentMask | kDoubleMantissaMask) );
		const __m256d maskd = _mm256_castsi256_pd(maski);
		const __m256d a     = _mm256_castps_pd(_a);
		const __m256d r     = _mm256_and_pd(a, maskd);
		return _mm256_castpd_ps(r);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_clamp(simd256_avx_t _a, simd256_avx_t _min, simd256_avx_t _max)
	{
		const simd256_avx_t maxed  = simd256_f64_max(_a, _min);
		const simd256_avx_t result = simd256_f64_min(maxed, _max);
		return result;
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_lerp(simd256_avx_t _a, simd256_avx_t _b, simd256_avx_t _s)
	{
		return simd_f64_lerp_ni(_a, _b, _s);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_rcp(simd256_avx_t _a)
	{
		return simd_f64_rcp_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_sqrt(simd256_avx_t _a)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d result = _mm256_sqrt_pd(a);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_rsqrt(simd256_avx_t _a)
	{
		return simd_f64_rsqrt_ni(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_round(simd256_avx_t _a)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d result = _mm256_round_pd(a, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_ceil(simd256_avx_t _a)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d result = _mm256_ceil_pd(a);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_floor(simd256_avx_t _a)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d result = _mm256_floor_pd(a);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_cmpeq(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_cmp_pd(a, b, _CMP_EQ_OQ);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_cmpneq(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_cmp_pd(a, b, _CMP_NEQ_OQ);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_cmplt(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_cmp_pd(a, b, _CMP_LT_OQ);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_cmple(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_cmp_pd(a, b, _CMP_LE_OQ);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_cmpgt(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_cmp_pd(a, b, _CMP_GT_OQ);
		return _mm256_castpd_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_f64_cmpge(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256d a      = _mm256_castps_pd(_a);
		const __m256d b      = _mm256_castps_pd(_b);
		const __m256d result = _mm256_cmp_pd(a, b, _CMP_GE_OQ);
		return _mm256_castpd_ps(result);
	}

#if BX_SIMD_AVX2

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i64_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a      = _mm256_castps_si256(_a);
		const __m256i b      = _mm256_castps_si256(_b);
		const __m256i result = _mm256_add_epi64(a, b);
		return _mm256_castsi256_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_i64_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a      = _mm256_castps_si256(_a);
		const __m256i b      = _mm256_castps_si256(_b);
		const __m256i result = _mm256_sub_epi64(a, b);
		return _mm256_castsi256_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u64_add(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a      = _mm256_castps_si256(_a);
		const __m256i b      = _mm256_castps_si256(_b);
		const __m256i result = _mm256_add_epi64(a, b);
		return _mm256_castsi256_ps(result);
	}

	template<>
	BX_SIMD_FORCE_INLINE simd256_avx_t simd256_u64_sub(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i a      = _mm256_castps_si256(_a);
		const __m256i b      = _mm256_castps_si256(_b);
		const __m256i result = _mm256_sub_epi64(a, b);
		return _mm256_castsi256_ps(result);
	}

#endif // BX_SIMD_AVX2

	template<>
	BX_SIMD_FORCE_INLINE bool simd256_test_any(simd256_avx_t _test)
	{
		return 0 != _mm256_movemask_ps(_test);
	}

	template<>
	BX_SIMD_FORCE_INLINE bool simd256_test_all(simd256_avx_t _test)
	{
		return 0xff == _mm256_movemask_ps(_test);
	}

	template<>
	BX_SIMD_FORCE_INLINE bool simd256_test_zero(simd256_avx_t _a, simd256_avx_t _b)
	{
		const __m256i ai = _mm256_castps_si256(_a);
		const __m256i bi = _mm256_castps_si256(_b);
		return 0 != _mm256_testz_si256(ai, bi);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd256_x32_signbitsmask(simd256_avx_t _a)
	{
		return _mm256_movemask_ps(_a);
	}

	template<>
	BX_SIMD_FORCE_INLINE int simd256_x8_signbitsmask(simd256_avx_t _a)
	{
#if BX_SIMD_AVX2
		const __m256i ai     = _mm256_castps_si256(_a);
		const int     result = _mm256_movemask_epi8(ai);
		return result;
#else
		const __m128  lo     = _mm256_castps256_ps128(_a);
		const __m128  hi     = _mm256_extractf128_ps(_a, 1);
		const __m128i loi    = _mm_castps_si128(lo);
		const __m128i hii    = _mm_castps_si128(hi);
		const int     maskLo = _mm_movemask_epi8(loi);
		const int     maskHi = _mm_movemask_epi8(hii);
		const int     result = maskLo | (maskHi << 16);
		return result;
#endif
	}

	typedef simd256_avx_t  simd256_t;

} // namespace bx
