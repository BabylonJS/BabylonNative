/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_SIMD_T_H_HEADER_GUARD
#	error "Must be included from bx/simd_t.h!"
#endif // BX_SIMD_T_H_HEADER_GUARD

namespace bx
{
	BX_ALIGN_DECL(8, struct)  simd64_f64_ref_t   { double   f64; };
	BX_ALIGN_DECL(8, struct)  simd64_i64_ref_t   { int64_t  i64; };
	BX_ALIGN_DECL(8, struct)  simd64_f32_ref_t { float    f32[2]; };
	BX_ALIGN_DECL(8, struct)  simd64_i32_ref_t { int32_t  i32[2]; };
	BX_ALIGN_DECL(8, struct)  simd64_u32_ref_t { uint32_t u32[2]; };

#if BX_SIMD_LANGEXT
	typedef float    simd64_f32_langext_t __attribute__((__vector_size__(8), __aligned__(8)));
	typedef int32_t  simd64_i32_langext_t __attribute__((__vector_size__(8), __aligned__(8)));
	typedef uint32_t simd64_u32_langext_t __attribute__((__vector_size__(8), __aligned__(8)));
#endif // BX_SIMD_LANGEXT

	inline BX_CONSTEXPR_FUNC double simd_floor(double _x)
	{
		const uint64_t xi = bitCast<uint64_t>(_x);
		const int32_t  exp = int32_t((xi >> 52) & 0x7ff) - 1023;

		if (exp < 0)
		{
			return (xi >> 63) ? -1.0 : 0.0;
		}

		if (exp >= 52)
		{
			return _x;
		}

		const uint64_t mask = (UINT64_C(1) << (52 - exp)) - 1;

		if (0 == (xi & mask))
		{
			return _x;
		}

		const uint64_t trunc = xi & ~mask;
		return (xi >> 63)
			? bitCast<double>(trunc) - 1.0
			: bitCast<double>(trunc)
			;
	}

	inline BX_CONSTEXPR_FUNC double simd_ceil(double _x)
	{
		return -simd_floor(-_x);
	}

	inline BX_CONSTEXPR_FUNC double simd_round(double _x)
	{
		return (bitCast<uint64_t>(_x) >> 63)
			? -simd_floor(-_x + 0.5)
			:  simd_floor( _x + 0.5)
			;
	}

	inline BX_CONSTEXPR_FUNC double simd_sqrt(double _x)
	{
		if (_x <= 0.0)
		{
			return 0.0;
		}

		uint64_t ii = bitCast<uint64_t>(_x);
		ii = UINT64_C(0x5fe6eb50c7b537a9) - (ii >> 1);

		double yy = bitCast<double>(ii);
		yy = yy * (1.5 - 0.5 * _x * yy * yy);
		yy = yy * (1.5 - 0.5 * _x * yy * yy);
		yy = yy * (1.5 - 0.5 * _x * yy * yy);
		yy = yy * (1.5 - 0.5 * _x * yy * yy);

		return _x * yy;
	}

	BX_SIMD_FORCE_INLINE simd64_t simd64_ld(const void* _ptr)
	{
		simd64_t result;
		memCopy(&result, _ptr, sizeof(simd64_t) );
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_ld(float _x, float _y)
	{
		return { .u64 = uint64_t(bitCast<uint32_t>(_x)) | (uint64_t(bitCast<uint32_t>(_y)) << 32) };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_ld(uint32_t _x, uint32_t _y)
	{
		return { .u64 = uint64_t(_x) | (uint64_t(_y) << 32) };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_ld(int32_t _x, int32_t _y)
	{
		return { .u64 = uint64_t(uint32_t(_x)) | (uint64_t(uint32_t(_y)) << 32) };
	}

	BX_SIMD_FORCE_INLINE void simd64_st(void* _ptr, simd64_t _a)
	{
		memCopy(_ptr, &_a, sizeof(simd64_t) );
	}

	BX_SIMD_FORCE_INLINE void simd64_x32_st1(void* _ptr, simd64_t _a)
	{
		uint32_t* result = reinterpret_cast<uint32_t*>(_ptr);
		*result = uint32_t(_a.u64);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_zero()
	{
		return { .u64 = 0 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(float _a)
	{
		const uint32_t bits = bitCast<uint32_t>(_a);
		return { .u64 = uint64_t(bits) | (uint64_t(bits) << 32) };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(uint32_t _a)
	{
		return { .u64 = uint64_t(_a) | (uint64_t(_a) << 32) };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(int32_t _a)
	{
		return simd64_splat(uint32_t(_a) );
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(uint64_t _a)
	{
		return { .u64 = _a };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(int64_t _a)
	{
		return { .u64 = uint64_t(_a) };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(double _a)
	{
		const simd64_f64_ref_t result = { .f64 = _a };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(uint16_t _a)
	{
		const uint64_t val = uint64_t(_a);
		return { .u64 = (val << 48) | (val << 32) | (val << 16) | val };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(int16_t _a)
	{
		return simd64_splat(uint16_t(_a) );
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(uint8_t _a)
	{
		const uint64_t val  = uint64_t(_a);
		const uint64_t tmp0 = (val  << 8 ) | val;
		const uint64_t tmp1 = (tmp0 << 16) | tmp0;
		const uint64_t tmp2 = (tmp1 << 32) | tmp1;
		return { .u64 = tmp2 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_splat(int8_t _a)
	{
		return simd64_splat(uint8_t(_a) );
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_add(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		const simd64_f64_ref_t result = { .f64 = a.f64 + b.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_sub(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		const simd64_f64_ref_t result = { .f64 = a.f64 - b.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_mul(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		const simd64_f64_ref_t result = { .f64 = a.f64 * b.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_div(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		const simd64_f64_ref_t result = { .f64 = a.f64 / b.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_madd(simd64_t _a, simd64_t _b, simd64_t _c)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		const simd64_f64_ref_t c = bitCast<simd64_f64_ref_t>(_c);
		const simd64_f64_ref_t result = { .f64 = a.f64 * b.f64 + c.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_nmsub(simd64_t _a, simd64_t _b, simd64_t _c)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		const simd64_f64_ref_t c = bitCast<simd64_f64_ref_t>(_c);
		const simd64_f64_ref_t result = { .f64 = c.f64 - a.f64 * b.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_neg(simd64_t _a)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t result = { .f64 = -a.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_abs(simd64_t _a)
	{
		return { .u64 = _a.u64 & (kDoubleExponentMask | kDoubleMantissaMask) };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_min(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		const simd64_f64_ref_t result = { .f64 = a.f64 < b.f64 ? a.f64 : b.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_max(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		const simd64_f64_ref_t result = { .f64 = a.f64 > b.f64 ? a.f64 : b.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_clamp(simd64_t _a, simd64_t _min, simd64_t _max)
	{
		return simd64_f64_min(simd64_f64_max(_a, _min), _max);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_lerp(simd64_t _a, simd64_t _b, simd64_t _s)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		const simd64_f64_ref_t s = bitCast<simd64_f64_ref_t>(_s);
		const simd64_f64_ref_t result = { .f64 = a.f64 + (b.f64 - a.f64) * s.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_rcp(simd64_t _a)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t result = { .f64 = 1.0 / a.f64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_sqrt(simd64_t _a)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t result = { .f64 = simd_sqrt(a.f64) };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_rsqrt(simd64_t _a)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t result = { .f64 = 1.0 / simd_sqrt(a.f64) };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_round(simd64_t _a)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t result = { .f64 = simd_round(a.f64) };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_ceil(simd64_t _a)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t result = { .f64 = simd_ceil(a.f64) };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_floor(simd64_t _a)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t result = { .f64 = simd_floor(a.f64) };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmpeq(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		return { .u64 = a.f64 == b.f64 ? UINT64_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmpneq(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		return { .u64 = a.f64 != b.f64 ? UINT64_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmplt(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		return { .u64 = a.f64 < b.f64 ? UINT64_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmple(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		return { .u64 = a.f64 <= b.f64 ? UINT64_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmpgt(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		return { .u64 = a.f64 > b.f64 ? UINT64_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f64_cmpge(simd64_t _a, simd64_t _b)
	{
		const simd64_f64_ref_t a = bitCast<simd64_f64_ref_t>(_a);
		const simd64_f64_ref_t b = bitCast<simd64_f64_ref_t>(_b);
		return { .u64 = a.f64 >= b.f64 ? UINT64_MAX : 0 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_add(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) + bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t result = { { a.f32[0] + b.f32[0], a.f32[1] + b.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_sub(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) - bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t result = { { a.f32[0] - b.f32[0], a.f32[1] - b.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_mul(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) * bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t result = { { a.f32[0] * b.f32[0], a.f32[1] * b.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_div(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) / bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t result = { { a.f32[0] / b.f32[0], a.f32[1] / b.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_min(simd64_t _a, simd64_t _b)
	{
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t result = { { a.f32[0] < b.f32[0] ? a.f32[0] : b.f32[0], a.f32[1] < b.f32[1] ? a.f32[1] : b.f32[1] } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_max(simd64_t _a, simd64_t _b)
	{
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t result = { { a.f32[0] > b.f32[0] ? a.f32[0] : b.f32[0], a.f32[1] > b.f32[1] ? a.f32[1] : b.f32[1] } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_madd(simd64_t _a, simd64_t _b, simd64_t _c)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) * bitCast<simd64_f32_langext_t>(_b) + bitCast<simd64_f32_langext_t>(_c));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t c = bitCast<simd64_f32_ref_t>(_c);
		const simd64_f32_ref_t result = { { a.f32[0] * b.f32[0] + c.f32[0], a.f32[1] * b.f32[1] + c.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_msub(simd64_t _a, simd64_t _b, simd64_t _c)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) * bitCast<simd64_f32_langext_t>(_b) - bitCast<simd64_f32_langext_t>(_c));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t c = bitCast<simd64_f32_ref_t>(_c);
		const simd64_f32_ref_t result = { { a.f32[0] * b.f32[0] - c.f32[0], a.f32[1] * b.f32[1] - c.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_nmsub(simd64_t _a, simd64_t _b, simd64_t _c)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_c) - bitCast<simd64_f32_langext_t>(_a) * bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t c = bitCast<simd64_f32_ref_t>(_c);
		const simd64_f32_ref_t result = { { c.f32[0] - a.f32[0] * b.f32[0], c.f32[1] - a.f32[1] * b.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC int simd64_x32_signbitsmask(simd64_t _a)
	{
		return int(uint32_t(_a.u64) >> 31) | (int(uint32_t(_a.u64 >> 32) >> 31) << 1);
	}

	inline BX_CONSTEXPR_FUNC int simd64_x8_signbitsmask(simd64_t _a)
	{
		int result = 0;
		for (int ii = 0; ii < 8; ++ii)
		{
			result |= (int((_a.u64 >> (ii * 8 + 7)) & 1) << ii);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x8_shuffle(simd64_t _a, simd64_t _indices)
	{
		uint64_t out = 0;
		for (uint64_t ii = 0; ii < 8; ++ii)
		{
			const uint64_t idx = (_indices.u64 >> (ii*8) ) & 0xffu;
			const uint64_t byte = (idx & 0x80u)
				? uint64_t(0)
				: (_a.u64 >> ( (idx & 0x07u) * 8) ) & 0xffu;
			out |= byte << (ii*8);
		}
		return { .u64 = out };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x8_shuffle(simd64_t _a, simd64_t _b, simd64_t _indices)
	{
		uint64_t out = 0;
		for (uint64_t ii = 0; ii < 8; ++ii)
		{
			const uint64_t idx = (_indices.u64 >> (ii*8) ) & 0xffu;
			uint64_t byte = 0;
			if (0 == (idx & 0x80u) )
			{
				const uint64_t sel = idx & 0x0fu;
				const uint64_t src = sel < 8u ? _a.u64 : _b.u64;
				byte = (src >> ( (sel & 0x07u) * 8) ) & 0xffu;
			}
			out |= byte << (ii*8);
		}
		return { .u64 = out };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_neg(simd64_t _a)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(-bitCast<simd64_f32_langext_t>(_a));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t result = { { -a.f32[0], -a.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_abs(simd64_t _a)
	{
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t result = { { a.u32[0] & (kFloatExponentMask | kFloatMantissaMask), a.u32[1] & (kFloatExponentMask | kFloatMantissaMask) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_clamp(simd64_t _a, simd64_t _min, simd64_t _max)
	{
		return simd64_f32_min(simd64_f32_max(_a, _min), _max);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_lerp(simd64_t _a, simd64_t _b, simd64_t _s)
	{
#if BX_SIMD_LANGEXT
		const simd64_f32_langext_t a = bitCast<simd64_f32_langext_t>(_a);
		return bitCast<simd64_t>(a + (bitCast<simd64_f32_langext_t>(_b) - a) * bitCast<simd64_f32_langext_t>(_s));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_f32_ref_t s = bitCast<simd64_f32_ref_t>(_s);
		const simd64_f32_ref_t result = { { a.f32[0] + (b.f32[0] - a.f32[0]) * s.f32[0], a.f32[1] + (b.f32[1] - a.f32[1]) * s.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_rcp(simd64_t _a)
	{
#if BX_SIMD_LANGEXT
		const simd64_f32_langext_t one = {1.0f, 1.0f};
		return bitCast<simd64_t>(one / bitCast<simd64_f32_langext_t>(_a));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t result = { { 1.0f / a.f32[0], 1.0f / a.f32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_sqrt(simd64_t _a)
	{
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t result = { { sqrt(a.f32[0]), sqrt(a.f32[1]) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_rsqrt(simd64_t _a)
	{
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t result = { { 1.0f / sqrt(a.f32[0]), 1.0f / sqrt(a.f32[1]) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_round(simd64_t _a)
	{
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t result = { { round(a.f32[0]), round(a.f32[1]) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_ceil(simd64_t _a)
	{
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t result = { { ceil(a.f32[0]), ceil(a.f32[1]) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_floor(simd64_t _a)
	{
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t result = { { floor(a.f32[0]), floor(a.f32[1]) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmpeq(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) == bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.f32[0] == b.f32[0] ? 0xffffffff : 0u, a.f32[1] == b.f32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmpneq(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) != bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.f32[0] != b.f32[0] ? 0xffffffff : 0u, a.f32[1] != b.f32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmplt(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) < bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.f32[0] < b.f32[0] ? 0xffffffff : 0u, a.f32[1] < b.f32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmple(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) <= bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.f32[0] <= b.f32[0] ? 0xffffffff : 0u, a.f32[1] <= b.f32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmpgt(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) > bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.f32[0] > b.f32[0] ? 0xffffffff : 0u, a.f32[1] > b.f32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_cmpge(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_f32_langext_t>(_a) >= bitCast<simd64_f32_langext_t>(_b));
#else
		const simd64_f32_ref_t a = bitCast<simd64_f32_ref_t>(_a);
		const simd64_f32_ref_t b = bitCast<simd64_f32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.f32[0] >= b.f32[0] ? 0xffffffff : 0u, a.f32[1] >= b.f32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_add(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_i32_langext_t>(_a) + bitCast<simd64_i32_langext_t>(_b));
#else
		const simd64_i32_ref_t a = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t b = bitCast<simd64_i32_ref_t>(_b);
		const simd64_i32_ref_t result = { { a.i32[0] + b.i32[0], a.i32[1] + b.i32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_sub(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_i32_langext_t>(_a) - bitCast<simd64_i32_langext_t>(_b));
#else
		const simd64_i32_ref_t a = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t b = bitCast<simd64_i32_ref_t>(_b);
		const simd64_i32_ref_t result = { { a.i32[0] - b.i32[0], a.i32[1] - b.i32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_neg(simd64_t _a)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(-bitCast<simd64_i32_langext_t>(_a));
#else
		const simd64_i32_ref_t a = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t result = { { -a.i32[0], -a.i32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_abs(simd64_t _a)
	{
		const simd64_i32_ref_t a = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t result = { { a.i32[0] < 0 ? -a.i32[0] : a.i32[0], a.i32[1] < 0 ? -a.i32[1] : a.i32[1] } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_min(simd64_t _a, simd64_t _b)
	{
		const simd64_i32_ref_t a = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t b = bitCast<simd64_i32_ref_t>(_b);
		const simd64_i32_ref_t result = { { a.i32[0] < b.i32[0] ? a.i32[0] : b.i32[0], a.i32[1] < b.i32[1] ? a.i32[1] : b.i32[1] } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_max(simd64_t _a, simd64_t _b)
	{
		const simd64_i32_ref_t a = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t b = bitCast<simd64_i32_ref_t>(_b);
		const simd64_i32_ref_t result = { { a.i32[0] > b.i32[0] ? a.i32[0] : b.i32[0], a.i32[1] > b.i32[1] ? a.i32[1] : b.i32[1] } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_clamp(simd64_t _a, simd64_t _min, simd64_t _max)
	{
		return simd64_i32_min(simd64_i32_max(_a, _min), _max);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_cmpeq(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_i32_langext_t>(_a) == bitCast<simd64_i32_langext_t>(_b));
#else
		const simd64_i32_ref_t a = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t b = bitCast<simd64_i32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.i32[0] == b.i32[0] ? 0xffffffff : 0u, a.i32[1] == b.i32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_cmplt(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_i32_langext_t>(_a) < bitCast<simd64_i32_langext_t>(_b));
#else
		const simd64_i32_ref_t a = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t b = bitCast<simd64_i32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.i32[0] < b.i32[0] ? 0xffffffff : 0u, a.i32[1] < b.i32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_cmpgt(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_i32_langext_t>(_a) > bitCast<simd64_i32_langext_t>(_b));
#else
		const simd64_i32_ref_t a = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t b = bitCast<simd64_i32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.i32[0] > b.i32[0] ? 0xffffffff : 0u, a.i32[1] > b.i32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_add(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_u32_langext_t>(_a) + bitCast<simd64_u32_langext_t>(_b));
#else
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] + b.u32[0], a.u32[1] + b.u32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_sub(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_u32_langext_t>(_a) - bitCast<simd64_u32_langext_t>(_b));
#else
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] - b.u32[0], a.u32[1] - b.u32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_mul(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_u32_langext_t>(_a) * bitCast<simd64_u32_langext_t>(_b));
#else
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] * b.u32[0], a.u32[1] * b.u32[1] } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_min(simd64_t _a, simd64_t _b)
	{
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] < b.u32[0] ? a.u32[0] : b.u32[0], a.u32[1] < b.u32[1] ? a.u32[1] : b.u32[1] } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_max(simd64_t _a, simd64_t _b)
	{
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] > b.u32[0] ? a.u32[0] : b.u32[0], a.u32[1] > b.u32[1] ? a.u32[1] : b.u32[1] } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_clamp(simd64_t _a, simd64_t _min, simd64_t _max)
	{
		return simd64_u32_min(simd64_u32_max(_a, _min), _max);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmpeq(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_u32_langext_t>(_a) == bitCast<simd64_u32_langext_t>(_b));
#else
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] == b.u32[0] ? 0xffffffff : 0u, a.u32[1] == b.u32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmpneq(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_u32_langext_t>(_a) != bitCast<simd64_u32_langext_t>(_b));
#else
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] != b.u32[0] ? 0xffffffff : 0u, a.u32[1] != b.u32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmplt(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_u32_langext_t>(_a) < bitCast<simd64_u32_langext_t>(_b));
#else
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] < b.u32[0] ? 0xffffffff : 0u, a.u32[1] < b.u32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmple(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_u32_langext_t>(_a) <= bitCast<simd64_u32_langext_t>(_b));
#else
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] <= b.u32[0] ? 0xffffffff : 0u, a.u32[1] <= b.u32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmpgt(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_u32_langext_t>(_a) > bitCast<simd64_u32_langext_t>(_b));
#else
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] > b.u32[0] ? 0xffffffff : 0u, a.u32[1] > b.u32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u32_cmpge(simd64_t _a, simd64_t _b)
	{
#if BX_SIMD_LANGEXT
		return bitCast<simd64_t>(bitCast<simd64_u32_langext_t>(_a) >= bitCast<simd64_u32_langext_t>(_b));
#else
		const simd64_u32_ref_t a = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b = bitCast<simd64_u32_ref_t>(_b);
		const simd64_u32_ref_t result = { { a.u32[0] >= b.u32[0] ? 0xffffffff : 0u, a.u32[1] >= b.u32[1] ? 0xffffffff : 0u } };
		return bitCast<simd64_t>(result);
#endif // BX_SIMD_LANGEXT
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i16_add(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const int16_t av = int16_t(_a.u64 >> (ii * 16));
			const int16_t bv = int16_t(_b.u64 >> (ii * 16));
			result.u64 |= uint64_t(uint16_t(int16_t(av + bv))) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i16_sub(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const int16_t av = int16_t(_a.u64 >> (ii * 16));
			const int16_t bv = int16_t(_b.u64 >> (ii * 16));
			result.u64 |= uint64_t(uint16_t(int16_t(av - bv))) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i16_mullo(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const int16_t av = int16_t(_a.u64 >> (ii * 16));
			const int16_t bv = int16_t(_b.u64 >> (ii * 16));
			result.u64 |= uint64_t(uint16_t(int16_t(av * bv))) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i16_cmpeq(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const int16_t av = int16_t(_a.u64 >> (ii * 16));
			const int16_t bv = int16_t(_b.u64 >> (ii * 16));
			result.u64 |= uint64_t(av == bv ? uint16_t(0xffff) : uint16_t(0)) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x16_sll(simd64_t _a, int _count)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const uint16_t av = uint16_t(_a.u64 >> (ii * 16));
			result.u64 |= uint64_t(uint16_t(av << _count)) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x16_srl(simd64_t _a, int _count)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const uint16_t av = uint16_t(_a.u64 >> (ii * 16));
			result.u64 |= uint64_t(uint16_t(av >> _count)) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i8_add(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 8; ++ii)
		{
			result.u64 |= uint64_t(uint8_t(int8_t(_a.u64 >> (ii * 8)) + int8_t(_b.u64 >> (ii * 8)))) << (ii * 8);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i8_sub(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 8; ++ii)
		{
			result.u64 |= uint64_t(uint8_t(int8_t(_a.u64 >> (ii * 8)) - int8_t(_b.u64 >> (ii * 8)))) << (ii * 8);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u8_satadd(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 8; ++ii)
		{
			const uint16_t sum = uint16_t(uint8_t(_a.u64 >> (ii * 8))) + uint8_t(_b.u64 >> (ii * 8));
			result.u64 |= uint64_t(sum > 255 ? 255 : uint8_t(sum)) << (ii * 8);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u8_satsub(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 8; ++ii)
		{
			const uint8_t a = uint8_t(_a.u64 >> (ii * 8));
			const uint8_t b = uint8_t(_b.u64 >> (ii * 8));
			result.u64 |= uint64_t(a > b ? uint8_t(a - b) : uint8_t(0)) << (ii * 8);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u16_satadd(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const uint32_t sum = uint32_t(uint16_t(_a.u64 >> (ii * 16))) + uint16_t(_b.u64 >> (ii * 16));
			result.u64 |= uint64_t(sum > 65535 ? 65535 : uint16_t(sum)) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u16_satsub(simd64_t _a, simd64_t _b)
	{
		simd64_t result{ .u64 = 0 };
		for (int ii = 0; ii < 4; ++ii)
		{
			const uint16_t a = uint16_t(_a.u64 >> (ii * 16));
			const uint16_t b = uint16_t(_b.u64 >> (ii * 16));
			result.u64 |= uint64_t(a > b ? uint16_t(a - b) : uint16_t(0)) << (ii * 16);
		}
		return result;
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u64_add(simd64_t _a, simd64_t _b)
	{
		return { .u64 = _a.u64 + _b.u64 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u64_sub(simd64_t _a, simd64_t _b)
	{
		return { .u64 = _a.u64 - _b.u64 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_u64_mul(simd64_t _a, simd64_t _b)
	{
		return { .u64 = _a.u64 * _b.u64 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i64_add(simd64_t _a, simd64_t _b)
	{
		const simd64_i64_ref_t a = bitCast<simd64_i64_ref_t>(_a);
		const simd64_i64_ref_t b = bitCast<simd64_i64_ref_t>(_b);
		const simd64_i64_ref_t result = { a.i64 + b.i64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i64_sub(simd64_t _a, simd64_t _b)
	{
		const simd64_i64_ref_t a = bitCast<simd64_i64_ref_t>(_a);
		const simd64_i64_ref_t b = bitCast<simd64_i64_ref_t>(_b);
		const simd64_i64_ref_t result = { a.i64 - b.i64 };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_and(simd64_t _a, simd64_t _b)
	{
		return { .u64 = _a.u64 & _b.u64 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_andc(simd64_t _a, simd64_t _b)
	{
		return { .u64 = _a.u64 & ~_b.u64 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_or(simd64_t _a, simd64_t _b)
	{
		return { .u64 = _a.u64 | _b.u64 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_orc(simd64_t _a, simd64_t _b)
	{
		return { .u64 = _a.u64 | ~_b.u64 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_xor(simd64_t _a, simd64_t _b)
	{
		return { .u64 = _a.u64 ^ _b.u64 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_not(simd64_t _a)
	{
		return { .u64 = ~_a.u64 };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x64_sll(simd64_t _a, int _count)
	{
		return { .u64 = _a.u64 << _count };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x64_srl(simd64_t _a, int _count)
	{
		return { .u64 = _a.u64 >> _count };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x64_sra(simd64_t _a, int _count)
	{
		const simd64_i64_ref_t a = bitCast<simd64_i64_ref_t>(_a);
		const simd64_i64_ref_t result = { a.i64 >> _count };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_selb(simd64_t _mask, simd64_t _a, simd64_t _b)
	{
		return { .u64 = (_a.u64 & _mask.u64) | (_b.u64 & ~_mask.u64) };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_sels(simd64_t _test, simd64_t _a, simd64_t _b)
	{
		const simd64_i32_ref_t test = bitCast<simd64_i32_ref_t>(_test);
		const simd64_u32_ref_t a    = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t b    = bitCast<simd64_u32_ref_t>(_b);
		const uint32_t m0 = uint32_t(test.i32[0] >> 31);
		const uint32_t m1 = uint32_t(test.i32[1] >> 31);
		const simd64_u32_ref_t result = { { (a.u32[0] & m0) | (b.u32[0] & ~m0), (a.u32[1] & m1) | (b.u32[1] & ~m1) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC bool simd64_test_any_x(simd64_t _test)
	{
		return 0 != (uint32_t(_test.u64) >> 31);
	}

	inline BX_CONSTEXPR_FUNC bool simd64_test_any_y(simd64_t _test)
	{
		return 0 != (uint32_t(_test.u64 >> 32) >> 31);
	}

	inline BX_CONSTEXPR_FUNC bool simd64_test_any_xy(simd64_t _test)
	{
		return 0 != ((uint32_t(_test.u64) | uint32_t(_test.u64 >> 32)) >> 31);
	}

	inline BX_CONSTEXPR_FUNC bool simd64_test_all_x(simd64_t _test)
	{
		return 0 != (uint32_t(_test.u64) >> 31);
	}

	inline BX_CONSTEXPR_FUNC bool simd64_test_all_y(simd64_t _test)
	{
		return 0 != (uint32_t(_test.u64 >> 32) >> 31);
	}

	inline BX_CONSTEXPR_FUNC bool simd64_test_all_xy(simd64_t _test)
	{
		return 0 != ((uint32_t(_test.u64) & uint32_t(_test.u64 >> 32)) >> 31);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x64_rol(simd64_t _a, int _count)
	{
		return { .u64 = (_a.u64 << _count) | (_a.u64 >> (64 - _count)) };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x64_ror(simd64_t _a, int _count)
	{
		return { .u64 = (_a.u64 >> _count) | (_a.u64 << (64 - _count)) };
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x64_cntbits(simd64_t _a)
	{
#if BX_COMPILER_GCC || BX_COMPILER_CLANG
		return { .u64 = uint64_t(__builtin_popcountll(_a.u64) ) };
#else
		const simd32_t lo  { .u32 = uint32_t(_a.u64) };
		const simd32_t hi  { .u32 = uint32_t(_a.u64 >> 32) };
		const simd32_t cntLo = simd32_x32_cntbits(lo);
		const simd32_t cntHi = simd32_x32_cntbits(hi);
		return { .u64 = uint64_t(cntLo.u32) + uint64_t(cntHi.u32) };
#endif // BX_COMPILER_*
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x64_cntlz(simd64_t _a)
	{
#if BX_COMPILER_GCC || BX_COMPILER_CLANG
		return { .u64 = 0 == _a.u64 ? 64 : uint64_t(__builtin_clzll(_a.u64) ) };
#else
		const simd32_t lo  { .u32 = uint32_t(_a.u64) };
		const simd32_t hi  { .u32 = uint32_t(_a.u64 >> 32) };
		const simd32_t cntHi = simd32_x32_cntlz(hi);
		const simd32_t cntLo = simd32_x32_cntlz(lo);
		return { .u64 = uint32_t(_a.u64 >> 32)
			? uint64_t(cntHi.u32)
			: uint64_t(cntLo.u32) + 32
			};
#endif // BX_COMPILER_*
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x64_cnttz(simd64_t _a)
	{
#if BX_COMPILER_GCC || BX_COMPILER_CLANG
		return { .u64 = 0 == _a.u64 ? 64 : uint64_t(__builtin_ctzll(_a.u64) ) };
#else
		const simd32_t lo  { .u32 = uint32_t(_a.u64) };
		const simd32_t hi  { .u32 = uint32_t(_a.u64 >> 32) };
		const simd32_t cntLo = simd32_x32_cnttz(lo);
		const simd32_t cntHi = simd32_x32_cnttz(hi);
		return { .u64 = uint32_t(_a.u64)
			? uint64_t(cntLo.u32)
			: uint64_t(cntHi.u32) + 32
			};
#endif // BX_COMPILER_*
	}

	BX_SIMD_FORCE_INLINE simd64_t simd64_ldu(const void* _ptr)
	{
		return simd64_ld(_ptr);
	}

	BX_SIMD_FORCE_INLINE void simd64_stu(void* _ptr, simd64_t _a)
	{
		simd64_st(_ptr, _a);
	}

	inline BX_CONSTEXPR_FUNC bool simd64_test_any(simd64_t _test)
	{
		return simd64_test_any_xy(_test);
	}

	inline BX_CONSTEXPR_FUNC bool simd64_test_all(simd64_t _test)
	{
		return simd64_test_all_xy(_test);
	}

	inline BX_CONSTEXPR_FUNC bool simd64_test_zero(simd64_t _a, simd64_t _b)
	{
		return 0 == (_a.u64 & _b.u64);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x32_sll(simd64_t _a, int _count)
	{
		const simd64_u32_ref_t a      = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t result = { { a.u32[0] << _count, a.u32[1] << _count } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x32_srl(simd64_t _a, int _count)
	{
		const simd64_u32_ref_t a      = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t result = { { a.u32[0] >> _count, a.u32[1] >> _count } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x32_sra(simd64_t _a, int _count)
	{
		const simd64_i32_ref_t a      = bitCast<simd64_i32_ref_t>(_a);
		const simd64_i32_ref_t result = { { a.i32[0] >> _count, a.i32[1] >> _count } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x32_sll(simd64_t _a, simd64_t _count)
	{
		const simd64_u32_ref_t a      = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t c      = bitCast<simd64_u32_ref_t>(_count);
		const simd64_u32_ref_t result = { { a.u32[0] << (c.u32[0] & 31u), a.u32[1] << (c.u32[1] & 31u) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x32_srl(simd64_t _a, simd64_t _count)
	{
		const simd64_u32_ref_t a      = bitCast<simd64_u32_ref_t>(_a);
		const simd64_u32_ref_t c      = bitCast<simd64_u32_ref_t>(_count);
		const simd64_u32_ref_t result = { { a.u32[0] >> (c.u32[0] & 31u), a.u32[1] >> (c.u32[1] & 31u) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_x32_sra(simd64_t _a, simd64_t _count)
	{
		const simd64_i32_ref_t a      = bitCast<simd64_i32_ref_t>(_a);
		const simd64_u32_ref_t c      = bitCast<simd64_u32_ref_t>(_count);
		const simd64_i32_ref_t result = { { a.i32[0] >> int32_t(c.u32[0] & 31u), a.i32[1] >> int32_t(c.u32[1] & 31u) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_ftoi_trunc(simd64_t _a)
	{
		const simd64_f32_ref_t a      = bitCast<simd64_f32_ref_t>(_a);
		const simd64_i32_ref_t result = { { int32_t(a.f32[0]), int32_t(a.f32[1]) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_f32_ftoi_round(simd64_t _a)
	{
		const simd64_f32_ref_t a      = bitCast<simd64_f32_ref_t>(_a);
		const simd64_i32_ref_t result = { { int32_t(round(a.f32[0]) ), int32_t(round(a.f32[1]) ) } };
		return bitCast<simd64_t>(result);
	}

	inline BX_CONSTEXPR_FUNC simd64_t simd64_i32_itof(simd64_t _a)
	{
		const simd64_i32_ref_t a      = bitCast<simd64_i32_ref_t>(_a);
		const simd64_f32_ref_t result = { { float(a.i32[0]), float(a.i32[1]) } };
		return bitCast<simd64_t>(result);
	}

} // namespace bx
