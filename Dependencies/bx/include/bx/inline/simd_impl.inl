/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#	error "Must be included from bx/simd_t.h!"
#endif // BX_SIMD_T_H_HEADER_GUARD

namespace bx
{
	simd32_t simd32_ld(const void* _ptr);
	BX_CONSTEXPR_FUNC simd32_t simd32_ld(float _x);
	BX_CONSTEXPR_FUNC simd32_t simd32_ld(int32_t _x);
	BX_CONSTEXPR_FUNC simd32_t simd32_ld(uint32_t _x);
	void     simd32_st(void* _ptr, simd32_t _a);
	void     simd32_x32_st1(void* _ptr, simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_zero();

	BX_CONSTEXPR_FUNC simd32_t simd32_splat(float _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_splat(uint32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_splat(int32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_splat(uint16_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_splat(int16_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_splat(uint8_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_splat(int8_t _a);

	BX_CONSTEXPR_FUNC simd32_t simd32_f32_add(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_sub(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_mul(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_div(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_madd(simd32_t _a, simd32_t _b, simd32_t _c);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_msub(simd32_t _a, simd32_t _b, simd32_t _c);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_nmsub(simd32_t _a, simd32_t _b, simd32_t _c);
	BX_CONSTEXPR_FUNC int      simd32_x32_signbitsmask(simd32_t _a);
	BX_CONSTEXPR_FUNC int      simd32_x8_signbitsmask(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_x8_shuffle(simd32_t _a, simd32_t _indices);
	BX_CONSTEXPR_FUNC simd32_t simd32_x8_shuffle(simd32_t _a, simd32_t _b, simd32_t _indices);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_neg(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_abs(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_min(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_max(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_clamp(simd32_t _a, simd32_t _min, simd32_t _max);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_lerp(simd32_t _a, simd32_t _b, simd32_t _s);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_rcp(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_sqrt(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_rsqrt(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_round(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_ceil(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_floor(simd32_t _a);

	BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmpeq(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmpneq(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmplt(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmple(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmpgt(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_f32_cmpge(simd32_t _a, simd32_t _b);

	BX_CONSTEXPR_FUNC simd32_t simd32_u32_add(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_sub(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_mul(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_div(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_mod(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_min(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_max(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_clamp(simd32_t _a, simd32_t _min, simd32_t _max);

	BX_CONSTEXPR_FUNC simd32_t simd32_i32_add(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i32_sub(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i32_mul(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i32_neg(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_i32_abs(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_i32_min(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i32_max(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i32_clamp(simd32_t _a, simd32_t _min, simd32_t _max);

	BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmpeq(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmpneq(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmplt(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmple(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmpgt(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u32_cmpge(simd32_t _a, simd32_t _b);

	BX_CONSTEXPR_FUNC simd32_t simd32_i32_cmpeq(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i32_cmplt(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i32_cmpgt(simd32_t _a, simd32_t _b);

	BX_CONSTEXPR_FUNC simd32_t simd32_i16_add(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i16_sub(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i16_cmpeq(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_x16_sll(simd32_t _a, int _count);
	BX_CONSTEXPR_FUNC simd32_t simd32_x16_srl(simd32_t _a, int _count);
	BX_CONSTEXPR_FUNC simd32_t simd32_i8_add(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_i8_sub(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u8_satadd(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u8_satsub(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u16_satadd(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_u16_satsub(simd32_t _a, simd32_t _b);

	BX_CONSTEXPR_FUNC simd32_t simd32_and(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_andc(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_or(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_orc(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_xor(simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_not(simd32_t _a);
	BX_CONSTEXPR_FUNC simd32_t simd32_x32_sll(simd32_t _a, int _count);
	BX_CONSTEXPR_FUNC simd32_t simd32_x32_srl(simd32_t _a, int _count);
	BX_CONSTEXPR_FUNC simd32_t simd32_x32_sra(simd32_t _a, int _count);
	BX_CONSTEXPR_FUNC simd32_t simd32_x32_sll(simd32_t _a, simd32_t _count);
	BX_CONSTEXPR_FUNC simd32_t simd32_x32_srl(simd32_t _a, simd32_t _count);
	BX_CONSTEXPR_FUNC simd32_t simd32_x32_sra(simd32_t _a, simd32_t _count);

	BX_CONSTEXPR_FUNC simd32_t simd32_selb(simd32_t _mask, simd32_t _a, simd32_t _b);
	BX_CONSTEXPR_FUNC simd32_t simd32_sels(simd32_t _test, simd32_t _a, simd32_t _b);

	BX_CONSTEXPR_FUNC bool simd32_test(simd32_t _test);

	simd64_t simd64_ld(const void* _ptr);
	BX_CONSTEXPR_FUNC simd64_t simd64_ld(float _x, float _y);
	BX_CONSTEXPR_FUNC simd64_t simd64_ld(int32_t _x, int32_t _y);
	BX_CONSTEXPR_FUNC simd64_t simd64_ld(uint32_t _x, uint32_t _y);
	void     simd64_st(void* _ptr, simd64_t _a);
	void     simd64_x32_st1(void* _ptr, simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_zero();
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(float _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(uint32_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(int32_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(uint64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(int64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(double _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(uint16_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(int16_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(uint8_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_splat(int8_t _a);

	BX_CONSTEXPR_FUNC simd64_t simd64_f64_add(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_sub(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_mul(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_div(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_madd(simd64_t _a, simd64_t _b, simd64_t _c);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_nmsub(simd64_t _a, simd64_t _b, simd64_t _c);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_neg(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_abs(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_min(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_max(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_clamp(simd64_t _a, simd64_t _min, simd64_t _max);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_lerp(simd64_t _a, simd64_t _b, simd64_t _s);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_rcp(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_sqrt(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_rsqrt(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_round(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_ceil(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_floor(simd64_t _a);

	BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmpeq(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmpneq(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmplt(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmple(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmpgt(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmpge(simd64_t _a, simd64_t _b);

	BX_CONSTEXPR_FUNC simd64_t simd64_f32_add(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_sub(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_mul(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_div(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_madd(simd64_t _a, simd64_t _b, simd64_t _c);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_msub(simd64_t _a, simd64_t _b, simd64_t _c);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_nmsub(simd64_t _a, simd64_t _b, simd64_t _c);
	BX_CONSTEXPR_FUNC int      simd64_x32_signbitsmask(simd64_t _a);
	BX_CONSTEXPR_FUNC int      simd64_x8_signbitsmask(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_x8_shuffle(simd64_t _a, simd64_t _indices);
	BX_CONSTEXPR_FUNC simd64_t simd64_x8_shuffle(simd64_t _a, simd64_t _b, simd64_t _indices);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_neg(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_abs(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_min(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_max(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_clamp(simd64_t _a, simd64_t _min, simd64_t _max);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_lerp(simd64_t _a, simd64_t _b, simd64_t _s);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_rcp(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_sqrt(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_rsqrt(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_round(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_ceil(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_floor(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmpeq(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmpneq(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmplt(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmple(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmpgt(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmpge(simd64_t _a, simd64_t _b);

	BX_CONSTEXPR_FUNC simd64_t simd64_i32_add(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i32_sub(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i32_neg(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_i32_abs(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_i32_min(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i32_max(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i32_clamp(simd64_t _a, simd64_t _min, simd64_t _max);
	BX_CONSTEXPR_FUNC simd64_t simd64_i32_cmpeq(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i32_cmplt(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i32_cmpgt(simd64_t _a, simd64_t _b);

	BX_CONSTEXPR_FUNC simd64_t simd64_u32_add(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_sub(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_mul(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_min(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_max(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_clamp(simd64_t _a, simd64_t _min, simd64_t _max);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmpeq(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmpneq(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmplt(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmple(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmpgt(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmpge(simd64_t _a, simd64_t _b);

	BX_CONSTEXPR_FUNC simd64_t simd64_i16_add(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i16_sub(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i16_mullo(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i16_cmpeq(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_x16_sll(simd64_t _a, int _count);
	BX_CONSTEXPR_FUNC simd64_t simd64_x16_srl(simd64_t _a, int _count);
	BX_CONSTEXPR_FUNC simd64_t simd64_i8_add(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i8_sub(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u8_satadd(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u8_satsub(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u16_satadd(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u16_satsub(simd64_t _a, simd64_t _b);

	BX_CONSTEXPR_FUNC simd64_t simd64_i64_add(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_i64_sub(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u64_add(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u64_sub(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_u64_mul(simd64_t _a, simd64_t _b);

	BX_CONSTEXPR_FUNC simd64_t simd64_and(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_andc(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_or(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_orc(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_xor(simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_not(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_x64_sll(simd64_t _a, int _count);
	BX_CONSTEXPR_FUNC simd64_t simd64_x64_srl(simd64_t _a, int _count);
	BX_CONSTEXPR_FUNC simd64_t simd64_x64_sra(simd64_t _a, int _count);
	BX_CONSTEXPR_FUNC simd64_t simd64_x64_rol(simd64_t _a, int _count);
	BX_CONSTEXPR_FUNC simd64_t simd64_x64_ror(simd64_t _a, int _count);
	BX_CONSTEXPR_FUNC simd64_t simd64_x64_cntbits(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_x64_cntlz(simd64_t _a);
	BX_CONSTEXPR_FUNC simd64_t simd64_x64_cnttz(simd64_t _a);

	BX_CONSTEXPR_FUNC simd64_t simd64_selb(simd64_t _mask, simd64_t _a, simd64_t _b);
	BX_CONSTEXPR_FUNC simd64_t simd64_sels(simd64_t _test, simd64_t _a, simd64_t _b);

	BX_CONSTEXPR_FUNC bool simd64_test_any_x(simd64_t _test);
	BX_CONSTEXPR_FUNC bool simd64_test_any_y(simd64_t _test);
	BX_CONSTEXPR_FUNC bool simd64_test_any_xy(simd64_t _test);
	BX_CONSTEXPR_FUNC bool simd64_test_all_x(simd64_t _test);
	BX_CONSTEXPR_FUNC bool simd64_test_all_y(simd64_t _test);
	BX_CONSTEXPR_FUNC bool simd64_test_all_xy(simd64_t _test);

	/// Load 128-bit register from aligned memory.
	///
	template<typename Ty>
	Ty simd128_ld(const void* _ptr);

	/// Load 128-bit register from unaligned memory.
	///
	template<typename Ty>
	Ty simd128_ldu(const void* _ptr);

	/// Store 128-bit register to aligned memory.
	///
	template<typename Ty>
	void simd128_st(void* _ptr, Ty _a);

	/// Store 128-bit register to unaligned memory.
	///
	template<typename Ty>
	void simd128_stu(void* _ptr, Ty _a);

	/// Store lowest 32-bit element to memory.
	///
	template<typename Ty>
	void simd128_x32_st1(void* _ptr, Ty _a);

	/// Non-temporal (streaming) store.
	///
	template<typename Ty>
	void simd128_stream(void* _ptr, Ty _a);

	/// Splat 32-bit value to all lanes.
	///
	template<typename Ty> Ty simd128_splat(float _a);

	/// Splat 32-bit value to all lanes.
	///
	template<typename Ty> Ty simd128_splat(int32_t _a);

	/// Splat 32-bit value to all lanes.
	///
	template<typename Ty> Ty simd128_splat(uint32_t _a);

	/// Splat 64-bit double value to two lanes.
	///
	template<typename Ty> Ty simd128_splat(double _a);

	/// Construct from 4 floats.
	///
	template<typename Ty>
	Ty simd128_ld(float _x, float _y, float _z, float _w);

	/// Construct from 4 int32s (as bit pattern).
	///
	template<typename Ty>
	Ty simd128_ld(int32_t _x, int32_t _y, int32_t _z, int32_t _w);

	/// Construct from 4 uint32s (as bit pattern).
	///
	template<typename Ty>
	Ty simd128_ld(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w);

	/// Return all-zero register.
	///
	template<typename Ty>
	Ty simd128_zero();

	/// Convert 4xi32 -> 4xf32.
	///
	template<typename Ty>
	Ty simd128_i32_itof(Ty _a);

	/// Convert 4xf32 -> 4xi32 (truncation toward zero).
	///
	template<typename Ty>
	Ty simd128_f32_ftoi_trunc(Ty _a);

	/// Convert 4xf32 -> 4xi32 (round-to-nearest, ties to even).
	///
	template<typename Ty>
	Ty simd128_f32_ftoi_round(Ty _a);

	/// 4xf32 add.
	///
	template<typename Ty>
	Ty simd128_f32_add(Ty _a, Ty _b);

	/// 4xf32 subtract.
	///
	template<typename Ty>
	Ty simd128_f32_sub(Ty _a, Ty _b);

	/// 4xf32 multiply.
	///
	template<typename Ty>
	Ty simd128_f32_mul(Ty _a, Ty _b);

	/// 4xf32 divide.
	///
	template<typename Ty>
	Ty simd128_f32_div(Ty _a, Ty _b);

	/// 4xf32 fused multiply-add: _a * _b + _c
	///
	template<typename Ty>
	Ty simd128_f32_madd(Ty _a, Ty _b, Ty _c);

	/// 4xf32 fused multiply-subtract: _a * _b - _c
	///
	template<typename Ty>
	Ty simd128_f32_msub(Ty _a, Ty _b, Ty _c);

	/// 4xf32 negate-multiply-subtract: _c - _a * _b
	///
	template<typename Ty>
	Ty simd128_f32_nmsub(Ty _a, Ty _b, Ty _c);

	/// 4x32 extract sign bits into int bitmask (bit N = sign of lane N).
	///
	template<typename Ty>
	int simd128_x32_signbitsmask(Ty _a);

	/// 16x8 extract sign bits into int bitmask (bit N = sign of byte N).
	///
	template<typename Ty>
	int simd128_x8_signbitsmask(Ty _a);

	/// Per-byte shuffle within 16-byte lane (single-source).
	///
	template<typename Ty>
	Ty simd128_x8_shuffle(Ty _a, Ty _indices);

	/// Per-byte shuffle within 16-byte lane (two-source).
	///
	template<typename Ty>
	Ty simd128_x8_shuffle(Ty _a, Ty _b, Ty _indices);

	/// 4xf32 negate.
	///
	template<typename Ty>
	Ty simd128_f32_neg(Ty _a);

	/// 4xf32 absolute value.
	///
	template<typename Ty>
	Ty simd128_f32_abs(Ty _a);

	/// 4xf32 min.
	///
	template<typename Ty>
	Ty simd128_f32_min(Ty _a, Ty _b);

	/// 4xf32 max.
	///
	template<typename Ty>
	Ty simd128_f32_max(Ty _a, Ty _b);

	/// 4xf32 clamp: max(min(_a, _max), _min).
	///
	template<typename Ty>
	Ty simd128_f32_clamp(Ty _a, Ty _min, Ty _max);

	/// 4xf32 linear interpolation: _a + (_b - _a) * _s.
	///
	template<typename Ty>
	Ty simd128_f32_lerp(Ty _a, Ty _b, Ty _s);

	/// 4xf32 reciprocal estimate.
	///
	template<typename Ty>
	Ty simd128_f32_rcp_est(Ty _a);

	/// 4xf32 reciprocal (refined).
	///
	template<typename Ty>
	Ty simd128_f32_rcp(Ty _a);

	/// 4xf32 square root.
	///
	template<typename Ty>
	Ty simd128_f32_sqrt(Ty _a);

	/// 4xf32 sqrt Newton-Raphson.
	///
	template<typename Ty>
	Ty simd128_f32_sqrt_nr(Ty _a);

	/// 4xf32 reciprocal-sqrt estimate.
	///
	template<typename Ty>
	Ty simd128_f32_rsqrt_est(Ty _a);

	/// 4xf32 reciprocal-sqrt (refined).
	///
	template<typename Ty>
	Ty simd128_f32_rsqrt(Ty _a);

	/// 4xf32 reciprocal-sqrt Newton-Raphson.
	///
	template<typename Ty>
	Ty simd128_f32_rsqrt_nr(Ty _a);

	/// 4xf32 reciprocal-sqrt Carmack (fast inverse square root).
	///
	template<typename Ty>
	Ty simd128_f32_rsqrt_carmack(Ty _a);

	/// 4xf32 div Newton-Raphson.
	///
	template<typename Ty>
	Ty simd128_f32_div_nr(Ty _a, Ty _b);

	/// 4xf32 round to nearest integer.
	///
	template<typename Ty>
	Ty simd128_f32_round(Ty _a);

	/// 4xf32 ceil.
	///
	template<typename Ty>
	Ty simd128_f32_ceil(Ty _a);

	/// 4xf32 floor.
	///
	template<typename Ty>
	Ty simd128_f32_floor(Ty _a);

	/// 4xf32 compare equal (returns mask).
	///
	template<typename Ty>
	Ty simd128_f32_cmpeq(Ty _a, Ty _b);

	/// 4xf32 compare not-equal.
	///
	template<typename Ty>
	Ty simd128_f32_cmpneq(Ty _a, Ty _b);

	/// 4xf32 compare less-than.
	///
	template<typename Ty>
	Ty simd128_f32_cmplt(Ty _a, Ty _b);

	/// 4xf32 compare less-or-equal.
	///
	template<typename Ty>
	Ty simd128_f32_cmple(Ty _a, Ty _b);

	/// 4xf32 compare greater-than.
	///
	template<typename Ty>
	Ty simd128_f32_cmpgt(Ty _a, Ty _b);

	/// 4xf32 compare greater-or-equal.
	///
	template<typename Ty>
	Ty simd128_f32_cmpge(Ty _a, Ty _b);

	/// Extract X (lane 0) as float.
	///
	template<typename Ty>
	float simd128_f32_x(Ty _a);

	/// Extract Y (lane 1) as float.
	///
	template<typename Ty>
	float simd128_f32_y(Ty _a);

	/// Extract Z (lane 2) as float.
	///
	template<typename Ty>
	float simd128_f32_z(Ty _a);

	/// Extract W (lane 3) as float.
	///
	template<typename Ty>
	float simd128_f32_w(Ty _a);

	/// 3-component dot product (result broadcast).
	///
	template<typename Ty>
	Ty simd128_f32_dot3(Ty _a, Ty _b);

	/// 4-component dot product (result broadcast).
	///
	template<typename Ty>
	Ty simd128_f32_dot(Ty _a, Ty _b);

	/// 3-component cross product.
	///
	template<typename Ty>
	Ty simd128_f32_cross3(Ty _a, Ty _b);

	/// 3-component normalize.
	///
	template<typename Ty>
	Ty simd128_f32_normalize3(Ty _a);

	/// 4xi32 add.
	///
	template<typename Ty>
	Ty simd128_i32_add(Ty _a, Ty _b);

	/// 4xi32 subtract.
	///
	template<typename Ty>
	Ty simd128_i32_sub(Ty _a, Ty _b);

	/// 4xi32 negate.
	///
	template<typename Ty>
	Ty simd128_i32_neg(Ty _a);

	/// 4xi32 absolute value.
	///
	template<typename Ty>
	Ty simd128_i32_abs(Ty _a);

	/// 4xi32 min.
	///
	template<typename Ty>
	Ty simd128_i32_min(Ty _a, Ty _b);

	/// 4xi32 max.
	///
	template<typename Ty>
	Ty simd128_i32_max(Ty _a, Ty _b);

	/// 4xi32 clamp.
	///
	template<typename Ty>
	Ty simd128_i32_clamp(Ty _a, Ty _min, Ty _max);

	/// 4xu32 add.
	///
	template<typename Ty>
	Ty simd128_u32_add(Ty _a, Ty _b);

	/// 4xu32 subtract.
	///
	template<typename Ty>
	Ty simd128_u32_sub(Ty _a, Ty _b);

	/// 4xu32 multiply.
	///
	template<typename Ty>
	Ty simd128_u32_mul(Ty _a, Ty _b);

	/// 4xu32 min.
	///
	template<typename Ty>
	Ty simd128_u32_min(Ty _a, Ty _b);

	/// 4xu32 max.
	///
	template<typename Ty>
	Ty simd128_u32_max(Ty _a, Ty _b);

	/// 4xu32 clamp.
	///
	template<typename Ty>
	Ty simd128_u32_clamp(Ty _a, Ty _min, Ty _max);

	/// 4xu32 compare equal.
	///
	template<typename Ty>
	Ty simd128_u32_cmpeq(Ty _a, Ty _b);

	/// 4xu32 compare less-than.
	///
	template<typename Ty>
	Ty simd128_u32_cmplt(Ty _a, Ty _b);

	/// 4xu32 compare greater-than.
	///
	template<typename Ty>
	Ty simd128_u32_cmpgt(Ty _a, Ty _b);

	/// 4xi32 compare equal.
	///
	template<typename Ty>
	Ty simd128_i32_cmpeq(Ty _a, Ty _b);

	/// 4xi32 compare less-than.
	///
	template<typename Ty>
	Ty simd128_i32_cmplt(Ty _a, Ty _b);

	/// 4xi32 compare greater-than.
	///
	template<typename Ty>
	Ty simd128_i32_cmpgt(Ty _a, Ty _b);

	/// 8xi16 add.
	///
	template<typename Ty>
	Ty simd128_i16_add(Ty _a, Ty _b);

	/// 8xi16 subtract.
	///
	template<typename Ty>
	Ty simd128_i16_sub(Ty _a, Ty _b);

	/// 8xi16 multiply low (keep low 16 bits of 32-bit result).
	///
	template<typename Ty>
	Ty simd128_i16_mullo(Ty _a, Ty _b);

	/// 8xi16 compare equal.
	///
	template<typename Ty>
	Ty simd128_i16_cmpeq(Ty _a, Ty _b);

	/// 128-bit shift left logical (per-16-bit lane).
	///
	template<typename Ty>
	Ty simd128_x16_sll(Ty _a, int _count);

	/// 128-bit shift right logical (per-16-bit lane).
	///
	template<typename Ty>
	Ty simd128_x16_srl(Ty _a, int _count);

	/// Splat signed 16-bit value to all lanes.
	///
	template<typename Ty> Ty simd128_splat(int16_t _a);

	/// Splat unsigned 16-bit value to all lanes.
	///
	template<typename Ty> Ty simd128_splat(uint16_t _a);

	/// 16xi8 add.
	///
	template<typename Ty>
	Ty simd128_i8_add(Ty _a, Ty _b);

	/// 16xi8 subtract.
	///
	template<typename Ty>
	Ty simd128_i8_sub(Ty _a, Ty _b);

	/// 16xu8 saturating add.
	///
	template<typename Ty>
	Ty simd128_u8_satadd(Ty _a, Ty _b);

	/// 16xu8 saturating subtract.
	///
	template<typename Ty>
	Ty simd128_u8_satsub(Ty _a, Ty _b);

	/// 8xu16 saturating add.
	///
	template<typename Ty>
	Ty simd128_u16_satadd(Ty _a, Ty _b);

	/// 8xu16 saturating subtract.
	///
	template<typename Ty>
	Ty simd128_u16_satsub(Ty _a, Ty _b);

	/// 128-bit bitwise AND.
	///
	template<typename Ty>
	Ty simd128_and(Ty _a, Ty _b);

	/// 128-bit AND-NOT: _a & ~_b.
	///
	template<typename Ty>
	Ty simd128_andc(Ty _a, Ty _b);

	/// 128-bit bitwise OR.
	///
	template<typename Ty>
	Ty simd128_or(Ty _a, Ty _b);

	/// 128-bit OR complement: ~_a | _b.
	///
	template<typename Ty>
	Ty simd128_orc(Ty _a, Ty _b);

	/// 128-bit bitwise XOR.
	///
	template<typename Ty>
	Ty simd128_xor(Ty _a, Ty _b);

	/// 128-bit bitwise NOT.
	///
	template<typename Ty>
	Ty simd128_not(Ty _a);

	/// 128-bit shift left logical (per-32-bit lane, immediate count).
	///
	template<typename Ty>
	Ty simd128_x32_sll(Ty _a, int _count);

	/// 128-bit shift left logical (per-32-bit lane, per-lane variable count).
	///
	template<typename Ty>
	Ty simd128_x32_sll(Ty _a, Ty _count);

	/// 128-bit shift right logical (per-32-bit lane).
	///
	template<typename Ty>
	Ty simd128_x32_srl(Ty _a, int _count);

	/// 128-bit shift right logical (per-32-bit lane, per-lane variable count).
	///
	template<typename Ty>
	Ty simd128_x32_srl(Ty _a, Ty _count);

	/// 128-bit shift right arithmetic (per-32-bit lane).
	///
	template<typename Ty>
	Ty simd128_x32_sra(Ty _a, int _count);

	/// 128-bit shift right arithmetic (per-32-bit lane, per-lane variable count).
	///
	template<typename Ty>
	Ty simd128_x32_sra(Ty _a, Ty _count);

	/// Select by bitmask: for each bit, result = mask ? _a : _b.
	///
	template<typename Ty>
	Ty simd128_selb(Ty _mask, Ty _a, Ty _b);

	/// Select by sign bit: for each lane, result = (test < 0) ? _a : _b.
	///
	template<typename Ty>
	Ty simd128_sels(Ty _test, Ty _a, Ty _b);

	/// OR-reduce all lanes into lane 0.
	///
	template<typename Ty>
	Ty simd128_orx(Ty _a);

#define ELEMx 0
#define ELEMy 1
#define ELEMz 2
#define ELEMw 3

#define BX_SIMD128_IMPLEMENT_SWIZZLE(_x, _y, _z, _w) \
			template<typename Ty>                     \
			Ty simd128_x32_swiz_##_x##_y##_z##_w(Ty _a);

#include "simd128_swizzle.inl"

#undef BX_SIMD128_IMPLEMENT_SWIZZLE
#undef ELEMw
#undef ELEMz
#undef ELEMy
#undef ELEMx

	/// Shuffle: [a.x, a.y, b.x, b.y]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_xyAB(Ty _a, Ty _b);

	/// Shuffle: [b.x, b.y, a.x, a.y]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_ABxy(Ty _a, Ty _b);

	/// Shuffle: [b.z, b.w, a.z, a.w]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_CDzw(Ty _a, Ty _b);

	/// Shuffle: [a.z, a.w, b.z, b.w]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_zwCD(Ty _a, Ty _b);

	/// Shuffle: [a.x, b.x, a.y, b.y]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_xAyB(Ty _a, Ty _b);

	/// Shuffle: [b.x, a.x, b.y, a.y]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_AxBy(Ty _a, Ty _b);

	/// Shuffle: [a.z, b.z, a.w, b.w]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_zCwD(Ty _a, Ty _b);

	/// Shuffle: [b.z, a.z, b.w, a.w]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_CzDw(Ty _a, Ty _b);

	/// Shuffle: [a.x, b.x, a.z, b.z]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_xAzC(Ty _a, Ty _b);

	/// Shuffle: [a.y, b.y, a.w, b.w]
	///
	template<typename Ty>
	Ty simd128_x32_shuf_yBwD(Ty _a, Ty _b);

	/// Shuffle: [a.x, a.z, b.x, b.z] (PSHUFPS imm 0x88, even-lane deinterleave)
	///
	template<typename Ty>
	Ty simd128_x32_shuf_xzAC(Ty _a, Ty _b);

	/// Shuffle: [a.y, a.w, b.y, b.w] (PSHUFPS imm 0xDD, odd-lane deinterleave)
	///
	template<typename Ty>
	Ty simd128_x32_shuf_ywBD(Ty _a, Ty _b);

	/// Shuffle: [a.x, a.x, b.x, b.x] (PSHUFPS imm 0x00)
	///
	template<typename Ty>
	Ty simd128_x32_shuf_xxAA(Ty _a, Ty _b);

	/// Shuffle: [a.y, a.y, b.y, b.y] (PSHUFPS imm 0x55)
	///
	template<typename Ty>
	Ty simd128_x32_shuf_yyBB(Ty _a, Ty _b);

	/// Shuffle: [a.z, a.z, b.z, b.z] (PSHUFPS imm 0xAA)
	///
	template<typename Ty>
	Ty simd128_x32_shuf_zzCC(Ty _a, Ty _b);

	/// Shuffle: [a.w, a.w, b.w, b.w] (PSHUFPS imm 0xFF)
	///
	template<typename Ty>
	Ty simd128_x32_shuf_wwDD(Ty _a, Ty _b);

#define BX_SIMD128_IMPLEMENT_TEST(_xyzw) \
		template<typename Ty>            \
		BX_SIMD_FORCE_INLINE bool simd128_test_any_##_xyzw(Ty _test); \
		                                                               \
		template<typename Ty>                                          \
		BX_SIMD_FORCE_INLINE bool simd128_test_all_##_xyzw(Ty _test);

	BX_SIMD128_IMPLEMENT_TEST(x   );
	BX_SIMD128_IMPLEMENT_TEST(y   );
	BX_SIMD128_IMPLEMENT_TEST(xy  );
	BX_SIMD128_IMPLEMENT_TEST(z   );
	BX_SIMD128_IMPLEMENT_TEST(xz  );
	BX_SIMD128_IMPLEMENT_TEST(yz  );
	BX_SIMD128_IMPLEMENT_TEST(xyz );
	BX_SIMD128_IMPLEMENT_TEST(w   );
	BX_SIMD128_IMPLEMENT_TEST(xw  );
	BX_SIMD128_IMPLEMENT_TEST(yw  );
	BX_SIMD128_IMPLEMENT_TEST(xyw );
	BX_SIMD128_IMPLEMENT_TEST(zw  );
	BX_SIMD128_IMPLEMENT_TEST(xzw );
	BX_SIMD128_IMPLEMENT_TEST(yzw );
	BX_SIMD128_IMPLEMENT_TEST(xyzw);

#undef BX_SIMD128_IMPLEMENT_TEST

	template<typename Ty> bool simd128_test_zero(Ty _a, Ty _b);

	template<typename Ty> Ty simd128_f64_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_sub(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_mul(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_div(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_min(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_max(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_madd(Ty _a, Ty _b, Ty _c);
	template<typename Ty> Ty simd128_f64_nmsub(Ty _a, Ty _b, Ty _c);
	template<typename Ty> Ty simd128_f64_neg(Ty _a);
	template<typename Ty> Ty simd128_f64_abs(Ty _a);
	template<typename Ty> Ty simd128_f64_clamp(Ty _a, Ty _min, Ty _max);
	template<typename Ty> Ty simd128_f64_lerp(Ty _a, Ty _b, Ty _s);
	template<typename Ty> Ty simd128_f64_rcp(Ty _a);
	template<typename Ty> Ty simd128_f64_sqrt(Ty _a);
	template<typename Ty> Ty simd128_f64_rsqrt(Ty _a);
	template<typename Ty> Ty simd128_f64_round(Ty _a);
	template<typename Ty> Ty simd128_f64_ceil(Ty _a);
	template<typename Ty> Ty simd128_f64_floor(Ty _a);
	template<typename Ty> Ty simd128_f64_cmpeq(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_cmpneq(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_cmplt(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_cmple(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_cmpgt(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_f64_cmpge(Ty _a, Ty _b);

	template<typename Ty> Ty simd128_i64_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_i64_sub(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_u64_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd128_u64_sub(Ty _a, Ty _b);

	template<typename Ty> bool simd256_test_any(Ty _test);
	template<typename Ty> bool simd256_test_all(Ty _test);
	template<typename Ty> bool simd256_test_zero(Ty _a, Ty _b);

	template<typename Ty> Ty   simd256_ld(const void* _ptr);
	template<typename Ty> Ty   simd256_ldu(const void* _ptr);
	template<typename Ty> void simd256_st(void* _ptr, Ty _a);
	template<typename Ty> void simd256_stu(void* _ptr, Ty _a);
	template<typename Ty> void simd256_x32_st1(void* _ptr, Ty _a);
	template<typename Ty> void simd256_stream(void* _ptr, Ty _a);

	template<typename Ty> Ty simd256_splat(float _a);
	template<typename Ty> Ty simd256_splat(int32_t _a);
	template<typename Ty> Ty simd256_splat(uint32_t _a);
	template<typename Ty> Ty simd256_splat(double _a);
	template<typename Ty> Ty simd256_splat(int16_t _a);
	template<typename Ty> Ty simd256_splat(uint16_t _a);
	template<typename Ty> Ty simd256_ld(float _x0, float _x1, float _x2, float _x3, float _x4, float _x5, float _x6, float _x7);
	template<typename Ty> Ty simd256_ld(int32_t _x0, int32_t _x1, int32_t _x2, int32_t _x3, int32_t _x4, int32_t _x5, int32_t _x6, int32_t _x7);
	template<typename Ty> Ty simd256_ld(uint32_t _x0, uint32_t _x1, uint32_t _x2, uint32_t _x3, uint32_t _x4, uint32_t _x5, uint32_t _x6, uint32_t _x7);
	template<typename Ty> Ty simd256_zero();

	template<typename Ty> Ty simd256_i32_itof(Ty _a);
	template<typename Ty> Ty simd256_f32_ftoi_trunc(Ty _a);
	template<typename Ty> Ty simd256_f32_ftoi_round(Ty _a);

	template<typename Ty> Ty simd256_f32_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_sub(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_mul(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_div(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_madd(Ty _a, Ty _b, Ty _c);
	template<typename Ty> Ty simd256_f32_msub(Ty _a, Ty _b, Ty _c);
	template<typename Ty> Ty simd256_f32_nmsub(Ty _a, Ty _b, Ty _c);
	template<typename Ty> int simd256_x32_signbitsmask(Ty _a);
	template<typename Ty> int simd256_x8_signbitsmask(Ty _a);
	template<typename Ty> Ty simd256_x8_shuffle(Ty _a, Ty _indices);
	template<typename Ty> Ty simd256_x8_shuffle(Ty _a, Ty _b, Ty _indices);
	template<typename Ty> Ty simd256_f32_neg(Ty _a);
	template<typename Ty> Ty simd256_f32_abs(Ty _a);
	template<typename Ty> Ty simd256_f32_min(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_max(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_clamp(Ty _a, Ty _min, Ty _max);
	template<typename Ty> Ty simd256_f32_lerp(Ty _a, Ty _b, Ty _s);
	template<typename Ty> Ty simd256_f32_rcp_est(Ty _a);
	template<typename Ty> Ty simd256_f32_rcp(Ty _a);
	template<typename Ty> Ty simd256_f32_sqrt(Ty _a);
	template<typename Ty> Ty simd256_f32_rsqrt_est(Ty _a);
	template<typename Ty> Ty simd256_f32_rsqrt(Ty _a);
	template<typename Ty> Ty simd256_f32_round(Ty _a);
	template<typename Ty> Ty simd256_f32_ceil(Ty _a);
	template<typename Ty> Ty simd256_f32_floor(Ty _a);
	template<typename Ty> Ty simd256_f32_cmpeq(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_cmpneq(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_cmplt(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_cmple(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_cmpgt(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f32_cmpge(Ty _a, Ty _b);

	template<typename Ty> Ty simd256_i32_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i32_sub(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i32_neg(Ty _a);
	template<typename Ty> Ty simd256_i32_abs(Ty _a);
	template<typename Ty> Ty simd256_i32_min(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i32_max(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i32_clamp(Ty _a, Ty _min, Ty _max);

	template<typename Ty> Ty simd256_u32_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u32_sub(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u32_mul(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u32_min(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u32_max(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u32_clamp(Ty _a, Ty _min, Ty _max);
	template<typename Ty> Ty simd256_u32_cmpeq(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u32_cmplt(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u32_cmpgt(Ty _a, Ty _b);

	template<typename Ty> Ty simd256_i32_cmpeq(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i32_cmplt(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i32_cmpgt(Ty _a, Ty _b);

	template<typename Ty> Ty simd256_i16_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i16_sub(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i16_mullo(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i16_cmpeq(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_x16_sll(Ty _a, int _count);
	template<typename Ty> Ty simd256_x16_srl(Ty _a, int _count);

	template<typename Ty> Ty simd256_i8_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i8_sub(Ty _a, Ty _b);

	template<typename Ty> Ty simd256_u8_satadd(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u8_satsub(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u16_satadd(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u16_satsub(Ty _a, Ty _b);

	template<typename Ty> Ty simd256_and(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_andc(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_or(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_orc(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_xor(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_not(Ty _a);
	template<typename Ty> Ty simd256_x32_sll(Ty _a, int _count);
	template<typename Ty> Ty simd256_x32_sll(Ty _a, Ty _count);
	template<typename Ty> Ty simd256_x32_srl(Ty _a, int _count);
	template<typename Ty> Ty simd256_x32_srl(Ty _a, Ty _count);
	template<typename Ty> Ty simd256_x32_sra(Ty _a, int _count);
	template<typename Ty> Ty simd256_x32_sra(Ty _a, Ty _count);

	template<typename Ty> Ty simd256_selb(Ty _mask, Ty _a, Ty _b);
	template<typename Ty> Ty simd256_sels(Ty _test, Ty _a, Ty _b);

	template<typename Ty> Ty simd256_f64_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_sub(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_mul(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_div(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_min(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_max(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_madd(Ty _a, Ty _b, Ty _c);
	template<typename Ty> Ty simd256_f64_nmsub(Ty _a, Ty _b, Ty _c);
	template<typename Ty> Ty simd256_f64_neg(Ty _a);
	template<typename Ty> Ty simd256_f64_abs(Ty _a);
	template<typename Ty> Ty simd256_f64_clamp(Ty _a, Ty _min, Ty _max);
	template<typename Ty> Ty simd256_f64_lerp(Ty _a, Ty _b, Ty _s);
	template<typename Ty> Ty simd256_f64_rcp(Ty _a);
	template<typename Ty> Ty simd256_f64_sqrt(Ty _a);
	template<typename Ty> Ty simd256_f64_rsqrt(Ty _a);
	template<typename Ty> Ty simd256_f64_round(Ty _a);
	template<typename Ty> Ty simd256_f64_ceil(Ty _a);
	template<typename Ty> Ty simd256_f64_floor(Ty _a);
	template<typename Ty> Ty simd256_f64_cmpeq(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_cmpneq(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_cmplt(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_cmple(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_cmpgt(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_f64_cmpge(Ty _a, Ty _b);

	template<typename Ty> Ty simd256_i64_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_i64_sub(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u64_add(Ty _a, Ty _b);
	template<typename Ty> Ty simd256_u64_sub(Ty _a, Ty _b);

	// These are used when a platform lacks a dedicated instruction.

	template<typename Ty>
	Ty simd_f32_madd_ni(Ty _a, Ty _b, Ty _c);

	template<typename Ty>
	Ty simd_f32_msub_ni(Ty _a, Ty _b, Ty _c);

	template<typename Ty>
	Ty simd_f32_nmsub_ni(Ty _a, Ty _b, Ty _c);

	template<typename Ty>
	int simd_x32_signbitsmask_ni(Ty _a);

	template<typename Ty>
	int simd_x8_signbitsmask_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_neg_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_abs_ni(Ty _a);

	template<typename Ty>
	Ty simd_f64_abs_ni(Ty _a);

	template<typename Ty>
	Ty simd_andc_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_u32_cmplt_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_u32_cmpgt_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_f64_madd_ni(Ty _a, Ty _b, Ty _c);

	template<typename Ty>
	Ty simd_f64_nmsub_ni(Ty _a, Ty _b, Ty _c);

	template<typename Ty>
	Ty simd_f64_neg_ni(Ty _a);

	template<typename Ty>
	Ty simd_f64_lerp_ni(Ty _a, Ty _b, Ty _s);

	template<typename Ty>
	Ty simd_f64_rcp_ni(Ty _a);

	template<typename Ty>
	Ty simd_f64_rsqrt_ni(Ty _a);

	template<typename Ty>
	Ty simd_f64_cmpneq_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_f32_rcp_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_div_nr_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_f32_sqrt_nr_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_sqrt_nr1_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_rsqrt_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_rsqrt_nr_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_rsqrt_carmack_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_min_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_f32_max_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_f32_clamp_ni(Ty _a, Ty _min, Ty _max);

	template<typename Ty>
	Ty simd_f32_lerp_ni(Ty _a, Ty _b, Ty _s);

	template<typename Ty>
	Ty simd128_f32_dot3_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd128_f32_dot_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd128_f32_cross3_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd128_f32_normalize3_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_ceil_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_floor_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_round_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_log2_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_exp2_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_pow_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_f32_ldexp_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_f32_cos_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_sin_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_log_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_exp_ni(Ty _a);

	template<typename Ty>
	Ty simd_f32_cmpneq_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_i32_min_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_i32_max_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_i32_neg_ni(Ty _a);

	template<typename Ty>
	Ty simd_i32_abs_ni(Ty _a);

	template<typename Ty>
	Ty simd_selb_ni(Ty _mask, Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_sels_ni(Ty _test, Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_not_ni(Ty _a);

	template<typename Ty>
	Ty simd_orc_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd128_orx_ni(Ty _a);

	template<typename Ty>
	Ty simd128_x32_shuf_xAzC_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd128_x32_shuf_yBwD_ni(Ty _a, Ty _b);

	template<typename Ty>
	bool simd128_test_any_ni(Ty _a);

	template<typename Ty>
	bool simd128_test_all_ni(Ty _a);

	template<typename Ty>
	bool simd128_test_zero_ni(Ty _a, Ty _b);

	template<typename Ty>
	Ty simd_f32_ftoi_round_ni(Ty _a);

	template<typename Ty>
	Ty simd_u32_cntlz_ni(Ty _a);

	template<typename Ty>
	Ty simd_x32_srl_ni(Ty _a, Ty _count);

	template<typename Ty>
	Ty simd_x32_sll_ni(Ty _a, Ty _count);

	template<typename Ty>
	Ty simd_x32_sra_ni(Ty _a, Ty _count);

	template<typename Ty>
	Ty simd_x8_shuffle_ni(Ty _a, Ty _indices);

	template<typename Ty>
	Ty simd_x8_shuffle_ni(Ty _a, Ty _b, Ty _indices);

	template<typename Ty>
	Ty simd_f16_fromf32_ni(Ty _a);

	template<typename Ty>
	Ty simd_f16_tof32_ni(Ty _a);

} // namespace bx

#if BX_SIMD_AVX
#	include "simd256_avx.inl"
#endif // BX_SIMD_AVX

#if BX_SIMD_SSE
#	include "simd128_sse.inl"
#endif // BX_SIMD_SSE

#if BX_SIMD_NEON
#	include "simd128_neon.inl"
#endif // BX_SIMD_NEON

#if BX_SIMD_WASM
#	include "simd128_wasm.inl"
#endif // BX_SIMD_WASM

namespace bx
{
	BX_CONSTEXPR_FUNC float floor(float _f);
	BX_CONSTEXPR_FUNC float ceil(float _f);
	BX_CONSTEXPR_FUNC float round(float _f);
	BX_CONSTEXPR_FUNC float sqrt(float _a);
} // namespace bx

#include "simd32_ref.inl"
#include "simd64_ref.inl"
#include "simd128_ref.inl"
#include "simd256_ref.inl"
#include "simd_ni.inl"

namespace bx
{

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_add(simd128_t _a, simd128_t _b) { return simd128_f32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_sub(simd128_t _a, simd128_t _b) { return simd128_f32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_mul(simd128_t _a, simd128_t _b) { return simd128_f32_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_div(simd128_t _a, simd128_t _b) { return simd128_f32_div(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_madd(simd128_t _a, simd128_t _b, simd128_t _c) { return simd128_f32_madd(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_msub(simd128_t _a, simd128_t _b, simd128_t _c) { return simd128_f32_msub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE int simd_x32_signbitsmask(simd128_t _a) { return simd128_x32_signbitsmask(_a); }

	template<>
	BX_SIMD_FORCE_INLINE int simd_x8_signbitsmask(simd128_t _a) { return simd128_x8_signbitsmask(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_x8_shuffle(simd128_t _a, simd128_t _indices) { return simd128_x8_shuffle(_a, _indices); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_x8_shuffle(simd128_t _a, simd128_t _b, simd128_t _indices) { return simd128_x8_shuffle(_a, _b, _indices); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_min(simd128_t _a, simd128_t _b) { return simd128_f32_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_max(simd128_t _a, simd128_t _b) { return simd128_f32_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_add(simd128_t _a, simd128_t _b) { return simd128_i32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_sub(simd128_t _a, simd128_t _b) { return simd128_i32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_neg(simd128_t _a) { return simd128_i32_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_abs(simd128_t _a) { return simd128_i32_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_and(simd128_t _a, simd128_t _b) { return simd128_and(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_or(simd128_t _a, simd128_t _b) { return simd128_or(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_xor(simd128_t _a, simd128_t _b) { return simd128_xor(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_add(simd256_t _a, simd256_t _b) { return simd256_f32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_sub(simd256_t _a, simd256_t _b) { return simd256_f32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_mul(simd256_t _a, simd256_t _b) { return simd256_f32_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_div(simd256_t _a, simd256_t _b) { return simd256_f32_div(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_msub(simd256_t _a, simd256_t _b, simd256_t _c) { return simd256_f32_msub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_madd(simd256_t _a, simd256_t _b, simd256_t _c) { return simd256_f32_madd(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE int simd_x32_signbitsmask(simd256_t _a) { return simd256_x32_signbitsmask(_a); }

	template<>
	BX_SIMD_FORCE_INLINE int simd_x8_signbitsmask(simd256_t _a) { return simd256_x8_signbitsmask(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_x8_shuffle(simd256_t _a, simd256_t _indices) { return simd256_x8_shuffle(_a, _indices); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_x8_shuffle(simd256_t _a, simd256_t _b, simd256_t _indices) { return simd256_x8_shuffle(_a, _b, _indices); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_min(simd256_t _a, simd256_t _b) { return simd256_f32_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_max(simd256_t _a, simd256_t _b) { return simd256_f32_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_add(simd256_t _a, simd256_t _b) { return simd256_i32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_sub(simd256_t _a, simd256_t _b) { return simd256_i32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_neg(simd256_t _a) { return simd256_i32_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_abs(simd256_t _a) { return simd256_i32_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_and(simd256_t _a, simd256_t _b) { return simd256_and(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_or(simd256_t _a, simd256_t _b) { return simd256_or(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_xor(simd256_t _a, simd256_t _b) { return simd256_xor(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_nmsub(simd128_t _a, simd128_t _b, simd128_t _c) { return simd128_f32_nmsub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_neg(simd128_t _a) { return simd128_f32_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_abs(simd128_t _a) { return simd128_f32_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_rcp_est(simd128_t _a) { return simd128_f32_rcp_est(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_rsqrt_est(simd128_t _a) { return simd128_f32_rsqrt_est(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_sqrt(simd128_t _a) { return simd128_f32_sqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_rsqrt(simd128_t _a) { return simd128_f32_rsqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_div_nr(simd128_t _a, simd128_t _b) { return simd128_f32_div_nr(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_sqrt_nr(simd128_t _a) { return simd128_f32_sqrt_nr(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_sqrt_nr1(simd128_t _a) { return simd_f32_sqrt_nr1_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_rsqrt_nr(simd128_t _a) { return simd128_f32_rsqrt_nr(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_rsqrt_carmack(simd128_t _a) { return simd128_f32_rsqrt_carmack(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_cmpeq(simd128_t _a, simd128_t _b) { return simd128_f32_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_cmplt(simd128_t _a, simd128_t _b) { return simd128_f32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_cmpgt(simd128_t _a, simd128_t _b) { return simd128_f32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_cmpneq(simd128_t _a, simd128_t _b) { return simd128_f32_cmpneq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_cmple(simd128_t _a, simd128_t _b) { return simd128_f32_cmpgt(_b, _a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_cmpge(simd128_t _a, simd128_t _b) { return simd128_f32_cmplt(_b, _a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_clamp(simd128_t _a, simd128_t _min, simd128_t _max) { return simd128_f32_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_lerp(simd128_t _a, simd128_t _b, simd128_t _s) { return simd128_f32_lerp(_a, _b, _s); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_rcp(simd128_t _a) { return simd128_f32_rcp(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_round(simd128_t _a) { return simd128_f32_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_ceil(simd128_t _a) { return simd128_f32_ceil(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_floor(simd128_t _a) { return simd128_f32_floor(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_cos(simd128_t _a) { return simd_f32_cos_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_sin(simd128_t _a) { return simd_f32_sin_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_log(simd128_t _a) { return simd_f32_log_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_exp(simd128_t _a) { return simd_f32_exp_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_ldexp(simd128_t _a, simd128_t _b) { return simd_f32_ldexp_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_log2(simd128_t _a) { return simd_f32_log2_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_exp2(simd128_t _a) { return simd_f32_exp2_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_pow(simd128_t _a, simd128_t _b) { return simd_f32_pow_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_ftoi_trunc(simd128_t _a) { return simd128_f32_ftoi_trunc(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f32_ftoi_round(simd128_t _a) { return simd128_f32_ftoi_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_itof(simd128_t _a) { return simd128_i32_itof(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_add(simd128_t _a, simd128_t _b) { return simd128_f64_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_sub(simd128_t _a, simd128_t _b) { return simd128_f64_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_mul(simd128_t _a, simd128_t _b) { return simd128_f64_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_div(simd128_t _a, simd128_t _b) { return simd128_f64_div(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_madd(simd128_t _a, simd128_t _b, simd128_t _c) { return simd128_f64_madd(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_nmsub(simd128_t _a, simd128_t _b, simd128_t _c) { return simd128_f64_nmsub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_neg(simd128_t _a) { return simd128_f64_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_abs(simd128_t _a) { return simd128_f64_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_min(simd128_t _a, simd128_t _b) { return simd128_f64_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_max(simd128_t _a, simd128_t _b) { return simd128_f64_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_clamp(simd128_t _a, simd128_t _min, simd128_t _max) { return simd128_f64_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_lerp(simd128_t _a, simd128_t _b, simd128_t _s) { return simd128_f64_lerp(_a, _b, _s); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_rcp(simd128_t _a) { return simd128_f64_rcp(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_sqrt(simd128_t _a) { return simd128_f64_sqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_rsqrt(simd128_t _a) { return simd128_f64_rsqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_round(simd128_t _a) { return simd128_f64_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_ceil(simd128_t _a) { return simd128_f64_ceil(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_floor(simd128_t _a) { return simd128_f64_floor(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_cmpeq(simd128_t _a, simd128_t _b) { return simd128_f64_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_cmpneq(simd128_t _a, simd128_t _b) { return simd128_f64_cmpneq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_cmplt(simd128_t _a, simd128_t _b) { return simd128_f64_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_cmple(simd128_t _a, simd128_t _b) { return simd128_f64_cmple(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_cmpgt(simd128_t _a, simd128_t _b) { return simd128_f64_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_f64_cmpge(simd128_t _a, simd128_t _b) { return simd128_f64_cmpge(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_cmplt(simd128_t _a, simd128_t _b) { return simd128_i32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_cmpgt(simd128_t _a, simd128_t _b) { return simd128_i32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_min(simd128_t _a, simd128_t _b) { return simd128_i32_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_max(simd128_t _a, simd128_t _b) { return simd128_i32_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_clamp(simd128_t _a, simd128_t _min, simd128_t _max) { return simd128_i32_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_cmpeq(simd128_t _a, simd128_t _b) { return simd128_i32_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_u32_add(simd128_t _a, simd128_t _b) { return simd128_u32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_u32_sub(simd128_t _a, simd128_t _b) { return simd128_u32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_u32_cmplt(simd128_t _a, simd128_t _b) { return simd128_u32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_u32_cmpgt(simd128_t _a, simd128_t _b) { return simd128_u32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_orc(simd128_t _a, simd128_t _b) { return simd128_orc(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_ld<simd128_t>(const void* _ptr) { return simd128_ld<simd128_t>(_ptr); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_ldu<simd128_t>(const void* _ptr) { return simd128_ldu<simd128_t>(_ptr); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_st(void* _ptr, simd128_t _a) { simd128_st(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_stu(void* _ptr, simd128_t _a) { simd128_stu(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_x32_st1(void* _ptr, simd128_t _a) { simd128_x32_st1(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_selb(simd128_t _mask, simd128_t _a, simd128_t _b) { return simd128_selb(_mask, _a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_sels(simd128_t _test, simd128_t _a, simd128_t _b) { return simd128_sels(_test, _a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_not(simd128_t _a) { return simd128_not(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_andc(simd128_t _a, simd128_t _b) { return simd128_andc(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_x32_sra(simd128_t _a, int _count) { return simd128_x32_sra(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_x32_srl(simd128_t _a, int _count) { return simd128_x32_srl(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_x32_sll(simd128_t _a, int _count) { return simd128_x32_sll(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_x32_sll(simd128_t _a, simd128_t _count) { return simd128_x32_sll(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_x32_srl(simd128_t _a, simd128_t _count) { return simd128_x32_srl(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_x32_sra(simd128_t _a, simd128_t _count) { return simd128_x32_sra(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_i32_mul(simd128_t _a, simd128_t _b) { return simd128_u32_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_u8_satadd(simd128_t _a, simd128_t _b) { return simd128_u8_satadd(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_u8_satsub(simd128_t _a, simd128_t _b) { return simd128_u8_satsub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_u16_satadd(simd128_t _a, simd128_t _b) { return simd128_u16_satadd(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_u16_satsub(simd128_t _a, simd128_t _b) { return simd128_u16_satsub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_splat<simd128_t>(float _a) { return simd128_splat<simd128_t>(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_splat<simd128_t>(uint32_t _a) { return simd128_splat<simd128_t>(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_splat<simd128_t>(double _a) { return simd128_splat<simd128_t>(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd128_t simd_zero<simd128_t>() { return simd128_zero<simd128_t>(); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_any(simd128_t _test) { return simd128_test_any_xyzw(_test); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_all(simd128_t _test) { return simd128_test_all_xyzw(_test); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_zero(simd128_t _a, simd128_t _b) { return simd128_test_zero(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_nmsub(simd256_t _a, simd256_t _b, simd256_t _c) { return simd256_f32_nmsub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_neg(simd256_t _a) { return simd256_f32_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_abs(simd256_t _a) { return simd256_f32_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_rcp_est(simd256_t _a) { return simd256_f32_rcp_est(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_rsqrt_est(simd256_t _a) { return simd256_f32_rsqrt_est(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_sqrt(simd256_t _a) { return simd256_f32_sqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_rsqrt(simd256_t _a) { return simd256_f32_rsqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_div_nr(simd256_t _a, simd256_t _b) { return simd_f32_div_nr_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_sqrt_nr(simd256_t _a) { return simd_f32_sqrt_nr_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_sqrt_nr1(simd256_t _a) { return simd_f32_sqrt_nr1_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_rsqrt_nr(simd256_t _a) { return simd_f32_rsqrt_nr_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_rsqrt_carmack(simd256_t _a) { return simd_f32_rsqrt_carmack_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_cmpeq(simd256_t _a, simd256_t _b) { return simd256_f32_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_cmplt(simd256_t _a, simd256_t _b) { return simd256_f32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_cmpgt(simd256_t _a, simd256_t _b) { return simd256_f32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_cmpneq(simd256_t _a, simd256_t _b) { return simd256_f32_cmpneq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_cmple(simd256_t _a, simd256_t _b) { return simd256_f32_cmpgt(_b, _a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_cmpge(simd256_t _a, simd256_t _b) { return simd256_f32_cmplt(_b, _a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_clamp(simd256_t _a, simd256_t _min, simd256_t _max) { return simd256_f32_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_lerp(simd256_t _a, simd256_t _b, simd256_t _s) { return simd256_f32_lerp(_a, _b, _s); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_rcp(simd256_t _a) { return simd256_f32_rcp(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_round(simd256_t _a) { return simd256_f32_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_ceil(simd256_t _a) { return simd256_f32_ceil(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_floor(simd256_t _a) { return simd256_f32_floor(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_cos(simd256_t _a) { return simd_f32_cos_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_sin(simd256_t _a) { return simd_f32_sin_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_log(simd256_t _a) { return simd_f32_log_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_exp(simd256_t _a) { return simd_f32_exp_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_ldexp(simd256_t _a, simd256_t _b) { return simd_f32_ldexp_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_log2(simd256_t _a) { return simd_f32_log2_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_exp2(simd256_t _a) { return simd_f32_exp2_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_pow(simd256_t _a, simd256_t _b) { return simd_f32_pow_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_ftoi_trunc(simd256_t _a) { return simd256_f32_ftoi_trunc(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f32_ftoi_round(simd256_t _a) { return simd256_f32_ftoi_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_itof(simd256_t _a) { return simd256_i32_itof(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_add(simd256_t _a, simd256_t _b) { return simd256_f64_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_sub(simd256_t _a, simd256_t _b) { return simd256_f64_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_mul(simd256_t _a, simd256_t _b) { return simd256_f64_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_div(simd256_t _a, simd256_t _b) { return simd256_f64_div(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_madd(simd256_t _a, simd256_t _b, simd256_t _c) { return simd256_f64_madd(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_nmsub(simd256_t _a, simd256_t _b, simd256_t _c) { return simd256_f64_nmsub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_neg(simd256_t _a) { return simd256_f64_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_abs(simd256_t _a) { return simd256_f64_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_min(simd256_t _a, simd256_t _b) { return simd256_f64_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_max(simd256_t _a, simd256_t _b) { return simd256_f64_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_clamp(simd256_t _a, simd256_t _min, simd256_t _max) { return simd256_f64_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_lerp(simd256_t _a, simd256_t _b, simd256_t _s) { return simd256_f64_lerp(_a, _b, _s); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_rcp(simd256_t _a) { return simd256_f64_rcp(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_sqrt(simd256_t _a) { return simd256_f64_sqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_rsqrt(simd256_t _a) { return simd256_f64_rsqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_round(simd256_t _a) { return simd256_f64_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_ceil(simd256_t _a) { return simd256_f64_ceil(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_floor(simd256_t _a) { return simd256_f64_floor(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_cmpeq(simd256_t _a, simd256_t _b) { return simd256_f64_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_cmpneq(simd256_t _a, simd256_t _b) { return simd256_f64_cmpneq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_cmplt(simd256_t _a, simd256_t _b) { return simd256_f64_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_cmple(simd256_t _a, simd256_t _b) { return simd256_f64_cmple(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_cmpgt(simd256_t _a, simd256_t _b) { return simd256_f64_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_f64_cmpge(simd256_t _a, simd256_t _b) { return simd256_f64_cmpge(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_cmplt(simd256_t _a, simd256_t _b) { return simd256_i32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_cmpgt(simd256_t _a, simd256_t _b) { return simd256_i32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_min(simd256_t _a, simd256_t _b) { return simd256_i32_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_max(simd256_t _a, simd256_t _b) { return simd256_i32_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_clamp(simd256_t _a, simd256_t _min, simd256_t _max) { return simd256_i32_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_cmpeq(simd256_t _a, simd256_t _b) { return simd256_i32_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_u32_add(simd256_t _a, simd256_t _b) { return simd256_u32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_u32_sub(simd256_t _a, simd256_t _b) { return simd256_u32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_u32_cmplt(simd256_t _a, simd256_t _b) { return simd256_u32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_u32_cmpgt(simd256_t _a, simd256_t _b) { return simd256_u32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_orc(simd256_t _a, simd256_t _b) { return simd256_orc(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_ld<simd256_t>(const void* _ptr) { return simd256_ld<simd256_t>(_ptr); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_ldu<simd256_t>(const void* _ptr) { return simd256_ldu<simd256_t>(_ptr); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_st(void* _ptr, simd256_t _a) { simd256_st(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_stu(void* _ptr, simd256_t _a) { simd256_stu(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_x32_st1(void* _ptr, simd256_t _a) { simd256_x32_st1(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_selb(simd256_t _mask, simd256_t _a, simd256_t _b) { return simd256_selb(_mask, _a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_sels(simd256_t _test, simd256_t _a, simd256_t _b) { return simd256_sels(_test, _a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_not(simd256_t _a) { return simd256_not(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_andc(simd256_t _a, simd256_t _b) { return simd256_andc(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_x32_sra(simd256_t _a, int _count) { return simd256_x32_sra(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_x32_srl(simd256_t _a, int _count) { return simd256_x32_srl(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_x32_sll(simd256_t _a, int _count) { return simd256_x32_sll(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_x32_sll(simd256_t _a, simd256_t _count) { return simd256_x32_sll(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_x32_srl(simd256_t _a, simd256_t _count) { return simd256_x32_srl(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_x32_sra(simd256_t _a, simd256_t _count) { return simd256_x32_sra(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_i32_mul(simd256_t _a, simd256_t _b) { return simd256_u32_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_u8_satadd(simd256_t _a, simd256_t _b) { return simd256_u8_satadd(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_u8_satsub(simd256_t _a, simd256_t _b) { return simd256_u8_satsub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_u16_satadd(simd256_t _a, simd256_t _b) { return simd256_u16_satadd(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_u16_satsub(simd256_t _a, simd256_t _b) { return simd256_u16_satsub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_splat<simd256_t>(float _a) { return simd256_splat<simd256_t>(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_splat<simd256_t>(uint32_t _a) { return simd256_splat<simd256_t>(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_splat<simd256_t>(double _a) { return simd256_splat<simd256_t>(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd256_t simd_zero<simd256_t>() { return simd256_zero<simd256_t>(); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_any(simd256_t _test) { return simd256_test_any(_test); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_all(simd256_t _test) { return simd256_test_all(_test); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_zero(simd256_t _a, simd256_t _b) { return simd256_test_zero(_a, _b); }

	// --- simd32_t dispatch ---

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_and(simd32_t _a, simd32_t _b) { return simd32_and(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_andc(simd32_t _a, simd32_t _b) { return simd32_andc(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_or(simd32_t _a, simd32_t _b) { return simd32_or(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_orc(simd32_t _a, simd32_t _b) { return simd32_orc(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_xor(simd32_t _a, simd32_t _b) { return simd32_xor(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_not(simd32_t _a) { return simd32_not(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_x32_srl(simd32_t _a, int _count) { return simd32_x32_srl(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_x32_sll(simd32_t _a, int _count) { return simd32_x32_sll(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_x32_sra(simd32_t _a, int _count) { return simd32_x32_sra(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_x32_sll(simd32_t _a, simd32_t _count) { return simd32_x32_sll(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_x32_srl(simd32_t _a, simd32_t _count) { return simd32_x32_srl(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_x32_sra(simd32_t _a, simd32_t _count) { return simd32_x32_sra(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_x8_shuffle(simd32_t _a, simd32_t _indices) { return simd32_x8_shuffle(_a, _indices); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_x8_shuffle(simd32_t _a, simd32_t _b, simd32_t _indices) { return simd32_x8_shuffle(_a, _b, _indices); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_mul(simd32_t _a, simd32_t _b) { return simd32_u32_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_u8_satadd(simd32_t _a, simd32_t _b) { return simd32_u8_satadd(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_u8_satsub(simd32_t _a, simd32_t _b) { return simd32_u8_satsub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_u16_satadd(simd32_t _a, simd32_t _b) { return simd32_u16_satadd(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_u16_satsub(simd32_t _a, simd32_t _b) { return simd32_u16_satsub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_u32_add(simd32_t _a, simd32_t _b) { return simd32_u32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_u32_sub(simd32_t _a, simd32_t _b) { return simd32_u32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_u32_cmplt(simd32_t _a, simd32_t _b) { return simd32_u32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_u32_cmpgt(simd32_t _a, simd32_t _b) { return simd32_u32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_add(simd32_t _a, simd32_t _b) { return simd32_i32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_sub(simd32_t _a, simd32_t _b) { return simd32_i32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_neg(simd32_t _a) { return simd32_i32_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_abs(simd32_t _a) { return simd32_i32_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_min(simd32_t _a, simd32_t _b) { return simd32_i32_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_max(simd32_t _a, simd32_t _b) { return simd32_i32_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_clamp(simd32_t _a, simd32_t _min, simd32_t _max) { return simd32_i32_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_cmpeq(simd32_t _a, simd32_t _b) { return simd32_i32_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_cmplt(simd32_t _a, simd32_t _b) { return simd32_i32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_cmpgt(simd32_t _a, simd32_t _b) { return simd32_i32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_add(simd32_t _a, simd32_t _b) { return simd32_f32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_sub(simd32_t _a, simd32_t _b) { return simd32_f32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_mul(simd32_t _a, simd32_t _b) { return simd32_f32_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_div(simd32_t _a, simd32_t _b) { return simd32_f32_div(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_min(simd32_t _a, simd32_t _b) { return simd32_f32_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_max(simd32_t _a, simd32_t _b) { return simd32_f32_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_madd(simd32_t _a, simd32_t _b, simd32_t _c) { return simd32_f32_madd(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_msub(simd32_t _a, simd32_t _b, simd32_t _c) { return simd32_f32_msub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_nmsub(simd32_t _a, simd32_t _b, simd32_t _c) { return simd32_f32_nmsub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_clamp(simd32_t _a, simd32_t _min, simd32_t _max) { return simd32_f32_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_lerp(simd32_t _a, simd32_t _b, simd32_t _s) { return simd32_f32_lerp(_a, _b, _s); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_neg(simd32_t _a) { return simd32_f32_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_abs(simd32_t _a) { return simd32_f32_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_round(simd32_t _a) { return simd32_f32_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_ceil(simd32_t _a) { return simd32_f32_ceil(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_floor(simd32_t _a) { return simd32_f32_floor(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_rcp(simd32_t _a) { return simd32_f32_rcp(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_rcp_est(simd32_t _a) { return simd32_f32_rcp(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_sqrt(simd32_t _a) { return simd32_f32_sqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_rsqrt(simd32_t _a) { return simd32_f32_rsqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_rsqrt_est(simd32_t _a) { return simd32_f32_rsqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_div_nr(simd32_t _a, simd32_t _b) { return simd_f32_div_nr_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_sqrt_nr(simd32_t _a) { return simd_f32_sqrt_nr_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_sqrt_nr1(simd32_t _a) { return simd_f32_sqrt_nr1_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_rsqrt_nr(simd32_t _a) { return simd_f32_rsqrt_nr_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_rsqrt_carmack(simd32_t _a) { return simd_f32_rsqrt_carmack_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_cos(simd32_t _a) { return simd_f32_cos_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_sin(simd32_t _a) { return simd_f32_sin_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_log(simd32_t _a) { return simd_f32_log_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_exp(simd32_t _a) { return simd_f32_exp_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_log2(simd32_t _a) { return simd_f32_log2_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_exp2(simd32_t _a) { return simd_f32_exp2_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_pow(simd32_t _a, simd32_t _b) { return simd_f32_pow_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_ldexp(simd32_t _a, simd32_t _b) { return simd_f32_ldexp_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_ftoi_trunc(simd32_t _a) { return simd32_f32_ftoi_trunc(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_ftoi_round(simd32_t _a) { return simd32_f32_ftoi_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_i32_itof(simd32_t _a) { return simd32_i32_itof(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_cmpeq(simd32_t _a, simd32_t _b) { return simd32_f32_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_cmpneq(simd32_t _a, simd32_t _b) { return simd32_f32_cmpneq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_cmplt(simd32_t _a, simd32_t _b) { return simd32_f32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_cmple(simd32_t _a, simd32_t _b) { return simd32_f32_cmple(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_cmpgt(simd32_t _a, simd32_t _b) { return simd32_f32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_f32_cmpge(simd32_t _a, simd32_t _b) { return simd32_f32_cmpge(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE int simd_x32_signbitsmask(simd32_t _a) { return simd32_x32_signbitsmask(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_sels(simd32_t _test, simd32_t _a, simd32_t _b) { return simd32_sels(_test, _a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_selb(simd32_t _mask, simd32_t _a, simd32_t _b) { return simd32_selb(_mask, _a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_splat<simd32_t>(float _a) { return simd32_splat(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_splat<simd32_t>(uint32_t _a) { return simd32_splat(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_zero<simd32_t>() { return simd32_zero(); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_ld<simd32_t>(const void* _ptr) { return simd32_ld(_ptr); }

	template<>
	BX_SIMD_FORCE_INLINE simd32_t simd_ldu<simd32_t>(const void* _ptr) { return simd32_ldu(_ptr); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_st(void* _ptr, simd32_t _a) { simd32_st(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_stu(void* _ptr, simd32_t _a) { simd32_stu(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_x32_st1(void* _ptr, simd32_t _a) { simd32_x32_st1(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_any(simd32_t _test) { return simd32_test_any(_test); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_all(simd32_t _test) { return simd32_test_all(_test); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_zero(simd32_t _a, simd32_t _b) { return simd32_test_zero(_a, _b); }

	// --- simd64_t dispatch ---

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_and(simd64_t _a, simd64_t _b) { return simd64_and(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_andc(simd64_t _a, simd64_t _b) { return simd64_andc(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_or(simd64_t _a, simd64_t _b) { return simd64_or(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_orc(simd64_t _a, simd64_t _b) { return simd64_orc(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_xor(simd64_t _a, simd64_t _b) { return simd64_xor(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_not(simd64_t _a) { return simd64_not(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_x32_srl(simd64_t _a, int _count) { return simd64_x32_srl(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_x32_sll(simd64_t _a, int _count) { return simd64_x32_sll(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_x32_sra(simd64_t _a, int _count) { return simd64_x32_sra(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_x32_sll(simd64_t _a, simd64_t _count) { return simd64_x32_sll(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_x32_srl(simd64_t _a, simd64_t _count) { return simd64_x32_srl(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_x32_sra(simd64_t _a, simd64_t _count) { return simd64_x32_sra(_a, _count); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_x8_shuffle(simd64_t _a, simd64_t _indices) { return simd64_x8_shuffle(_a, _indices); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_x8_shuffle(simd64_t _a, simd64_t _b, simd64_t _indices) { return simd64_x8_shuffle(_a, _b, _indices); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_mul(simd64_t _a, simd64_t _b) { return simd64_u32_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_u8_satadd(simd64_t _a, simd64_t _b) { return simd64_u8_satadd(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_u8_satsub(simd64_t _a, simd64_t _b) { return simd64_u8_satsub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_u16_satadd(simd64_t _a, simd64_t _b) { return simd64_u16_satadd(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_u16_satsub(simd64_t _a, simd64_t _b) { return simd64_u16_satsub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_u32_add(simd64_t _a, simd64_t _b) { return simd64_u32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_u32_sub(simd64_t _a, simd64_t _b) { return simd64_u32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_u32_cmplt(simd64_t _a, simd64_t _b) { return simd64_u32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_u32_cmpgt(simd64_t _a, simd64_t _b) { return simd64_u32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_add(simd64_t _a, simd64_t _b) { return simd64_i32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_sub(simd64_t _a, simd64_t _b) { return simd64_i32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_neg(simd64_t _a) { return simd64_i32_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_abs(simd64_t _a) { return simd64_i32_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_min(simd64_t _a, simd64_t _b) { return simd64_i32_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_max(simd64_t _a, simd64_t _b) { return simd64_i32_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_clamp(simd64_t _a, simd64_t _min, simd64_t _max) { return simd64_i32_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_cmpeq(simd64_t _a, simd64_t _b) { return simd64_i32_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_cmplt(simd64_t _a, simd64_t _b) { return simd64_i32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_cmpgt(simd64_t _a, simd64_t _b) { return simd64_i32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_add(simd64_t _a, simd64_t _b) { return simd64_f32_add(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_sub(simd64_t _a, simd64_t _b) { return simd64_f32_sub(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_mul(simd64_t _a, simd64_t _b) { return simd64_f32_mul(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_div(simd64_t _a, simd64_t _b) { return simd64_f32_div(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_min(simd64_t _a, simd64_t _b) { return simd64_f32_min(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_max(simd64_t _a, simd64_t _b) { return simd64_f32_max(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_madd(simd64_t _a, simd64_t _b, simd64_t _c) { return simd64_f32_madd(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_msub(simd64_t _a, simd64_t _b, simd64_t _c) { return simd64_f32_msub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_nmsub(simd64_t _a, simd64_t _b, simd64_t _c) { return simd64_f32_nmsub(_a, _b, _c); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_clamp(simd64_t _a, simd64_t _min, simd64_t _max) { return simd64_f32_clamp(_a, _min, _max); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_lerp(simd64_t _a, simd64_t _b, simd64_t _s) { return simd64_f32_lerp(_a, _b, _s); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_neg(simd64_t _a) { return simd64_f32_neg(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_abs(simd64_t _a) { return simd64_f32_abs(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_round(simd64_t _a) { return simd64_f32_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_ceil(simd64_t _a) { return simd64_f32_ceil(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_floor(simd64_t _a) { return simd64_f32_floor(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_rcp(simd64_t _a) { return simd64_f32_rcp(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_rcp_est(simd64_t _a) { return simd64_f32_rcp(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_sqrt(simd64_t _a) { return simd64_f32_sqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_rsqrt(simd64_t _a) { return simd64_f32_rsqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_rsqrt_est(simd64_t _a) { return simd64_f32_rsqrt(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_div_nr(simd64_t _a, simd64_t _b) { return simd_f32_div_nr_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_sqrt_nr(simd64_t _a) { return simd_f32_sqrt_nr_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_sqrt_nr1(simd64_t _a) { return simd_f32_sqrt_nr1_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_rsqrt_nr(simd64_t _a) { return simd_f32_rsqrt_nr_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_rsqrt_carmack(simd64_t _a) { return simd_f32_rsqrt_carmack_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_cos(simd64_t _a) { return simd_f32_cos_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_sin(simd64_t _a) { return simd_f32_sin_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_log(simd64_t _a) { return simd_f32_log_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_exp(simd64_t _a) { return simd_f32_exp_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_log2(simd64_t _a) { return simd_f32_log2_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_exp2(simd64_t _a) { return simd_f32_exp2_ni(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_pow(simd64_t _a, simd64_t _b) { return simd_f32_pow_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_ldexp(simd64_t _a, simd64_t _b) { return simd_f32_ldexp_ni(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_ftoi_trunc(simd64_t _a) { return simd64_f32_ftoi_trunc(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_ftoi_round(simd64_t _a) { return simd64_f32_ftoi_round(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_i32_itof(simd64_t _a) { return simd64_i32_itof(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_cmpeq(simd64_t _a, simd64_t _b) { return simd64_f32_cmpeq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_cmpneq(simd64_t _a, simd64_t _b) { return simd64_f32_cmpneq(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_cmplt(simd64_t _a, simd64_t _b) { return simd64_f32_cmplt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_cmple(simd64_t _a, simd64_t _b) { return simd64_f32_cmple(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_cmpgt(simd64_t _a, simd64_t _b) { return simd64_f32_cmpgt(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_f32_cmpge(simd64_t _a, simd64_t _b) { return simd64_f32_cmpge(_a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE int simd_x32_signbitsmask(simd64_t _a) { return simd64_x32_signbitsmask(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_sels(simd64_t _test, simd64_t _a, simd64_t _b) { return simd64_sels(_test, _a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_selb(simd64_t _mask, simd64_t _a, simd64_t _b) { return simd64_selb(_mask, _a, _b); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_splat<simd64_t>(float _a) { return simd64_splat(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_splat<simd64_t>(uint32_t _a) { return simd64_splat(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_splat<simd64_t>(double _a) { return simd64_splat(_a); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_zero<simd64_t>() { return simd64_zero(); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_ld<simd64_t>(const void* _ptr) { return simd64_ld(_ptr); }

	template<>
	BX_SIMD_FORCE_INLINE simd64_t simd_ldu<simd64_t>(const void* _ptr) { return simd64_ldu(_ptr); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_st(void* _ptr, simd64_t _a) { simd64_st(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_stu(void* _ptr, simd64_t _a) { simd64_stu(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE void simd_x32_st1(void* _ptr, simd64_t _a) { simd64_x32_st1(_ptr, _a); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_any(simd64_t _test) { return simd64_test_any(_test); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_all(simd64_t _test) { return simd64_test_all(_test); }

	template<>
	BX_SIMD_FORCE_INLINE bool simd_test_zero(simd64_t _a, simd64_t _b) { return simd64_test_zero(_a, _b); }


} // namespace bx

namespace bx
{

	BX_SIMD_FORCE_INLINE simd128_t simd128_ld(const void* _ptr)
	{
		return simd128_ld<simd128_t>(_ptr);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_ldu(const void* _ptr)
	{
		return simd128_ldu<simd128_t>(_ptr);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_ld(float _x, float _y, float _z, float _w)
	{
		return simd128_ld<simd128_t>(_x, _y, _z, _w);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_ld(int32_t _x, int32_t _y, int32_t _z, int32_t _w)
	{
		return simd128_ld<simd128_t>(_x, _y, _z, _w);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_ld(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w)
	{
		return simd128_ld<simd128_t>(_x, _y, _z, _w);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_splat(float _a)
	{
		return simd128_splat<simd128_t>(_a);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_splat(int32_t _a)
	{
		return simd128_splat<simd128_t>(_a);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_splat(uint32_t _a)
	{
		return simd128_splat<simd128_t>(_a);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_splat(int16_t _a)
	{
		return simd128_splat<simd128_t>(_a);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_splat(uint16_t _a)
	{
		return simd128_splat<simd128_t>(_a);
	}

	BX_SIMD_FORCE_INLINE simd128_t simd128_zero()
	{
		return simd128_zero<simd128_t>();
	}

} // namespace bx

#include "../math.h"

