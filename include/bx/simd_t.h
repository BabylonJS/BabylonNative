/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#define BX_SIMD_T_H_HEADER_GUARD

#include "bx.h"

// Naming convention:
//
//  simd[register-width][_<lane-type><lane-type-width>]_<operation>[_<suffix>]
//
//  <> - not optional
//  [] - optional
//
//  register-width: 32, 64, 128, 256
//      (omitted for width-generic templates to operate on any available register width)
//
//  lane-type:
//      f - floating point
//      i - signed integer
//      u - unsigned integer
//      x - typeless bitwise
//
//  lane-type-width: 8, 16, 32, 64
//
//      +----+----+----+----+----+----+----+----+- ~ -+----+
//      | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 |  ~  | NN | bytes
//      +----+----+----+----+----+----+----+----+- ~ -+----+
//      |         register width 32, 64, 128, 256          |
//      +----+----+----+----+----+----+----+----+- ~  -----+
//      | u8 | u8 | u8 | u8 | u8 | u8 | u8 | u8 |  ~  ...  |
//      +----+----+----+----+----+----+----+----+- ~  -----+
//      |   u16   |   u16   |   u16   |   u16   |  ~  ...  |
//      +---------+---------+---------+---------+- ~  -----+
//      |        u32        |        u32        |  ~  ...  |
//      +-------------------+-------------------+- ~  -----+
//      |                  u64                  |  ~  ...  |
//      +---------------------------------------+- ~  -----+
//
//  suffix:
//      est  - Fast estimate (lower precision)
//      ni   - Not Intrinsic (software fallback)
//      nr   - Newton-Raphson refined
//      nr1  - One-iteration Newton-Raphson
//      xyz1 - Operates only on xyz, sets w=1

#define BX_SIMD_AVX       0
#define BX_SIMD_AVX2      0
#define BX_SIMD_LANGEXT   0
#define BX_SIMD_NEON      0
#define BX_SIMD_SSE       0
#define BX_SIMD_WASM      0
#define BX_SIMD_SUPPORTED 0

#if BX_COMPILER_GCC || BX_COMPILER_CLANG
#	undef  BX_SIMD_LANGEXT
#	define BX_SIMD_LANGEXT 1
#endif // BX_COMPILER_GCC || BX_COMPILER_CLANG

#if defined(__AVX2__)
#	include <immintrin.h>
#	undef  BX_SIMD_AVX2
#	define BX_SIMD_AVX2 1
#	undef  BX_SIMD_AVX
#	define BX_SIMD_AVX  1
#elif defined(__AVX__)
#	include <immintrin.h>
#	undef  BX_SIMD_AVX
#	define BX_SIMD_AVX  1
#endif //

#if defined(__SSE2__) || (BX_COMPILER_MSVC && (BX_ARCH_64BIT || _M_IX86_FP >= 2) )
#	include <emmintrin.h>
#	include <smmintrin.h> // SSE4.1  minspec is SSE4.2 so always available
#	if defined(__SSE4_2__) || BX_COMPILER_MSVC
#		include <nmmintrin.h>
#	endif
#	include <xmmintrin.h>
#	undef  BX_SIMD_SSE
#	define BX_SIMD_SSE 1
#elif defined(__ARM_NEON__) || defined(__ARM_NEON)
#	include <arm_neon.h>
#	undef  BX_SIMD_NEON
#	define BX_SIMD_NEON 1
#elif defined(__wasm_simd128__)
#	include <wasm_simd128.h>
#	undef  BX_SIMD_WASM
#	define BX_SIMD_WASM 1
#endif //

#if (  BX_SIMD_AVX  \
	|| BX_SIMD_NEON \
	|| BX_SIMD_SSE  \
	|| BX_SIMD_WASM \
	)
#	undef  BX_SIMD_SUPPORTED
#	define BX_SIMD_SUPPORTED 1
#endif // BX_SIMD_*

#define BX_SIMD_FORCE_INLINE BX_FORCE_INLINE
#define BX_SIMD_INLINE inline

namespace bx
{

#if BX_SIMD_SSE
	typedef __m128  simd128_sse_t;
#endif // BX_SIMD_SSE

#if BX_SIMD_NEON
	typedef float32x4_t simd128_neon_t;
#endif // BX_SIMD_NEON

#if BX_SIMD_AVX
	typedef __m256  simd256_avx_t;
#endif // BX_SIMD_AVX

#if BX_SIMD_WASM
	typedef v128_t simd128_wasm_t;
#endif // BX_SIMD_WASM

	BX_ALIGN_DECL(4,  struct) simd32_ref_t  { uint32_t u32; };
	BX_ALIGN_DECL(8,  struct) simd64_ref_t  { uint64_t u64; };
	BX_ALIGN_DECL(16, struct) simd128_ref_t { uint32_t u32[4]; };
	BX_ALIGN_DECL(32, struct) simd256_ref_t { simd128_ref_t lo; simd128_ref_t hi; };

#if BX_SIMD_SSE
	typedef simd128_sse_t  simd128_t;
#elif BX_SIMD_NEON
	typedef simd128_neon_t simd128_t;
#elif BX_SIMD_WASM
	typedef simd128_wasm_t simd128_t;
#else
	typedef simd128_ref_t  simd128_t;
#endif // BX_SIMD_*

#if BX_SIMD_AVX
	typedef simd256_avx_t  simd256_t;
#else
	typedef simd256_ref_t  simd256_t;
#endif // BX_SIMD_AVX

	typedef simd32_ref_t   simd32_t;
	typedef simd64_ref_t   simd64_t;

	// These deduce register width from the type parameter. Call with
	// simd128_t for 128-bit, simd256_t for 256-bit, etc.

	/// Per-lane f32 add: `_a + _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane sum.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_add(Ty _a, Ty _b);

	/// Per-lane f32 subtract: `_a - _b`.
	///
	/// @param[in] _a Minuend.
	/// @param[in] _b Subtrahend.
	///
	/// @returns Per-lane difference.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_sub(Ty _a, Ty _b);

	/// Per-lane f32 multiply: `_a * _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane product.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_mul(Ty _a, Ty _b);

	/// Per-lane f32 divide: `_a / _b`.
	///
	/// @param[in] _a Dividend.
	/// @param[in] _b Divisor.
	///
	/// @returns Per-lane quotient.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_div(Ty _a, Ty _b);

	/// Per-lane fused multiply-add: `_a * _b + _c`.
	///
	/// @param[in] _a Multiplicand.
	/// @param[in] _b Multiplier.
	/// @param[in] _c Addend.
	///
	/// @returns Per-lane result of `_a * _b + _c`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_madd(Ty _a, Ty _b, Ty _c);

	/// Per-lane fused multiply-subtract: `_a * _b - _c`.
	///
	/// @param[in] _a Multiplicand.
	/// @param[in] _b Multiplier.
	/// @param[in] _c Subtrahend.
	///
	/// @returns Per-lane result of `_a * _b - _c`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_msub(Ty _a, Ty _b, Ty _c);

	/// Extract sign bits from each 32-bit lane into an int bitmask.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Bitmask with one bit per 32-bit lane.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	int simd_x32_signbitsmask(Ty _a);

	/// Extract sign bits from each 8-bit lane into an int bitmask.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Bitmask with one bit per 8-bit lane.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	int simd_x8_signbitsmask(Ty _a);

	/// Per-byte shuffle within each 16-byte lane (single-source).
	///
	/// For each output byte, an index byte selects which input byte is copied:
	/// - If bit 7 of the index byte is set, the output byte is zero.
	/// - Otherwise, bits 0..3 select one of the 16 bytes within the same
	///   16-byte lane of `_a`. Bits 4..6 must be zero (reserved).
	///
	/// For widths < 16 bytes (simd32, simd64), the lane is the full register
	/// and the active selector bits are correspondingly fewer (2 bits for
	/// simd32, 3 bits for simd64).
	///
	/// @param[in] _a Source register.
	/// @param[in] _indices Per-byte selector indices.
	///
	/// @returns Byte-shuffled result.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_x8_shuffle(Ty _a, Ty _indices);

	/// Per-byte shuffle within each 16-byte lane (two-source).
	///
	/// For each output byte, an index byte selects which input byte is copied
	/// from the concatenation of `_a` and `_b`:
	/// - If bit 7 of the index byte is set, the output byte is zero.
	/// - Otherwise, bits 0..4 select one of the 32 bytes within the matching
	///   16-byte lanes of `_a` (low 16) and `_b` (high 16). Bits 5..6 must be
	///   zero (reserved).
	///
	/// For widths < 16 bytes (simd32, simd64), the lane is the full register
	/// and the active selector bits are correspondingly fewer.
	///
	/// @param[in] _a First source register (low half of concatenation).
	/// @param[in] _b Second source register (high half of concatenation).
	/// @param[in] _indices Per-byte selector indices.
	///
	/// @returns Byte-shuffled result.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_x8_shuffle(Ty _a, Ty _b, Ty _indices);

	/// Per-lane f32 minimum: `min(_a, _b)`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane minimum.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_min(Ty _a, Ty _b);

	/// Per-lane f32 maximum: `max(_a, _b)`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane maximum.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_max(Ty _a, Ty _b);

	/// Per-lane i32 add: `_a + _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane sum.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_add(Ty _a, Ty _b);

	/// Per-lane i32 subtract: `_a - _b`.
	///
	/// @param[in] _a Minuend.
	/// @param[in] _b Subtrahend.
	///
	/// @returns Per-lane difference.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_sub(Ty _a, Ty _b);

	/// Per-lane i32 negate: `-_a`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane negation.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_neg(Ty _a);

	/// Per-lane i32 absolute value: `abs(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane absolute value.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_abs(Ty _a);

	/// Per-lane i32 multiply (low 32 bits of 32x32 product).
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane low 32 bits of `_a * _b`. The result is bit-identical
	///   for signed and unsigned operands.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_mul(Ty _a, Ty _b);

	/// Per-lane unsigned 8-bit saturating add: `min(_a + _b, 0xff)`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane saturated sum.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_u8_satadd(Ty _a, Ty _b);

	/// Per-lane unsigned 8-bit saturating subtract: `max(_a - _b, 0)`.
	///
	/// @param[in] _a Minuend.
	/// @param[in] _b Subtrahend.
	///
	/// @returns Per-lane saturated difference.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_u8_satsub(Ty _a, Ty _b);

	/// Per-lane unsigned 16-bit saturating add: `min(_a + _b, 0xffff)`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane saturated sum.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_u16_satadd(Ty _a, Ty _b);

	/// Per-lane unsigned 16-bit saturating subtract: `max(_a - _b, 0)`.
	///
	/// @param[in] _a Minuend.
	/// @param[in] _b Subtrahend.
	///
	/// @returns Per-lane saturated difference.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_u16_satsub(Ty _a, Ty _b);

	/// Bitwise AND: `_a & _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Bitwise AND of all bits.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_and(Ty _a, Ty _b);

	/// Bitwise OR: `_a | _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Bitwise OR of all bits.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_or(Ty _a, Ty _b);

	/// Bitwise XOR: `_a ^ _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Bitwise XOR of all bits.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_xor(Ty _a, Ty _b);

	/// Per-lane negative multiply-subtract: `_c - _a * _b`.
	///
	/// @param[in] _a Multiplicand.
	/// @param[in] _b Multiplier.
	/// @param[in] _c Value to subtract from.
	///
	/// @returns Per-lane result of `_c - _a * _b`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_nmsub(Ty _a, Ty _b, Ty _c);

	/// Per-lane f32 negate: `-_a`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane negation.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_neg(Ty _a);

	/// Per-lane f32 absolute value: `abs(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane absolute value.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_abs(Ty _a);

	/// Per-lane f32 reciprocal estimate: `~1/_a`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane approximate reciprocal.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_rcp_est(Ty _a);

	/// Per-lane f32 reciprocal square root estimate: `~1/sqrt(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane approximate reciprocal square root.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_rsqrt_est(Ty _a);

	/// Per-lane f32 square root: `sqrt(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane square root.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_sqrt(Ty _a);

	/// Per-lane f32 reciprocal square root: `1/sqrt(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane reciprocal square root.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_rsqrt(Ty _a);

	/// Per-lane f32 divide via Newton-Raphson: `_a / _b`.
	///
	/// @param[in] _a Dividend.
	/// @param[in] _b Divisor.
	///
	/// @returns Per-lane quotient (NR-refined).
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_div_nr(Ty _a, Ty _b);

	/// Per-lane f32 square root via Newton-Raphson: `sqrt(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane square root (NR-refined).
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_sqrt_nr(Ty _a);

	/// Per-lane f32 square root via Newton-Raphson (1 iteration).
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane square root (1 NR iteration).
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_sqrt_nr1(Ty _a);

	/// Per-lane f32 reciprocal square root via Newton-Raphson.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane reciprocal square root (NR-refined).
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_rsqrt_nr(Ty _a);

	/// Per-lane f32 reciprocal square root via Carmack's method.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane reciprocal square root (Carmack).
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_rsqrt_carmack(Ty _a);

	/// Per-lane f32 compare equal. Lanes set to all-ones if equal, zero otherwise.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_cmpeq(Ty _a, Ty _b);

	/// Per-lane f32 compare less-than. Lanes set to all-ones if `_a < _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_cmplt(Ty _a, Ty _b);

	/// Per-lane f32 compare greater-than. Lanes set to all-ones if `_a > _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_cmpgt(Ty _a, Ty _b);

	/// Per-lane f32 compare not-equal. Lanes set to all-ones if `_a != _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_cmpneq(Ty _a, Ty _b);

	/// Per-lane f32 compare less-or-equal. Lanes set to all-ones if `_a <= _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_cmple(Ty _a, Ty _b);

	/// Per-lane f32 compare greater-or-equal. Lanes set to all-ones if `_a >= _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_cmpge(Ty _a, Ty _b);

	/// Per-lane f32 clamp: `min(max(_a, _min), _max)`.
	///
	/// @param[in] _a Value to clamp.
	/// @param[in] _min Minimum bound.
	/// @param[in] _max Maximum bound.
	///
	/// @returns Per-lane clamped value.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_clamp(Ty _a, Ty _min, Ty _max);

	/// Per-lane f32 linear interpolation: `_a + (_b - _a) * _s`.
	///
	/// @param[in] _a Start value.
	/// @param[in] _b End value.
	/// @param[in] _s Interpolation factor.
	///
	/// @returns Per-lane interpolated value.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_lerp(Ty _a, Ty _b, Ty _s);

	/// Per-lane f32 reciprocal: `1/_a`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane reciprocal.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_rcp(Ty _a);

	/// Per-lane f32 round to nearest integer.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane rounded value.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_round(Ty _a);

	/// Per-lane f32 ceiling.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane ceiling value.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_ceil(Ty _a);

	/// Per-lane f32 floor.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane floor value.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_floor(Ty _a);

	/// Per-lane f32 base-2 logarithm: `log2(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane log2 value.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_log2(Ty _a);

	/// Per-lane f32 base-2 exponential: `2^_a`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane 2^_a.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_exp2(Ty _a);

	/// Per-lane f32 power: `_a^_b`.
	///
	/// @param[in] _a Base.
	/// @param[in] _b Exponent.
	///
	/// @returns Per-lane power.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_pow(Ty _a, Ty _b);

	/// Per-lane f32 cosine.
	///
	/// @param[in] _a Angle in radians.
	///
	/// @returns Per-lane cosine.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_cos(Ty _a);

	/// Per-lane f32 sine.
	///
	/// @param[in] _a Angle in radians.
	///
	/// @returns Per-lane sine.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_sin(Ty _a);

	/// Per-lane f32 natural logarithm: `ln(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane natural logarithm.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_log(Ty _a);

	/// Per-lane f32 natural exponential: `e^_a`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane e^_a.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_exp(Ty _a);

	/// Per-lane f32 ldexp: `_a * 2^_b`.
	///
	/// @param[in] _a Significand.
	/// @param[in] _b Exponent (as float).
	///
	/// @returns Per-lane result of `_a * 2^_b`.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_ldexp(Ty _a, Ty _b);

	/// Per-lane f32 to i32 conversion with truncation toward zero.
	///
	/// @param[in] _a Input f32 register.
	///
	/// @returns Per-lane integer (as bit pattern).
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_ftoi_trunc(Ty _a);

	/// Per-lane f32 to i32 conversion with round-to-nearest (ties to even).
	///
	/// @param[in] _a Input f32 register.
	///
	/// @returns Per-lane integer (as bit pattern).
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f32_ftoi_round(Ty _a);

	/// Per-lane i32 to f32 conversion.
	///
	/// @param[in] _a Input i32 register.
	///
	/// @returns Per-lane float value.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_itof(Ty _a);

	/// Per-lane f64 add: `_a + _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane sum.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_add(Ty _a, Ty _b);

	/// Per-lane f64 subtract: `_a - _b`.
	///
	/// @param[in] _a Minuend.
	/// @param[in] _b Subtrahend.
	///
	/// @returns Per-lane difference.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_sub(Ty _a, Ty _b);

	/// Per-lane f64 multiply: `_a * _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane product.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_mul(Ty _a, Ty _b);

	/// Per-lane f64 divide: `_a / _b`.
	///
	/// @param[in] _a Dividend.
	/// @param[in] _b Divisor.
	///
	/// @returns Per-lane quotient.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_div(Ty _a, Ty _b);

	/// Per-lane f64 fused multiply-add: `_a * _b + _c`.
	///
	/// @param[in] _a Multiplicand.
	/// @param[in] _b Multiplier.
	/// @param[in] _c Addend.
	///
	/// @returns Per-lane result of `_a * _b + _c`.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_madd(Ty _a, Ty _b, Ty _c);

	/// Per-lane f64 negative multiply-subtract: `_c - _a * _b`.
	///
	/// @param[in] _a Multiplicand.
	/// @param[in] _b Multiplier.
	/// @param[in] _c Value to subtract from.
	///
	/// @returns Per-lane result of `_c - _a * _b`.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_nmsub(Ty _a, Ty _b, Ty _c);

	/// Per-lane f64 negate: `-_a`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane negation.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_neg(Ty _a);

	/// Per-lane f64 absolute value: `abs(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane absolute value.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_abs(Ty _a);

	/// Per-lane f64 minimum: `min(_a, _b)`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane minimum.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_min(Ty _a, Ty _b);

	/// Per-lane f64 maximum: `max(_a, _b)`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane maximum.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_max(Ty _a, Ty _b);

	/// Per-lane f64 clamp: `min(max(_a, _min), _max)`.
	///
	/// @param[in] _a Value to clamp.
	/// @param[in] _min Minimum bound.
	/// @param[in] _max Maximum bound.
	///
	/// @returns Per-lane clamped value.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_clamp(Ty _a, Ty _min, Ty _max);

	/// Per-lane f64 linear interpolation: `_a + (_b - _a) * _s`.
	///
	/// @param[in] _a Start value.
	/// @param[in] _b End value.
	/// @param[in] _s Interpolation factor.
	///
	/// @returns Per-lane interpolated value.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_lerp(Ty _a, Ty _b, Ty _s);

	/// Per-lane f64 reciprocal: `1/_a`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane reciprocal.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_rcp(Ty _a);

	/// Per-lane f64 square root: `sqrt(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane square root.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_sqrt(Ty _a);

	/// Per-lane f64 reciprocal square root: `1/sqrt(_a)`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane reciprocal square root.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_rsqrt(Ty _a);

	/// Per-lane f64 round to nearest integer.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane rounded value.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_round(Ty _a);

	/// Per-lane f64 ceiling.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane ceiling value.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_ceil(Ty _a);

	/// Per-lane f64 floor.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Per-lane floor value.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_floor(Ty _a);

	/// Per-lane f64 compare equal. Lanes set to all-ones if equal.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_cmpeq(Ty _a, Ty _b);

	/// Per-lane f64 compare not-equal. Lanes set to all-ones if `_a != _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_cmpneq(Ty _a, Ty _b);

	/// Per-lane f64 compare less-than. Lanes set to all-ones if `_a < _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_cmplt(Ty _a, Ty _b);

	/// Per-lane f64 compare less-or-equal. Lanes set to all-ones if `_a <= _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_cmple(Ty _a, Ty _b);

	/// Per-lane f64 compare greater-than. Lanes set to all-ones if `_a > _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_cmpgt(Ty _a, Ty _b);

	/// Per-lane f64 compare greater-or-equal. Lanes set to all-ones if `_a >= _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_f64_cmpge(Ty _a, Ty _b);

	/// Per-lane i32 compare less-than. Lanes set to all-ones if `_a < _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_cmplt(Ty _a, Ty _b);

	/// Per-lane i32 compare greater-than. Lanes set to all-ones if `_a > _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_cmpgt(Ty _a, Ty _b);

	/// Per-lane i32 minimum: `min(_a, _b)`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane minimum.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_min(Ty _a, Ty _b);

	/// Per-lane i32 maximum: `max(_a, _b)`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane maximum.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_max(Ty _a, Ty _b);

	/// Per-lane i32 clamp: `min(max(_a, _min), _max)`.
	///
	/// @param[in] _a Value to clamp.
	/// @param[in] _min Minimum bound.
	/// @param[in] _max Maximum bound.
	///
	/// @returns Per-lane clamped value.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_clamp(Ty _a, Ty _min, Ty _max);

	/// Per-lane i32 compare equal. Lanes set to all-ones if equal.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_i32_cmpeq(Ty _a, Ty _b);

	/// Per-lane u32 add: `_a + _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane sum.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_u32_add(Ty _a, Ty _b);

	/// Per-lane u32 subtract: `_a - _b`.
	///
	/// @param[in] _a Minuend.
	/// @param[in] _b Subtrahend.
	///
	/// @returns Per-lane difference.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_u32_sub(Ty _a, Ty _b);

	/// Per-lane u32 compare less-than. Lanes set to all-ones if `_a < _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_u32_cmplt(Ty _a, Ty _b);

	/// Per-lane u32 compare greater-than. Lanes set to all-ones if `_a > _b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand.
	///
	/// @returns Per-lane comparison mask.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_u32_cmpgt(Ty _a, Ty _b);

	/// Broadcast float value to all lanes.
	///
	/// @param[in] _a Value to broadcast.
	///
	/// @returns Register with all lanes set to `_a`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_splat(float _a);

	/// Broadcast uint32 value to all 32-bit lanes.
	///
	/// @param[in] _a Value to broadcast.
	///
	/// @returns Register with all lanes set to `_a`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_splat(uint32_t _a);

	/// Broadcast double value to all 64-bit lanes.
	///
	/// @param[in] _a Value to broadcast.
	///
	/// @returns Register with all 64-bit lanes set to `_a`.
	///
	/// @remark Widths: simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_splat(double _a);

	/// Zero all lanes.
	///
	/// @returns Register with all bits zero.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_zero();

	/// Select by bit mask: per-bit `_mask ? _a : _b`.
	///
	/// @param[in] _mask Bit mask.
	/// @param[in] _a Value selected where mask bits are 1.
	/// @param[in] _b Value selected where mask bits are 0.
	///
	/// @returns Per-bit blend of `_a` and `_b`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_selb(Ty _mask, Ty _a, Ty _b);

	/// Select by sign: per-lane `_test < 0 ? _a : _b`.
	///
	/// @param[in] _test Sign test value.
	/// @param[in] _a Value selected where sign bit is 1.
	/// @param[in] _b Value selected where sign bit is 0.
	///
	/// @returns Per-lane selection based on sign.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_sels(Ty _test, Ty _a, Ty _b);

	/// Test if any lane sign bit is set.
	///
	/// @param[in] _test Input register.
	///
	/// @returns True if any lane has sign bit set.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	bool simd_test_any(Ty _test);

	/// Test if all lane sign bits are set.
	///
	/// @param[in] _test Input register.
	///
	/// @returns True if all lanes have sign bit set.
	///
	/// @remark Widths: simd128, simd256.
	///
	template<typename Ty>
	bool simd_test_all(Ty _test);

	/// Test if `(_a & _b)` is all-zero bits.
	///
	/// @param[in] _a First input register.
	/// @param[in] _b Second input register.
	///
	/// @returns True if every bit of `(_a & _b)` is zero.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	bool simd_test_zero(Ty _a, Ty _b);

	/// Bitwise NOT: `~_a`.
	///
	/// @param[in] _a Input register.
	///
	/// @returns Bitwise complement.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_not(Ty _a);

	/// Bitwise AND-complement: `_a & ~_b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand (complemented).
	///
	/// @returns Bitwise `_a & ~_b`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_andc(Ty _a, Ty _b);

	/// Per-lane 32-bit shift right arithmetic.
	///
	/// @param[in] _a Input register.
	/// @param[in] _count Number of bits to shift.
	///
	/// @returns Per-lane arithmetic right shift.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_x32_sra(Ty _a, int _count);

	/// Per-lane 32-bit shift right arithmetic with per-lane variable count.
	///
	/// @param[in] _a Input register.
	/// @param[in] _count Per-lane shift counts.
	///
	/// @returns Per-lane arithmetic right shift, where each lane is shifted by the
	///   matching lane in `_count`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_x32_sra(Ty _a, Ty _count);

	/// Per-lane 32-bit shift right logical.
	///
	/// @param[in] _a Input register.
	/// @param[in] _count Number of bits to shift.
	///
	/// @returns Per-lane logical right shift.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_x32_srl(Ty _a, int _count);

	/// Per-lane 32-bit shift right logical with per-lane variable count.
	///
	/// @param[in] _a Input register.
	/// @param[in] _count Per-lane shift counts.
	///
	/// @returns Per-lane logical right shift, where each lane is shifted by the
	///   matching lane in `_count`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_x32_srl(Ty _a, Ty _count);

	/// Per-lane 32-bit shift left logical.
	///
	/// @param[in] _a Input register.
	/// @param[in] _count Number of bits to shift.
	///
	/// @returns Per-lane logical left shift.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_x32_sll(Ty _a, int _count);

	/// Per-lane 32-bit shift left logical with per-lane variable count.
	///
	/// @param[in] _a Input register.
	/// @param[in] _count Per-lane shift counts.
	///
	/// @returns Per-lane logical left shift, where each lane is shifted by the
	///   matching lane in `_count`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_x32_sll(Ty _a, Ty _count);

	/// Bitwise OR-complement: `_a | ~_b`.
	///
	/// @param[in] _a First operand.
	/// @param[in] _b Second operand (complemented).
	///
	/// @returns Bitwise `_a | ~_b`.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_orc(Ty _a, Ty _b);

	/// Load from aligned memory.
	///
	/// @param[in] _ptr Pointer to aligned source data.
	///
	/// @returns Register loaded from memory.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_ld(const void* _ptr);

	/// Load from unaligned memory.
	///
	/// @param[in] _ptr Pointer to source data (any alignment).
	///
	/// @returns Register loaded from memory.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	Ty simd_ldu(const void* _ptr);

	/// Store to aligned memory.
	///
	/// @param[out] _ptr Pointer to aligned destination.
	/// @param[in] _a Register to store.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	void simd_st(void* _ptr, Ty _a);

	/// Store to unaligned memory.
	///
	/// @param[out] _ptr Pointer to destination (any alignment).
	/// @param[in] _a Register to store.
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	void simd_stu(void* _ptr, Ty _a);

	/// Store lowest 32-bit element to memory.
	///
	/// @param[out] _ptr Pointer to destination.
	/// @param[in] _a Register (lowest lane stored).
	///
	/// @remark Widths: simd32, simd64, simd128, simd256.
	///
	template<typename Ty>
	void simd_x32_st1(void* _ptr, Ty _a);

} // namespace bx

#include "inline/simd_impl.inl"

#endif // BX_SIMD_T_H_HEADER_GUARD
